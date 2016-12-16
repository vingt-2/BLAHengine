#include "../../Common/StdInclude.h"
#include "../../Common/System.h"
#include "../../Common/Maths.h"
#include "../Renderer/GL33Renderer.h"
#include "../Game/RenderingManager.h"
#include "../Game\Debug.h"
#include "../Assets/SceneManager.h"

namespace BLAengine
{
	class BLACORE_API MeshEditorControls
	{
	public:

		vec2 m_prevMouse;
		vec3 m_cameraRotation;
		vec3 m_lightRotation;

		GameObject* m_cameraObject;
		GameObject* m_lightObj;
		RenderWindow* m_renderWindow;

		MeshEditorControls(GameObject* cameraObject, GameObject* lightObj, RenderWindow* renderWindow) :
			m_prevMouse(vec2(0)),
			m_cameraRotation(vec3(0)),
			m_lightRotation(vec3(0)),
			m_cameraObject(cameraObject),
			m_renderWindow(renderWindow),
			m_lightObj(lightObj)
		{};


		~MeshEditorControls() {};

		void ControlCamera();

	};

	class BLACORE_API MeshImportSession
	{
	public:
		MeshImportSession(bool external, bool isFullscreen) :
			m_isFullScreen(isFullscreen),
			m_isTerminationRequested(false)
		{};

		~MeshImportSession() { TerminateEditor(); };

		bool InitializeEngine(RenderWindow* renderWindow);

		void UpdateEditor();

		void TerminateEditor();

		bool ShouldTerminate() { return m_isTerminationRequested; };

		bool ImportMesh(std::string filepath, std::string name);

		bool SaveMeshToCooked();

	private:

		// Required Engine Modules
		GL33Renderer* m_editorRenderer;
		AssetManager* m_assetManager;
		Debug* m_debug;
		Scene* m_workingScene;
		RenderWindow* m_renderWindow;
		RenderingManager* m_renderingManager;
		DebugRenderingManager* m_debugRenderingManager;
		MeshEditorControls* m_editorControls;

		//States
		bool m_isFullScreen;
		bool m_isTerminationRequested;
		int i = 0;

		TriangleMesh* m_importedMesh = nullptr;
	};
};