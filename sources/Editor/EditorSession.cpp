#include "EditorSession.hpp"

	EditorSession::EditorSession():
		m_scene(Scene(&m_mainCamera)),
		m_renderer("crotte",false)
	{

	}

	void EditorSession::SetupSession()
	{

	}

	void EditorSession::TerminateSession()
	{
		// Do various clean up operations here;
		glfwTerminate();
	}

	void EditorSession::Update()
	{
		//while(!terminationRequest)
		//	{
		//		glfwSetWindowTitle(mainRenderer->GetWindow(), title.str().data())
		//
		//
		//
		//		SimpleControls(mainCamera);
		//		
		//		mainCamera->Update();
		//		mainRenderer->Update();
		//		mainScene->Update();
		//
		//		if(mainScene->m_enableSimulation)
		//			solvIt += (mainScene->m_rigidBodySystem->m_collisionProcessor->m_iterationCount);
		//
		//
		//
		//		frameCount++;
		//		if( (glfwGetKey(mainRenderer->GetWindow(), GLFW_KEY_ESCAPE ) == GLFW_PRESS)  |  glfwWindowShouldClose(mainRenderer->GetWindow()) )
		//		{
		//			terminationRequest = true;
		//		}
		//
		//		cameraLight->UpdateView();
		//	}
		//
		//
		//	glfwTerminate();
		//
		//	return 0;
	}
