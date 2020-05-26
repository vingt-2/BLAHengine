// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderingManager.h"

#include "Core/CameraComponent.h"
#include "Renderer/DirectionalLightComponent.h"
#include "Renderer/MeshRendererComponent.h"
#include "Renderer/PointLightComponent.h"

#pragma optimize("", off)

using namespace BLA;

RenderingManager::RenderingManager(RenderManagerType type)
{
    this->currentTicket = 1;
    this->m_renderManagerType = type;
}


RenderingManager::~RenderingManager()
{

}

blaU32 RenderingManager::RegisterMeshRenderer(MeshRendererComponent* meshRender)
{
    //CHANGE THE WAY WE ASSIGN TICKET NUMBERS !
    int renderTicket = ++(this->currentTicket);
    this->m_ticketedMeshRenderers[renderTicket] = meshRender;

    meshRender->m_renderTicket = this->currentTicket;
    return this->currentTicket;
}

bool RenderingManager::CancelMeshRendererTicket(MeshRendererComponent* meshRender)
{
    int renderTicket = meshRender->m_renderTicket;
    auto itTicket = m_ticketedMeshRenderers.find(renderTicket);
    if(itTicket != m_ticketedMeshRenderers.end())
        this->m_ticketedMeshRenderers.erase(itTicket);
    return true;
}

blaU32 RenderingManager::RegisterDirectionalLight(DirectionalLightComponent* dirLight, CameraComponent* shadowCamera)
{
    //CHANGE THE WAY WE ASSIGN TICKET NUMBERS !
    int renderTicket = ++(this->currentTicket);
    this->m_ticketedDirLights[renderTicket] = blaPair<DirectionalLightComponent*, CameraComponent*>(dirLight, shadowCamera);

    dirLight->m_renderTicket = this->currentTicket;
    return this->currentTicket;
}

blaU32 RenderingManager::CancelDirectionalLightTicket(DirectionalLightComponent * dirLight)
{
    int renderTicket = dirLight->m_renderTicket;
    auto itTicket = m_ticketedDirLights.find(renderTicket);

    /*if(itTicket != m_ticketedDirLights.end()) 
    {
        this->m_ticketedDirLights.erase(itTicket);
        return true;
    }
    return false;*/
    m_ticketedDirLights.clear();
    return true;
}

blaU32 RenderingManager::RegisterPointLight(PointLightComponent* pointLight)
{
    //CHANGE THE WAY WE ASSIGN TICKET NUMBERS !
    int renderTicket = ++(this->currentTicket);
    this->m_ticketedPointLights[renderTicket] = pointLight;

    pointLight->m_renderTicket = this->currentTicket;
    return this->currentTicket;
}

blaU32 RenderingManager::CancelPointLightTicket(PointLightComponent * pointLight)
{
    int renderTicket = pointLight->m_renderTicket;
    auto itTicket = m_ticketedPointLights.find(renderTicket);
    this->m_ticketedPointLights.erase(itTicket);
    return true;
}

blaMap<blaU32, MeshRendererComponent*>* RenderingManager::GetTicketedMeshRenderers()
{
    return &(m_ticketedMeshRenderers);
}

blaMap<blaU32, blaPair<DirectionalLightComponent*, CameraComponent*>>* RenderingManager::GetTicketedDirectionalLights()
{
    return &(m_ticketedDirLights);
}

blaMap<blaU32, PointLightComponent*>* RenderingManager::GetTicketedPointLights()
{
    return &(m_ticketedPointLights);
}

void RenderingManager::Update()
{
}

void DebugRenderingManager::LoadDebugLineMesh(const blaPair<blaVector<blaVec3>, blaVector<blaVec3>>& lineMesh)
{
    m_lineMeshes.push_back(lineMesh);
}

void DebugRenderingManager::LoadDebugFilledMesh(const blaPair<blaVector<blaVec3>, blaVector<blaVec4>>& mesh)
{
    m_filledMeshes.push_back(mesh);
}

void DebugRenderingManager::Update()
{
}
