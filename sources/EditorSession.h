#include "./Common/StdInclude.h"
#include "./Common/System.h"
#include "./Common/Maths.h"
#include "./Engine/Renderer/GL33Renderer.h"
#include "Engine\Game\RenderingManager.h"
#include "Engine\Game\Debug.h"
#include "./Engine/Assets/SceneManager.h"

namespace BLAengine
{
	class BLACORE_API EditorSession
	{
	public:
		EditorSession(bool external, bool isFullscreen) :
			m_isFullScreen(isFullscreen),
			m_isTerminationRequested(false)
		{};

		~EditorSession() { TerminateEditor(); };

		bool InitializeEngine(RenderWindow* renderWindow);

		void UpdateEditor();

		void TerminateEditor();

		bool ShouldTerminate() { return m_isTerminationRequested; };

		bool LoadWorkingScene(std::string filePath);

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

		//States
		bool m_isFullScreen;
		bool m_isTerminationRequested;
	};

	class BLACORE_API EditorControls
	{
	public:
		EditorControls() {};
		~EditorControls() {};


	};
};