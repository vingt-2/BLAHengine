#include "Core/BehaviorComponent.h"

#include "System/Console.h"

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(ComponentSystemsRegistry)

void ComponentSystemsRegistry::FinalizeLoad()
{
    for (auto s : m_componentSystems)
    {
        s.second->FinalizeLoad();
    }
}

void ComponentSystemsRegistry::__RegisterComponentSystem(ComponentSystem* componentSystem)
{
    if(m_componentSystems.find(componentSystem->GetName()) == m_componentSystems.end())
    {
        m_componentSystems[componentSystem->GetName()] = componentSystem;
    }
    else
    {
        BLA_ASSERT(false);
    }
}