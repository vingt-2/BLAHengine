#include "GameComponent.h"

#include <Engine/System/Console.h>

using namespace BLAengine; 

ComponentReflection::ComponentDescriptor ms_emptyDescriptor{GameComponent::InitReflection};

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

ObjectTransform& GameComponent::GetObjectTransform()
{
	return m_parentObject->GetTransform();
}

ObjectTransform& GameComponent::GetLocalObjectTransform() 
{
	return m_parentObject->GetLocalTransform();
}

const ObjectTransform& GameComponent::GetObjectTransform() const
{
    return m_parentObject->GetTransform();
}

const ObjectTransform& GameComponent::GetLocalObjectTransform() const
{
    return m_parentObject->GetLocalTransform();
}

void InitEmptyDesc(ComponentReflection::ComponentDescriptor* d) { d->m_typeName = "Unknown Component"; };
ComponentReflection::ComponentDescriptor g_emptyDescriptor{InitEmptyDesc};
const ComponentReflection::ComponentDescriptor& GameComponent::GetComponentDescriptor() const
{
	return g_emptyDescriptor;
}

void GameComponentManager::__RegisterComponent(const blaString& name, ComponentManagerEntry::ComponentFactory factory)
{
    m_componentEntries.push_back(GameComponentManager::ComponentManagerEntry(name, factory));
}

std::vector<blaString> GameComponentManager::ListComponentNames()
{
    std::vector<blaString> names;
    for(auto e : m_componentEntries)
    {
        names.push_back(e.GetName());
    }
    return names;
}

const ComponentReflection::ComponentDescriptor& GameComponentManager::GetComponentDescriptor(blaString name)
{
	for (auto e : m_componentEntries)
	{
		if(e.GetName() == name)
		{
			GameObjectReference r;
			return e.CreateComponent(r)->GetComponentDescriptor();
		}
	}
	return g_emptyDescriptor;
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

BLA_CONSOLE_COMMAND(int, GetComponentCount)
{
    return GameComponentManager::GetSingletonInstance()->ListComponentNames().size();
}

BLA_CONSOLE_COMMAND(int, InspectComponent, blaString componentName)
{
	auto manager = GameComponentManager::GetSingletonInstance();
	blaString inspection = manager->GetComponentDescriptor(componentName).ToString(nullptr, 0);
	for(auto s : SplitString<blaString>(inspection, "\n"))
	{
		Console::LogMessage(s);
	}
	return 0;
}