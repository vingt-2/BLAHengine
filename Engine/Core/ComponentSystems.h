#pragma once
#include "StdInclude.h"
#include "BLAStringID.h"
#include "Core/GameComponent.h"
#include "Core/GameObject.h"
#include "Core/ComponentContainers.h"

// TODO: Crash if dependant doesn't exist !!

#define NOTHING(i)
#define InputComponents(...) __VA_ARGS__
#define OutputComponents(...) __VA_ARGS__
#define Dependencies(...) __VA_ARGS__
#define IND_DEP(X) BlaStringId(#X)

#define DeclareComponentSystem(ProjectName, SystemName, input_list, output_list)   \
    typedef TypedComponentSystem<IOTS<EXPAND(input_list)>, IOTS<EXPAND(output_list)>> SystemName;

#define BeginComponentSystemDeclaration(ProjectName, SystemName, input_list, output_list)   \
    class BLA_COMPONENT_API_IMPORT_EXPORT_SELECT(ProjectName) SystemName : public TypedComponentSystem<IOTS<EXPAND(input_list)>, IOTS<EXPAND(output_list)>> {

#define EndComponentSystemDeclaration() };

#define RegisterComponentSystem(SystemName, ...) \
    SystemName g_##SystemName(BlaStringId(#SystemName), blaVector<blaStringId>({ ENUM_WITH_PREFIX_MACRO(IND_DEP, NOTHING, EXPAND(__VA_ARGS__))}));

namespace BLA
{
    typedef const blaVector<blaStringId> SystemObjectsIterator;

    template <class T>
    constexpr std::tuple<InputComponents<T>> GetInputComponentsIterator(ComponentSystemIOInterface& systemIOInterface)
    {   
        InputComponents<T> componentIterator = systemIOInterface.GetInputComponents<T>();

        return std::make_tuple(componentIterator);
    }

    template <class T>
    constexpr std::tuple<OutputComponents<T>> GetOutputComponentsIterator(ComponentSystemIOInterface& systemIOInterface)
    {
        OutputComponents<T> componentIterator = systemIOInterface.GetOutputComponents<T>();

        return std::make_tuple(componentIterator);
    }

    template<class I>
    constexpr blaStringId GetInputComponentName()
    {
        static_assert(std::is_base_of<GameComponent, I>::value, "Invalid Input Component, this class does not derive from GameComponent");
        return I::ms_componentDescriptor.GetName();
    }

    template<class O>
    constexpr blaStringId GetOutputComponentName()
    {
        static_assert(std::is_base_of<GameComponent, O>::value, "Invalid Output Component, this class does not derive from GameComponent");
        return O::ms_componentDescriptor.GetName();
    }

    class BLACORE_API ComponentSystem
    {
    public:
        virtual void ExecuteSystem(SystemObjectsIterator&, ComponentSystemIOInterface&) const = 0;

        blaStringId GetName() const { return m_systemName; }

        const blaVector<blaStringId>& GetInputComponents() const { return m_inputComponents; }
        const blaVector<blaStringId>& GetOutputComponents() const { return m_outputComponents; }
        const blaVector<blaStringId>& GetSystemDependencies() const { return m_systemDependencies; }

        virtual void FinalizeLoad() = 0;

    protected:
        blaStringId m_systemName;
        blaVector<blaStringId> m_inputComponents;
        blaVector<blaStringId> m_outputComponents;
        blaVector<blaStringId> m_systemDependencies;
    };

    class BLACORE_API ComponentSystemsRegistry
    {
        BLA_DECLARE_SINGLETON(ComponentSystemsRegistry);
        friend class ComponentLibrariesManager;
        friend class ComponentSystemsScheduler;

        blaStringId m_currentRegisteringLibrary;

        typedef blaMap<blaStringId, ComponentSystem*> SystemsInLibraries;

        blaMap<blaStringId, SystemsInLibraries> m_componentSystemsPerLibrary;

    public:
        void FinalizeLoad();
        blaVector<blaPair<blaStringId, const ComponentSystem*>> GetAllAvailableSystems() const;
        const ComponentSystem* GetSystemPointer(blaStringId system) const;
        void __RegisterComponentSystem(ComponentSystem* componentSystem);
    };

    // Dummy class to serve as Input Output Template Separator (used to separate list of variadic template arguments)
    template <class...>
    class IOTS {};

    // Declaration of a template
    template<class Input, class Output>
    class TypedComponentSystem;

    template<class... Is, class... Os>
    class TypedComponentSystem<IOTS<Is...>, IOTS<Os...>> : public ComponentSystem
    {
    public:

        TypedComponentSystem(blaStringId name, blaVector<blaStringId> systemDependencies)
        {
            m_systemName = name;
            m_systemDependencies = systemDependencies;

            ComponentSystemsRegistry* systemRegistry = ComponentSystemsRegistry::GetSingletonInstance();
            if(!systemRegistry)
            {
                systemRegistry = ComponentSystemsRegistry::AssignAndReturnSingletonInstance(new ComponentSystemsRegistry());
            }
            systemRegistry->__RegisterComponentSystem(this);
        }

        void FinalizeLoad() override
        {
            RegisterInputComponents();
            RegisterOutputComponents();
        }

        // Throw compile time asserts if ComponentTypes include something that is not a GameComponent
        static_assert(std::conjunction_v<std::is_base_of<GameComponent, Is>..., std::is_base_of<GameComponent, Os>...>,
            "Not all inputs/outputs to this Component System derive from GameComponents");

        void ExecuteSystem(SystemObjectsIterator& systemObjects, ComponentSystemIOInterface& systemIOInterface) const override
        {
            auto executeArguments = std::tuple_cat(std::make_tuple(systemObjects), GetSystemIOIterators(systemIOInterface));
            std::apply(&Execute, executeArguments);
        }

    protected:

        static void Execute(SystemObjectsIterator& systemObjects, InputComponents<Is>..., OutputComponents<Os> ...);

        constexpr std::tuple<InputComponents<Is>..., OutputComponents<Os>...> GetSystemIOIterators(ComponentSystemIOInterface& systemIOInterface) const
        {
            return std::tuple_cat(GetInputComponentsIterator<Is>(systemIOInterface)..., GetOutputComponentsIterator<Os>(systemIOInterface)...);
        }
        constexpr void RegisterInputComponents()
        {
            m_inputComponents = { GetInputComponentName<Is>()... };
        }

        constexpr void RegisterOutputComponents()
        {
            m_outputComponents = { GetOutputComponentName<Os>()... };
        }
    };

    // The root system does nothing. A system that has no dependencies on any actual system depends on the root
    DeclareComponentSystem(BLAEngine, RootSystem, InputComponents(), OutputComponents())
}

//
// This whole component system shenanigan can also be implemented exclusively with macros.
// It would actually be a little more powerful and would end up generating less code
// I like the idea of not having to create a specific polymorphic class for each system though,
// No two system can have the same combination of I/O/D, so not relying on inheritence forces disambiguity and uniqueness
// Systems are stateless. Add a state object parameter to a system ?
// 
//#define NOTHING(i) 
//#define _ENUM_INPUTS_MACRO(x) InputComponents< x > input##x##s
//#define _ENUM_OUTPUTS_MACRO(x) InputComponents< x > output##x##s
//
//#define INPUT_COMPONENTS(...) __VA_ARGS__
//#define OUTPUT_COMPONENTS(...) __VA_ARGS__
//
//#define BEGIN_SYSTEM_DECLARATION(ProjectName, SystemName, input_list, output_list)                  \
//    class SystemName : TypedComponentSystem<IOTS<EXPAND(input_list)>, IOTS<EXPAND(output_list)>>{   \
//    static void Execute(SystemObjectsIterator& systemObjects,                                       \
//        ENUM_WITH_PREFIX_MACRO(_ENUM_INPUTS_MACRO, NOTHING, input_list ),                           \
//        ENUM_WITH_PREFIX_MACRO(_ENUM_OUTPUTS_MACRO, NOTHING, output_list )){                        
//
//#define END_COMPUTE_BODY_BEGIN_SYSTEM_MEMBERS() }                                                       \
//    public:                                                                                             \
//    void ExecuteSystem(SystemObjectsIterator& systemObjects) override                                   \
//    {                                                                                                   \
//        auto executeArguments = std::tuple_cat(std::make_tuple(systemObjects), GetSystemIOIterators()); \
//        std::apply(&Execute, executeArguments);                                                         \
//    }                                                                                                   
//
//#define END_SYSTEM_DECLARATION() };