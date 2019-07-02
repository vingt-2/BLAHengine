#include "IKComponent.h"

#include <Engine/Animation/IK/IKTools.h>

using namespace BLAengine;

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