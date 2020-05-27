#include "ComponentContainers.h"

using namespace BLA;

GameComponent* ComponentContainer::AddComponent(GameObjectID object, blaStringId componentId)
{
    GameComponent* gc = GameComponentRegistry::GetSingletonInstance()->__CreateComponent(componentId, object);

    auto it = m_components.find(componentId);

    if (it == m_components.end())
    {
        m_components[componentId] = blaHashMap<GameObjectID, GameComponent*, GameObjectID::Hasher>();
    }

    // TODO: Obviously will have to check we don't already have one ...
    m_components[componentId][object] = gc;

    return gc;
}

bool ComponentContainer::RemoveComponent(GameObjectID object, blaStringId componentId)
{
    auto compIt = m_components.find(componentId);

    if (compIt == m_components.end())
    {
        return false;
    }

    auto objIt = compIt->second.find(object);
	
    if (objIt == compIt->second.end())
    {
        return false;
    }
    compIt->second.erase(objIt);

    return true;
}

void ComponentContainer::Clear()
{
    for (auto c : m_components)
    {
        for (auto o : c.second)
        {
            o.second->Shutdown();
        }
    }

    for (auto c : m_components)
    {
        for (auto o : c.second)
        {
            delete o.second;
        }

        c.second.clear();
    }

    m_components.clear();
}

GameComponent* ComponentContainer::GetComponentPerObject(GameComponentID componentId, GameObjectID obj)
{
    auto& componentPool = m_components[componentId];

    auto it = componentPool.find(obj);

    if (it != componentPool.end())
    {
        return it->second;
    }
    return nullptr;
}

blaVector<GameComponent*> ComponentContainer::GetComponentsPerObject(GameObjectID obj)
{
    blaVector<GameComponent*> ret;

    for (auto c : m_components)
    {
        auto it = c.second.find(obj);

        if (it != c.second.end())
        {
            ret.push_back(it->second);
        }
    }
    return ret;
}