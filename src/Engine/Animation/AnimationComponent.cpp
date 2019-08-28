#include "AnimationComponent.h"

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(AnimationComponent)
END_DESCRIPTION()

AnimationComponent::AnimationComponent(GameObjectReference parentObject) :
    GameComponent(parentObject),
    m_animation(nullptr)
{}

AnimationComponent::~AnimationComponent()
{
    delete m_animation;
}

void AnimationComponent::Update()
{

}