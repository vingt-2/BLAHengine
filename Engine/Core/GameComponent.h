// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "BLAStringID.h"
#include "Core/GameObject.h"
#include "BLASingleton.h"
#include "Core/InspectableVariables.h"
#include "SillyMacros.h"

#define INVALID_COMPONENT_ID Core::GameComponentID("", 0)

/*
 *  Component declaration interface:
 *  BLA_DECLARE_COMPONENT(COMPONENT_NAME)
 *      blaVec3 m_someValue;
 *      blaVector<blaU32> m_someVector;
 *
 *      void SomeComponentPrivateFunction();
 *  END_COMPONENT_DECLARATION()
 */

#define IF_INCLUDING_EXTERN_COMPONENT_1 __declspec ( dllimport )
#define IF_INCLUDING_EXTERN_COMPONENT_0 __declspec ( dllexport )

#define BLA_EXPORT(ProjectName) \
    __SILLY_MACRO__CAT(IF_INCLUDING_EXTERN_COMPONENT_,__SILLY_MACRO__NOT_EQUAL(ProjectName, BLA_PROJECT_NAME))

#define BeginComponentDeclaration(ProjectName, ComponentName)                                                                       \
    class ComponentName : public BLA::Core::GameComponent {                                                                         \
    friend struct BLA::Core::InspectableVariables::DefaultResolver;                                                                 \
    BLA_EXPORT(ProjectName) static void InitReflection(BLA::Core::ComponentDescriptor*);								            \
    BLA_EXPORT(ProjectName) static BLA::Core::GameComponent* Factory(BLA::Core::GameObject obj) { return new ComponentName(obj); }  \
    BLA_EXPORT(ProjectName) ComponentName(BLA::Core::GameObject parentObject);                                                      \
    public:                                                                                                                         \
	BLA_EXPORT(ProjectName) virtual const BLA::Core::ComponentDescriptor& GetComponentDescriptor() const {		                    \
		return ms_componentDescriptor; }                                                                                            \
    BLA_EXPORT(ProjectName) static const BLA::Core::ComponentDescriptor ms_componentDescriptor;                                                

#define EndComponentDeclaration() };

#define BeginComponentDescription(ComponentName)														         \
    ComponentName::ComponentName(BLA::Core::GameObject parentObject) : BLA::Core::GameComponent(parentObject) {} \
    const BLA::Core::ComponentDescriptor ComponentName::ms_componentDescriptor{ComponentName::InitReflection};   \
    void ComponentName::InitReflection(BLA::Core::ComponentDescriptor* typeDesc) {                               \
        using T = ComponentName;                                                                                 \
        typeDesc->m_typeID = BlaStringId(#ComponentName);                                                        \
        typeDesc->size = sizeof(T);                                                                              \
        typeDesc->m_members = {

#define Expose(name)																			            \
            {BlaStringId(#name), offsetof(T, name), BLA::Core::InspectableVariables::TypeResolver<decltype(T::name)>::GetDescriptor()},

#define EndComponentDescription()                                                                                                   \
        };                                                                                                                          \
        BLA::Core::GameComponentRegistry* registry = BLA::Core::GameComponentRegistry::GetSingletonInstance();                      \
        if(!registry)                                                                                                               \
            registry = BLA::Core::GameComponentRegistry::AssignAndReturnSingletonInstance(new BLA::Core::GameComponentRegistry());  \
        registry->__RegisterComponent(typeDesc->m_typeID, T::Factory);                                                              \
    }

// Todo: Support component polymorphism ?
namespace BLA
{
    namespace Core
    {
        struct ComponentDescriptor : InspectableVariables::ExposedVarTypeDescriptor
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
        };

        class GameComponentRegistry;
        class BLACORE_API GameComponent // TODO: Make it an interface and make sure all components implement it... Later: Really, why ?
        {
            friend class ComponentContainer;
        public:
            friend class GameComponentRegistry;

            virtual void Init() {};
            virtual void Shutdown() {};

            GameObject GetOwnerObject() const { return m_parentObject; }

            virtual const ComponentDescriptor& GetComponentDescriptor() const;

            static void InitReflection(ComponentDescriptor* typeDesc) {}

        protected:
            GameComponent(GameObject parentObject);
            virtual ~GameComponent();

        private:
            const GameObject m_parentObject;
        };


        class GameComponentRegistry
        {
            // TODO: Make it so it should be access only and components outside of native assert if nullptr instead of creating the registry
            BLA_DECLARE_EXPORTED_SINGLETON(GameComponentRegistry)

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

            blaMap<blaStringId, CompEntries> m_componentsPerLibraries;
            blaStringId m_currentRegisteringLibrary;

            void UnloadLibraryComponents(blaStringId libraryId);

        public:

            BLACORE_API GameComponent* __CreateComponent(blaStringId componentName, GameObject objRef);

            template<class T>
            BLACORE_API T* __CreateComponent(GameObject objRef);

            BLACORE_API void __RegisterComponent(blaStringId name, GameComponentRegistryEntry::ComponentFactory factory);

            BLACORE_API blaVector<blaString> ListComponentNames();
            BLACORE_API const ComponentDescriptor& GetComponentDescriptor(blaStringId name);

            BLACORE_API GameComponentRegistry();
        };
    }
}