#include "AnimationComponent.h"

using namespace BLAengine;

AnimationComponent::AnimationComponent(GameObject* parentObject) :
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