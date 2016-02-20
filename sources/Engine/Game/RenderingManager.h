#pragma once
#include "../Game/GameObjects/GameObject.h"
#include "../../Std/std.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/WINGL33_Renderer.h"

typedef int renderTicket;

class RenderingManager
{
public:

	RenderingManager(int managerID, Renderer* renderer);
	~RenderingManager();

	int GetManagerId();

	renderTicket RequestRenderTicket(const GameObject& object);

	void Update();


private:

	Renderer* m_renderer;

	int m_managerId;
	std::map<renderTicket,const GameObject* > m_ticketedObjects;
	std::map<renderTicket,const RenderObject* >  m_renderObjects;
	
	void LoadObject(GameObject& object);
};



