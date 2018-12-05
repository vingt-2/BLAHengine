#pragma once
#include "./Common/StdInclude.h"
#include "./Common/System.h"
#include "./Common/Maths/Maths.h"
#include "./Engine/Renderer/GL33Renderer.h"
#include "./Engine/Game/RenderingManager.h"
#include "./Engine/Game/Debug.h"
#include "./Engine/Assets/SceneManager.h"

namespace BLAengine
{
    class BLACORE_API HairDemoControls
    {
    public:

        glm::vec2 m_prevMouse;
        blaVec3 m_cameraRotation;

        GameObject* m_cameraObject;
        RenderWindow* m_renderWindow;

        HairDemoControls(GameObject* cameraObject, RenderWindow* renderWindow) :
            m_prevMouse(glm::vec2(0)),
            m_cameraRotation(blaVec3(0)),
            m_cameraObject(cameraObject),
            m_renderWindow(renderWindow)
        {};


        ~HairDemoControls() {};

        void ControlCamera();

    };

    class BLACORE_API HairDemo
    {
    public:
        HairDemo(bool external, bool isFullscreen) :
            m_editorControls(nullptr),
            m_isFullScreen(isFullscreen),
            m_isTerminationRequested(false),
            debugRay(Ray(blaVec3(0), blaVec3(0), 1))
        {};

        ~HairDemo() { TerminateEditor(); };

        bool InitializeEngine(RenderWindow* renderWindow);

        void UpdateEditor();

        void TerminateEditor();

        bool ShouldTerminate() { return m_isTerminationRequested; };

        bool LoadWorkingScene(std::string filePath);

        bool SaveWorkingScene(std::string filePath);

        std::vector<string> GetSceneObjects();
        
        Debug* m_debug;

    private:

        // Required Engine Modules
        GL33Renderer* m_editorRenderer;
        AssetManager* m_assetManager;
        SceneManager* m_sceneManager;
        Scene* m_workingScene;
        Scene* m_editorScene;
        RenderWindow* m_renderWindow;
        Time* m_timer;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;
        HairDemoControls* m_editorControls;
        //States
        Ray debugRay;
        bool m_isFullScreen;
        bool m_isTerminationRequested;

        float m_lastHairPlacementTime;
    };
};