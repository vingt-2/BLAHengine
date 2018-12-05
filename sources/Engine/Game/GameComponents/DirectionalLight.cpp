#include "DirectionalLight.h"
#include "CameraComponent.h"
#include "../GameObject.h"
using namespace BLAengine;

DirectionalLight::DirectionalLight(blaVec3 direction)
{
    m_lightDirection = direction;
    m_renderTicket = 0;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Update()
{
    vector<CameraComponent*> cameras = m_parentObject->GetComponents<CameraComponent>();
    if (!cameras.size())
    {
        CameraComponent* shadowCamera = new CameraComponent();
        shadowCamera->m_isShadowMapCamera = true;
        m_parentObject->AddComponent(shadowCamera);

        shadowCamera->Update();
    }
}

void DirectionalLight::SetDirection(blaVec3 direction)
{
    m_lightDirection = direction;
}

blaVec3 DirectionalLight::GetDirection()
{
    const ObjectTransform& t = m_parentObject->GetTransform();
    return t.LocalDirectionToWorld(m_lightDirection);
}