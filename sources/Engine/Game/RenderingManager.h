#pragma once
#include "../Game/GameObjects/GameObject.h"
#include "../../Std/std.h"
#include "../Renderer/Renderer.h"

typedef int RenderTicket;

class RenderingManager
{
public:
	enum RenderManagerType{ Game = 0, DebugGizmo = 1 };

	RenderingManager(int managerID, Renderer* renderer, RenderManagerType type);
	~RenderingManager();

	int GetManagerId();

	RenderTicket RequestRenderTicket(const GameObject& object);

	void Update();


private:

	Renderer* m_renderer;
	RenderManagerType m_renderManagerType;

	int m_managerId;
	std::map<RenderTicket,const GameObject* > m_ticketedObjects;
	std::map<RenderTicket,const RenderObject* >  m_renderObjects;
	
	int currentTicket;

	void LoadObject(GameObject& object);
};


