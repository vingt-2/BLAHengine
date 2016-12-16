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

uint RenderingManager::RegisterMeshRenderer(MeshRenderer* meshRender)
{
	//CHANGE THE WAY WE ASSIGN TICKET NUMBERS !
	int renderTicket = ++(this->currentTicket);
	this->m_ticketedMeshRenderers[renderTicket] = meshRender;

	meshRender->m_renderTicket = this->currentTicket;
	return this->currentTicket;
}

bool RenderingManager::CancelMeshRendererTicket(MeshRenderer* meshRender)
{
	int renderTicket = meshRender->m_renderTicket;
	auto itTicket = m_ticketedMeshRenderers.find(renderTicket);
	this->m_ticketedMeshRenderers.erase(itTicket);
	return true;
}

uint BLAengine::RenderingManager::RegisterDirectionalLight(DirectionalLight* dirLight, Camera* shadowCamera)
{
	//CHANGE THE WAY WE ASSIGN TICKET NUMBERS !
	int renderTicket = ++(this->currentTicket);
	this->m_ticketedDirLights[renderTicket] = std::pair<DirectionalLight*, Camera*>(dirLight, shadowCamera);

	dirLight->m_renderTicket = this->currentTicket;
	return this->currentTicket;
}

uint BLAengine::RenderingManager::CancelDirectionalLightTicket(DirectionalLight * dirLight)
{
	int renderTicket = dirLight->m_renderTicket;
	auto itTicket = m_ticketedDirLights.find(renderTicket);
	this->m_ticketedDirLights.erase(itTicket);
	return true;
}

std::unordered_map<uint, MeshRenderer*>* BLAengine::RenderingManager::GetTicketedMeshRenderers()
{
	return &(m_ticketedMeshRenderers);
}

std::unordered_map<uint, std::pair<DirectionalLight*,Camera*>>* BLAengine::RenderingManager::GetTicketedDirectionalLights()
{
	return &(m_ticketedDirLights);
}

void RenderingManager::Update()
{
}

void DebugRenderingManager::LoadDebugLineMesh(pair<vector<vec3>, vector<vec3>>& lineMesh)
{
	m_lineMeshes.push_back(lineMesh);
}

void DebugRenderingManager::Update()
{
}