#pragma once
#include "StdInclude.h"
#include "BLAStringID.h"
#include "Core/GameObject.h"
#include "BLASingleton.h"
#include "Core/InspectableVariables.h"
#include "SillyMacros.h"

#define INVALID_COMPONENT_ID GameComponentID("", 0)

// Todo: Support component polymorphism ?
namespace BLAengine
{
    struct ComponentDescriptor : BLAInspectableVariables::ExposedVarTypeDescriptor
    {
        struct ExposedMember
        {
            blaStringId m_name;
            size_t m_offset;
            ExposedVarTypeDescriptor* m_type;
        };

        blaVector<ExposedMember> m_members;

        ComponentDescriptor(void(*init)(ComponentDescriptor*)) : ExposedVarTypeDescriptor{ BlaStringId(""), 0 }
        {
            init(this);
        }

        ComponentDescriptor(const char* name, size_t size, const std::initializer_list<ExposedMember>& init) : ExposedVarTypeDescriptor{ BlaStringId(""), 0 }, m_members{ init }
        {}

        BLACORE_API void Serialize(void* obj, BLASerializeWriter* writer) const override;

        struct TypedDeserializer : Deserializer
        {
            TypedDeserializer(void* obj) : Deserializer(obj) {}

            virtual Deserializer* StartObject();
            virtual Deserializer* Key(const char* str, size_t  length, bool copy);
            virtual Deserializer* EndObject(size_t  memberCount);

            void SetComponentDescriptor(const ComponentDescriptor* componentDescriptor);

        private:
            const ComponentDescriptor* m_componentDescriptor;
            int m_stage = 0;
            int m_parsingMember = 0;
        };

        BLACORE_API Deserializer* GetDeserializer(void* obj) const override;
    };

    class GameComponentRegistry;
    class GameObject;
    class BLACORE_API GameComponent
    {
        //TODO: Remove friend class Scene , used to call the private destructor
        friend class Scene;
    public:
        friend GameComponentRegistry;

        virtual void Init() {};
        virtual void Shutdown() {};
        virtual void Update() {};

        GameObject GetOwnerObject() const { return m_parentObject; }

        virtual const ComponentDescriptor& GetComponentDescriptor() const;

        static void InitReflection(ComponentDescriptor* typeDesc) {}

    protected:
        GameComponent(GameObject parentObject);
        virtual ~GameComponent();

    private:
        const GameObject m_parentObject;
    };

#define IF_INCLUDING_EXTERN_COMPONENT_1 __declspec ( dllimport )
#define IF_INCLUDING_EXTERN_COMPONENT_0 __declspec ( dllexport )

#define BLA_COMPONENT_API_IMPORT_EXPORT_SELECT(ProjectName) \
    __SILLY_MACRO__CAT(IF_INCLUDING_EXTERN_COMPONENT_,__SILLY_MACRO__NOT_EQUAL(ProjectName, BLA_PROJECT_NAME))

#define BEGIN_COMPONENT_DECLARATION(ProjectName, ComponentName)                                             \
    class BLA_COMPONENT_API_IMPORT_EXPORT_SELECT(ProjectName) ComponentName : public GameComponent {        \
    friend struct BLAInspectableVariables::DefaultResolver;                                                 \
    static void InitReflection(ComponentDescriptor*);														\
    static GameComponent* Factory(GameObject objR) { return new ComponentName(objR); }                      \
	virtual const ComponentDescriptor& GetComponentDescriptor() const {										\
		return ms_componentDescriptor; }																	\
    ComponentName(GameObject parentObject);                                                                 \
    public:                                                                                                 \
    static const ComponentDescriptor ms_componentDescriptor;

#define END_DECLARATION() };

#define BEGIN_COMPONENT_DESCRIPTION(ComponentName)														    \
    ComponentName::ComponentName(GameObject parentObject) : GameComponent(parentObject) {}                  \
    const ComponentDescriptor ComponentName::ms_componentDescriptor{ComponentName::InitReflection};		    \
    void ComponentName::InitReflection(ComponentDescriptor* typeDesc) {									    \
        using T = ComponentName;                                                                            \
        typeDesc->m_typeName = BlaStringId(#ComponentName);                                                 \
        typeDesc->size = sizeof(T);                                                                         \
        typeDesc->m_members = {

#define EXPOSE(name)																			            \
            {BlaStringId(#name), offsetof(T, name), BLAInspectableVariables::TypeResolver<decltype(T::name)>::get()},

#define END_DESCRIPTION()                                                                                   \
        };                                                                                                  \
        GameComponentRegistry* manager = GameComponentRegistry::GetSingletonInstance();                     \
        if(!manager)                                                                                        \
            manager = GameComponentRegistry::AssignAndReturnSingletonInstance(new GameComponentRegistry()); \
        manager->__RegisterComponent(typeDesc->m_typeName, T::Factory);                                     \
    }

    //Todo: Don't return GameComponent raw pointers... but references ?
    class BLACORE_API GameComponentRegistry
    {
        BLA_DECLARE_SINGLETON(GameComponentRegistry)

        friend class GameComponent;
        friend class ComponentLibrariesManager;

        class GameComponentRegistryEntry
        {
            friend class GameComponentRegistry;
        public:
            typedef GameComponent* (*ComponentFactory)(GameObject);
            GameComponentRegistryEntry(blaStringId name, ComponentFactory factory) : m_componentName(name), m_factory(factory) {}

            blaStringId GetName() const { return m_componentName; }

        private:
            GameComponent* CreateComponent(GameObject parentObj) const { return m_factory(parentObj); }
            blaStringId m_componentName;
            ComponentFactory m_factory;
        };

        typedef blaVector<GameComponentRegistryEntry> CompEntries;

        blaHashMap<blaStringId, CompEntries, blaStringId::Hasher> m_componentsPerLibraries;
        blaStringId m_currentRegisteringLibrary;

        void UnloadLibraryComponent(blaStringId libraryId);

    public:

        GameComponent* __CreateComponent(blaStringId componentName, GameObject objRef);

        template<class T>
        T* __CreateComponent(GameObject objRef);

        void __RegisterComponent(blaStringId name, GameComponentRegistryEntry::ComponentFactory factory);

        blaVector<blaString> ListComponentNames();
        const ComponentDescriptor& GetComponentDescriptor(blaStringId name);

        GameComponentRegistry(): m_currentRegisteringLibrary(BlaStringId("Native")) {}
    };

    template <class T>
    T* GameComponentRegistry::__CreateComponent(GameObject objRef)
    {
        return static_cast<T*>(T::Factory(objRef));
    }
}

/*
 *  Component declaration interface:
 *  BLA_DECLARE_COMPONENT(COMPONENT_NAME)
 *      blaVec3 m_someValue;
 *      blaVector<blaU32> m_someVector;
 *
 *      void SomeComponentPrivateFunction();
 *  END_COMPONENT_DECLARATION()
 */
