#include "GameComponent.h"

#include <Engine/System/Console.h>

using namespace BLAengine; 

GameComponent::GameComponent(GameObjectReference parentObject) : m_parentObject(parentObject)
{
    if (parentObject.IsValid())
    {
        parentObject->AddComponent(this);
        m_parentObject = parentObject;
    }
}

GameComponent::~GameComponent()
{
    m_parentObject = GameObjectReference::InvalidReference();
}

const ObjectTransform& GameComponent::GetObjectTransform() const
{
    return m_parentObject->GetTransform();
}

const ObjectTransform& GameComponent::GetLocalObjectTransform() const
{
    return m_parentObject->GetLocalTransform();
}

void GameComponentManager::__RegisterComponent(const blaString& name, ComponentManagerEntry::ComponentFactory factory)
{
    m_componentEntries.push_back(GameComponentManager::ComponentManagerEntry(name, factory));
}

std::vector<blaString> GameComponentManager::GetComponents()
{
    std::vector<blaString> names;
    for(auto e : m_componentEntries)
    {
        names.push_back(e.GetName());
    }
    return names;
}

GameComponent* GameComponentManager::CreateComponent(const blaString& componentName, GameObjectReference objRef)
{
    CompEntries::iterator findIt = std::find_if(m_componentEntries.begin(), m_componentEntries.begin(), [componentName](ComponentManagerEntry e) { return e.GetName() == componentName; });
    if(findIt != m_componentEntries.end())
    {
        return findIt->CreateComponent(objRef);
    }

    return nullptr;
}

BLA_IMPLEMENT_SINGLETON(GameComponentManager);

BLA_CONSOLE_COMMAND(int, GetComponentCount, int a)
{
    return GameComponentManager::GetSingletonInstance()->GetComponents().size();
}