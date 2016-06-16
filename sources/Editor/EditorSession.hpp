#include "../Engine/Renderer/WINGL33_Renderer.h"
#include "../Engine/Game/Scene.h"
#include "./EditorControls.hpp"

class EditorSession
{
public:
	EditorSession();
	~EditorSession();

	void SetupSession();
	void TerminateSession();
	void Update();


	// Members
	Scene m_scene;
	GL33Renderer m_renderer;
	EditorControls m_controls;
	SharedResources m_resourceAccess;
	Camera m_mainCamera;
};