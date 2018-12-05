#include "GameComponent.h"
#include "../GameObject.h"
using namespace BLAengine; 

const ObjectTransform& GameComponent::GetObjectTransform()
{
    return m_parentObject->GetTransform();
}
