#include "GameComponent.h"
#include "../GameObject.h"
using namespace BLAengine; 

const Transform&  BLAengine::GameComponent::GetObjectTransform()
{
    return m_parentObject->GetTransform();
}
