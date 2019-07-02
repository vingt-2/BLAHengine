#include "GameComponent.h"

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

