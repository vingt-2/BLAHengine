#include "IKComponent.h"

#include <Engine/Game/Animation/IK/IKTools.h>

using namespace BLAengine;

IKComponent::IKComponent(GameObject* parentObject) :
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