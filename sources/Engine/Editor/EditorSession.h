#include "../../Common/StdInclude.h"
#include "../../Common/System.h"
#include "../../Common/Maths.h"
#include "../Renderer/GL33Renderer.h"
#include "../Game/RenderingManager.h"
#include "../Game\Debug.h"
#include "../Assets/SceneManager.h"

namespace BLAengine
{
	class BLACORE_API EditorControls
	{
	public:

		vec2 m_prevMouse;
		vec3 m_cameraRotation;

		GameObject* m_cameraObject;
		RenderWindow* m_renderWindow;

		EditorControls(GameObject* cameraObject, RenderWindow* renderWindow) :
			m_prevMouse(vec2(0)),
			m_cameraRotation(vec3(0)),
			m_cameraObject(cameraObject),
			m_renderWindow(renderWindow)
		{};


		~EditorControls() {};

		void ControlCamera();

	};

	class BLACORE_API EditorSession
	{
	public:
		EditorSession(bool external, bool isFullscreen) :
			m_isFullScreen(isFullscreen),
			m_isTerminationRequested(false),
			debugRay(Ray(vec3(0), vec3(0),1))
		{};

		~EditorSession() { TerminateEditor(); };

		bool InitializeEngine(RenderWindow* renderWindow);

		void UpdateEditor();

		void TerminateEditor();

		bool ShouldTerminate() { return m_isTerminationRequested; };

		bool LoadWorkingScene(std::string filePath);

		bool SaveWorkingScene(std::string filePath);

		std::vector<string> GetSceneObjects();

	private:

		// Required Engine Modules
		GL33Renderer* m_editorRenderer;
		AssetManager* m_assetManager;
		SceneManager* m_sceneManager;
		Debug* m_debug;
		Scene* m_workingScene;
		Scene* m_editorScene;
		RenderWindow* m_renderWindow;
		Time* m_timer;
		RenderingManager* m_renderingManager;
		DebugRenderingManager* m_debugRenderingManager;
		EditorControls* m_editorControls;
		//States
		Ray debugRay;
		bool m_isFullScreen;
		bool m_isTerminationRequested;
	};
};