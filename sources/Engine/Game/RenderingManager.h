#pragma once
class RenderingManager
{
public:

	RenderingManager(int managerID);
	~RenderingManager();

	int GetManagerId();

	void RequestRenderTicket(const GameObject &object);

private:

	int m_managerId;
	vector<pair<GameObject, int>> m_ticketedObjects;
};



