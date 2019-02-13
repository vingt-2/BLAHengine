#include "GameComponent.h"

#include <Engine/Game/GameObject.h>

using namespace BLAengine; 

GameComponent::GameComponent(GameObjectReference parentObject) : m_parentObject(parentObject)
{
    if (parentObject.IsValid())
    {
        parentObject->AddComponent(this);
        m_parentObject = parentObject;
    }
}


const ObjectTransform& GameComponent::GetObjectTransform() const
{
    return m_parentObject->GetTransform();
}
