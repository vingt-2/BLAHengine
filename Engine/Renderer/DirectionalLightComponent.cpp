// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "DirectionalLightComponent.h"
#include "Core/CameraComponent.h"
#include "Core/TransformComponent.h"
#include "Core/GameObject.h"

#include "Core/Scene.h"

using namespace BLA;

BeginComponentDescription(DirectionalLightComponent)
EndComponentDescription()

void DirectionalLightComponent::Update()
{
    CameraComponent* shadowCamera = GetOwnerObject().GetComponent<CameraComponent>();
    shadowCamera->Update();
}

void DirectionalLightComponent::Init()
{
    CameraComponent* shadowCamera = GetOwnerObject().GetComponent<CameraComponent>();
    if (!shadowCamera)
    {
        shadowCamera = GetOwnerObject().CreateComponent<CameraComponent>();
        shadowCamera->m_isShadowMapCamera = true;
    }

    // m_renderTicket = Scene::GetSingletonInstance()->GetRenderingManager()->RegisterDirectionalLight(this, shadowCamera);
}

void DirectionalLightComponent::Shutdown()
{
    // Scene::GetSingletonInstance()->GetRenderingManager()->CancelDirectionalLightTicket(this);
}
