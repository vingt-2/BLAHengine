#include "GameComponent.h"
#include "../GameObject.h"

using namespace BLAengine; 

GameComponent::GameComponent(GameObject* parentObject) : m_parentObject(parentObject)
{
    if (parentObject != nullptr)
    {
        parentObject->AddComponent(this);
        m_parentObject = parentObject;
    }
}


const ObjectTransform& GameComponent::GetObjectTransform() const
{
    return m_parentObject->GetTransform();
}
