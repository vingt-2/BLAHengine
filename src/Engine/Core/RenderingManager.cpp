#include "RenderingManager.h"

using namespace BLAengine;

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
    this->m_ticketedDirLights.erase(itTicket);
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

void RenderingManager::Update()
{
}

void DebugRenderingManager::LoadDebugLineMesh(blaPair<blaVector<blaVec3>, blaVector<blaVec3>>& lineMesh)
{
    m_lineMeshes.push_back(lineMesh);
}

void DebugRenderingManager::Update()
{
}