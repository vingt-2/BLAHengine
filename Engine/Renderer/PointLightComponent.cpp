// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "PointLightComponent.h"

#include "Core/GameObject.h"

#include "Core/Scene.h"
#include "Core/RenderingManager.h"
#include "Core/TransformComponent.h"

using namespace BLA;

BeginBehaviorDescription(PointLightComponent, Dependencies(RootSystem))
Expose(m_radius)
Expose(m_draw)
EndComponentDescription()

// Norm of the default m_scale for Object Transforms...
#define BLA_SCALE_NORM 1.73205080757f

void PointLightComponent::Update()
{
    m_position = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform().GetPosition();
	if(m_renderTicket)
	{
		if(!m_draw)
		{
			Scene::GetSingletonInstance()->GetRenderingManager()->CancelPointLightTicket(this);
			m_renderTicket = 0;
		}
	}
	else
	{
		if (m_draw)
		{
			m_renderTicket = Scene::GetSingletonInstance()->GetRenderingManager()->RegisterPointLight(this);
		}
	}
}

void PointLightComponent::Init()
{
    m_radius = 1.f;
    m_renderTicket = Scene::GetSingletonInstance()->GetRenderingManager()->RegisterPointLight(this);
}

void PointLightComponent::Shutdown()
{
    Scene::GetSingletonInstance()->GetRenderingManager()->CancelPointLightTicket(this);
}