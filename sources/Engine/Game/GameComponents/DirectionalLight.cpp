#include "DirectionalLight.h"
#include "CameraComponent.h"

#include <Engine/Game/GameObject.h>

using namespace BLAengine;

DirectionalLight::DirectionalLight(GameObjectReference parentObject):
    GameComponent(parentObject)
{
    m_lightDirection = blaVec3(0.f);
    m_renderTicket = 0;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Update()
{
    vector<CameraComponent*> cameras = GetParentObject()->GetComponents<CameraComponent>();
    if (!cameras.size())
    {
        CameraComponent* shadowCamera = BLA_CREATE_COMPONENT(CameraComponent, GetParentObject());
        shadowCamera->m_isShadowMapCamera = true;

        shadowCamera->Update();
    }
}

void DirectionalLight::SetDirection(blaVec3 direction)
{
    m_lightDirection = direction;
}

blaVec3 DirectionalLight::GetDirection()
{
    const ObjectTransform& t = GetParentObject()->GetTransform();
    return t.LocalDirectionToWorld(m_lightDirection);
}