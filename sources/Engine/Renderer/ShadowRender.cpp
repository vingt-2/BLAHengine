#include "ShadowRender.h"
using namespace BLAengine;

blaMat4 DirectionalShadowRender::getShadowViewProjection()
{
    return m_shadowCamera.m_ViewProjection;
}

blaMat4 PerspectiveShadowRender::getShadowViewProjection()
{
    return m_shadowCamera.m_ViewProjection;
}

void DirectionalShadowRender::Update()
{
    m_shadowCamera.Update();
    m_shadowDirection = m_shadowCamera.m_attachedCamera->GetObjectTransform().LocalDirectionToWorld(blaVec3(0, 0, 1.f));
}

void PerspectiveShadowRender::Update()
{
    m_shadowCamera.Update();

    m_shadowDirection = m_shadowCamera.m_attachedCamera->GetObjectTransform().LocalDirectionToWorld(blaVec3(0, 0, 1.f));
}
