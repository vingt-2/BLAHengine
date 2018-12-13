#include "AnimationComponent.h"

using namespace BLAengine;

AnimationComponent::AnimationComponent():
m_animation(nullptr)
{}

AnimationComponent::~AnimationComponent()
{
    delete m_animation;
}

void AnimationComponent::Update()
{

}