#include "./Common/StdInclude.h"
#include "./Common/System.h"
#include "./Common/Maths.h"

#include "./Engine/Renderer/GL33Renderer.h"

#include "AssetsImport\OBJImport.h"
#include "Engine\Game\RenderingManager.h"
#include "Engine\Game\Debug.h"
#include "./Engine/Game/Scene.h"
#include "./Engine/Game/CursorPicker.h"
#include "./Engine/Game/GameSingleton.h"
#include "./Engine/Game/GameComponents/Collider.h"
#include "./Engine/Game/RigidBodySystem.h"

namespace BLAengine
{
	class BLACORE_API EngineDemo 
	{
	public:
		EngineDemo(bool external, bool _isFullscreen) : 
			isFullScreen(_isFullscreen),
			renderAndWindow(external),
			currentObject(NULL),
			previousMouseInput(new vec2(0)),
			cameraRotation(vec3(0)),
			terminationRequest(false),
			ray(Ray(vec3(0), vec3(0),0))
		{};

		~EngineDemo() { TerminateDemo(); };

		bool InitializeDemo(RenderWindow* renderWindow);

		void UpdateDemo();

		void TerminateDemo();

		bool ShouldTerminate() { return terminationRequest; };
	private:

		// Required Engine Modules
		GameSingleton* gameSingleton;
		GL33Renderer* mainRenderer;
		AssetManager* sharedResources;
		Debug* debug;
		Scene* mainScene;
		RenderWindow* renderWindow;
		Camera* mainCamera;
		Time* timer;
		CursorPicker* cursorPicker;
		RenderingManager* renderingManager;
		DebugRenderingManager* debugRenderingManager;

		bool isFullScreen;
		bool terminationRequest;
		bool renderAndWindow;
		// HardCoded game/world entities
		
			Camera* cameraLight;
			GameChar* lightObj;
			GameChar* currentObject = nullptr;
			vec2* previousMouseInput = new vec2(0, 0);
			vec3 cameraRotation = vec3(0);
			Ray ray;
			int frameCount = 0;
			vector<Ray> debugRays;
			double lastPressG = 0;
			double lastPressS = 0;
			double lastPressR = 0;
			bool stoped = false;
			int C = 0;
			vec3 currentRay;
			vec3 currentPoint;
			double averageDet = 0;
			double averageProc = 0;
			float averageContact = 0;
			double iteration = 1;
			int solvIt = 0;
			float lightRotation = 0;
			bool moveLight = true;
	};
}