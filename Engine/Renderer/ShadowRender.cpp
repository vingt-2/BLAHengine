// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "ShadowRender.h"
#include "Core/TransformComponent.h"

using namespace BLA;

blaMat4 DirectionalShadowRender::getShadowViewProjection()
{
    return m_shadowCamera.m_worldToClipSpace;
}

blaMat4 PerspectiveShadowRender::getShadowViewProjection()
{
    return m_shadowCamera.m_worldToClipSpace;
}

void DirectionalShadowRender::Update()
{
    m_shadowCamera.Update();
    m_shadowDirection = m_shadowCamera.m_attachedCamera->GetOwnerObject().GetComponent<TransformComponent>()->GetLocalTransform().LocalDirectionToWorld(blaVec3(0, 0, 1.f));
}

void PerspectiveShadowRender::Update()
{
    m_shadowCamera.Update();

    m_shadowDirection = m_shadowCamera.m_attachedCamera->GetOwnerObject().GetComponent<TransformComponent>()->GetLocalTransform().LocalDirectionToWorld(blaVec3(0, 0, 1.f));
}
