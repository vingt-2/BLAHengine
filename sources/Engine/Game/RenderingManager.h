#pragma once
#include "../Game/GameObjects/GameObject.h"
#include "../../Std/std.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/WINGL33_Renderer.h"

class RenderingManager
{
public:

	RenderingManager(int managerID, Renderer* renderer);
	~RenderingManager();

	int GetManagerId();

	int RequestRenderTicket(const GameObject& object);

	void Update();

private:

	Renderer* m_renderer;

	int m_managerId;
	vector<pair<GameObject&, int>> m_ticketedObjects;
};



