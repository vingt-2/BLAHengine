#include "DirectionalLight.h"
#include "Camera.h"
#include "../GameObject.h"
using namespace BLAengine;

DirectionalLight::DirectionalLight(vec3 direction)
{
    m_lightDirection = direction;
    m_renderTicket = 0;
}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Update()
{
    vector<Camera*> cameras = m_parentObject->GetComponents<Camera>();
    if (!cameras.size())
    {
        Camera* shadowCamera = new Camera();
        
        m_parentObject->AddComponent(shadowCamera);

        shadowCamera->Update();
    }
}

void DirectionalLight::SetDirection(vec3 direction)
{
    m_lightDirection = direction;
}

vec3 DirectionalLight::GetDirection()
{
    const Transform& t = m_parentObject->GetTransform();
    return t.LocalDirectionToWorld(m_lightDirection);
}