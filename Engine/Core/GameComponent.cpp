// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "GameComponent.h"

#include "System/Console.h"

namespace BLA::Core
{
    ComponentDescriptor ms_emptyDescriptor{ GameComponent::InitReflection };

    GameComponent::GameComponent(GameObject parentObject) : m_parentObject(parentObject)
    {}

    GameComponent::~GameComponent()
    {}

    void InitEmptyDesc(ComponentDescriptor* d) { d->m_typeID = INVALID_COMPONENT_ID; };
    ComponentDescriptor g_emptyDescriptor{ InitEmptyDesc };

    const ComponentDescriptor& GameComponent::GetComponentDescriptor() const
    {
        return g_emptyDescriptor;
    }

    void GameComponentRegistry::__RegisterComponent(blaStringId name, GameComponentRegistryEntry::ComponentFactory factory)
    {
        m_componentsPerLibraries[m_currentRegisteringLibrary].push_back(GameComponentRegistry::GameComponentRegistryEntry(name, factory));
    }

    std::vector<blaString> GameComponentRegistry::ListComponentNames()
    {
        std::vector<blaString> names;
        for (auto p : m_componentsPerLibraries)
        {
            for (auto e : p.second)
            {
                names.push_back(blaString(e.GetName())); // Did you know? At some point, in release, this will just be a number
                // And we're not just a number.
            }
        }
        return names;
    }

    const ComponentDescriptor& GameComponentRegistry::GetComponentDescriptor(blaStringId name)
    {
        for (auto p : m_componentsPerLibraries)
        {
            for (auto e : p.second)
            {
                if (e.GetName() == name)
                {
                    GameObject r;
                    return e.CreateComponent(r)->GetComponentDescriptor();
                }
            }
        }
        return g_emptyDescriptor;
    }

    GameComponentRegistry::GameComponentRegistry() : m_currentRegisteringLibrary(BlaStringId("Native")) {}

    GameComponent* GameComponentRegistry::__CreateComponent(GameComponentID componentName, GameObject objRef)
    {
        for (auto p : m_componentsPerLibraries)
        {
            CompEntries::iterator findIt = std::find_if(p.second.begin(), p.second.end(), [componentName](GameComponentRegistryEntry e) { return e.GetName() == componentName; });
            if (findIt != p.second.end())
            {
                return findIt->CreateComponent(objRef);
            }
        }

        return nullptr;
    }

    void GameComponentRegistry::UnloadLibraryComponents(blaStringId libraryId)
    {
        // TODO: Obviously, we need to clean the scene as well ...
        for (auto p : m_componentsPerLibraries)
        {
            if (p.first == libraryId)
            {
                p.second.clear();
                m_componentsPerLibraries.erase(p.first);
            }
        }
    }

    template <class T>
    T* GameComponentRegistry::__CreateComponent(GameObject objRef)
    {
        return static_cast<T*>(T::Factory(objRef));
    }

    BLA_IMPLEMENT_SINGLETON(GameComponentRegistry);

    DefineConsoleCommand(int, GetComponentCount, int a)
    {
        return (int)GameComponentRegistry::GetSingletonInstance()->ListComponentNames().size();
    }
}

#define TO_STRING(x) #x

DefineConsoleCommand(void, testMacro)
{
}

DefineConsoleCommand(void, GetComponentList, int b)
{
    for (auto componentName : BLA::Core::GameComponentRegistry::GetSingletonInstance()->ListComponentNames())
    {
        BLA::Console::LogMessage(componentName);
    }
}