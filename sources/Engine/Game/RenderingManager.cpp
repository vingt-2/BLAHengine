#include "RenderingManager.h"


RenderingManager::RenderingManager(int managerId,Renderer* renderer, RenderManagerType type)
{
	this->currentTicket = 1;
	this->m_renderer = renderer;
	this->m_renderManagerType = type;
}


RenderingManager::~RenderingManager()
{

}

RenderTicket RenderingManager::RequestRenderTicket(const GameObject& object)
{
	if (const GameChar* meshObject = dynamic_cast<const GameChar*> (&object)) 
	{
		//m_renderer->m_renderPool.push_back(renderObject->m_meshRenderer);
		//CHANGE THIS 
		int renderTicket = ++(this->currentTicket);
		this->m_ticketedObjects[renderTicket] = meshObject;

		if (RenderObject* renderObject = this->m_renderer->LoadRenderObject(*(meshObject->m_meshRenderer), m_renderManagerType))
		{
			this->m_renderObjects[renderTicket] = renderObject;
		}
		meshObject->m_meshRenderer->m_renderTicket = this->currentTicket;
		return this->currentTicket;
	}
	else 
	{
		printf("Rendering Manager Received non renderable object \n");
		return -1;
	}
}

bool RenderingManager::CancelRenderTicket(const GameObject& object)
{
	if (const GameChar* meshObject = dynamic_cast<const GameChar*> (&object))
	{
		int renderTicket = meshObject->m_meshRenderer->m_renderTicket;
		auto itTicket = m_ticketedObjects.find(renderTicket);
		this->m_ticketedObjects.erase(itTicket);
		return true;
	}
	else
	{
		return false;
	}
}

void RenderingManager::Update()
{
	m_renderer->Update();
}

void RenderingManager::LoadObject(GameObject& object)
{
	
}

bool RenderingManager::DebugSetupSphere(const GameObject& sphere)
{
	if (const GameChar* meshObject = dynamic_cast<const GameChar*> (&sphere))
	{
		//m_renderer->m_renderPool.push_back(renderObject->m_meshRenderer);
		//CHANGE THIS 
		int renderTicket = ++(this->currentTicket);
		this->m_ticketedObjects[renderTicket] = meshObject;

		if (RenderObject* renderObject = this->m_renderer->LoadRenderObject(*(meshObject->m_meshRenderer), 2))
		{
			this->m_renderObjects[renderTicket] = renderObject;
		}
		meshObject->m_meshRenderer->m_renderTicket = this->currentTicket;
		return this->currentTicket;
	}
	else
	{
		printf("Rendering Manager Received non renderable object \n");
		return -1;
	}
}

bool RenderingManager::DebugDrawRedSphere(vec3 position)
{
	this->m_renderer->debug_sphere_requests.push_back(position);
	return true;
}
