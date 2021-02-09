// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Core/BehaviorComponent.h"

#include "System/Console.h"

namespace BLA::Core
{

    BLA_IMPLEMENT_SINGLETON(ComponentSystemsRegistry)

    RootSystem g_RootSystem(BlaStringId("RootSystem"), blaVector<blaStringId>());
    template <>
    void RootSystem::Execute(SystemObjectsIterator& systemObjects) {  }

    void ComponentSystemsRegistry::FinalizeLoad()
    {
        for (auto s : m_componentSystemsPerLibrary[m_currentRegisteringLibrary])
        {
            s.second->FinalizeLoad();
        }
    }

    blaVector<blaPair<blaStringId, const ComponentSystem*>> ComponentSystemsRegistry::GetAllAvailableSystems() const
    {
        blaVector<blaPair<blaStringId, const ComponentSystem*>> v;

        for (auto le : m_componentSystemsPerLibrary)
        {
            for (auto se : le.second)
            {
                v.push_back(se);
            }
        }
        return v;
    }

    const ComponentSystem* ComponentSystemsRegistry::GetSystemPointer(blaStringId system) const
    {
        for (auto le : m_componentSystemsPerLibrary)
        {
            SystemsInLibraries::iterator systemIt = le.second.find(system);
            if (systemIt != le.second.end())
            {
                return systemIt->second;
            }
        }

        return nullptr;
    }

    void ComponentSystemsRegistry::__RegisterComponentSystem(ComponentSystem* componentSystem)
    {
        if (m_componentSystemsPerLibrary.find(m_currentRegisteringLibrary) == m_componentSystemsPerLibrary.end())
        {
            m_componentSystemsPerLibrary.insert(std::make_pair(m_currentRegisteringLibrary, SystemsInLibraries()));
        }

        if (m_componentSystemsPerLibrary[m_currentRegisteringLibrary].find(componentSystem->GetName()) == m_componentSystemsPerLibrary[m_currentRegisteringLibrary].end())
        {
            m_componentSystemsPerLibrary[m_currentRegisteringLibrary][componentSystem->GetName()] = componentSystem;
        }
        else
        {
            BLA_ASSERT(false);
        }
    }
}
