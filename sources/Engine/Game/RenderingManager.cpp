#include "RenderingManager.h"


RenderingManager::RenderingManager(int managerId,Renderer* renderer)
{
	this->m_renderer = renderer;
}


RenderingManager::~RenderingManager()
{

}

int RenderingManager::RequestRenderTicket(const GameObject& object)
{
	if (const GameChar* renderObject = dynamic_cast<const GameChar*> (&object)) 
	{
		m_renderer->m_renderPool.push_back(renderObject->m_meshRenderer);
		int renderTicket = 0;
		return 1;
	}
	else 
	{
		printf("Rendering Manager Received non renderable objecet \m");
		return -1;
	}
}

void RenderingManager::Update()
{
	m_renderer->Update();
}
