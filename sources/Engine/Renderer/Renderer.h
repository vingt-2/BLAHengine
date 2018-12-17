#pragma once
#include "../../Common/StdInclude.h"
#include "../Game/RenderingManager.h"
#include "../System/RenderWindow.h"
#include "../Game/GameComponents/CameraComponent.h"
#include "../Game/GameComponents/MeshRendererComponent.h"
#include "../Game/GameAlgebra/Ray.h"
#include "LightRender.h"

namespace BLAengine
{

    class BLACORE_API  RenderObject
    {
    public:
        virtual ~RenderObject(){};
    private:

    };

    class BLACORE_API DebugRenderObject
    {
    public:
        virtual ~DebugRenderObject() {};
    private:

    };

    class BLACORE_API Renderer
    {
    public:
        PerspectiveCamera m_mainRenderCamera;

        std::unordered_map<blaU32, RenderObject*> m_meshRenderPool;
        std::unordered_map<blaU32, RenderObject*> m_gizmoRenderPool;

        std::unordered_map<blaU32, DirectionalLightRender*> m_directionalLightPool;

        // Rendering Related
        virtual void InitializeRenderer(RenderWindow* renderWindow, RenderingManager* renderingManager, DebugRenderingManager* debugRenderManager) = 0;
        virtual void SwitchRenderingManager(RenderingManager* renderingManager) = 0;
        virtual bool Update() = 0;
        virtual int SynchWithRenderManager() = 0;
        virtual RenderObject* LoadRenderObject(const MeshRendererComponent& object, int type) = 0;
        virtual bool    CancelRender(const MeshRendererComponent& object) = 0;
        virtual bool    LoadDebugLines() = 0;
        virtual Ray        ScreenToRay() = 0;

        bool GetStatus()        const   { return m_isContextEnabled; }
        glm::vec2 GetRenderSize()    const   { return m_renderSize; }

        void SetCamera(CameraComponent* camera);

        Renderer();
        ~Renderer(void);

    protected:
        RenderWindow* m_renderWindow;
        RenderingManager* m_renderingManager;
        DebugRenderingManager* m_debugRenderingManager;

        glm::vec2 m_renderSize;
        bool m_isContextEnabled;
        bool m_isFullScreen;
    };


}