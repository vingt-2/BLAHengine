#include "IKComponent.h"

#include <Engine/Animation/IK/IKTools.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(IKComponent)

END_DESCRIPTION()

IKComponent::IKComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_ikChain(nullptr)
{}

IKComponent::~IKComponent()
{
    delete m_ikChain;
}

void IKComponent::Update()
{

}