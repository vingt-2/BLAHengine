#pragma once
#include <Engine/Core/CameraComponent.h>
#include <Engine/Renderer/DirectionalLightComponent.h>
#include <Engine/Renderer/MeshRendererComponent.h>
#include <Engine/Renderer/PointLightComponent.h>

#include <Common/StdInclude.h>

namespace BLAengine
{
    class BLACORE_API RenderingManager
    {
    public:
        enum RenderManagerType{ Game = 0, EditorGizmos = 1 };

        RenderingManager(RenderManagerType type);
        ~RenderingManager();

        blaU32 RegisterMeshRenderer(MeshRendererComponent* object);
        bool   CancelMeshRendererTicket(MeshRendererComponent* object);

        blaU32 RegisterDirectionalLight(DirectionalLightComponent* dirLight, CameraComponent* shadowCamera);
        blaU32 CancelDirectionalLightTicket(DirectionalLightComponent* dirLight);

        blaMap<blaU32, MeshRendererComponent*>* GetTicketedMeshRenderers();
        blaMap<blaU32, blaPair<DirectionalLightComponent*, CameraComponent*>>* GetTicketedDirectionalLights();

        void Update();


    private:

        RenderManagerType m_renderManagerType;

        blaMap<blaU32, MeshRendererComponent*> m_ticketedMeshRenderers;
        blaMap<blaU32, blaPair<DirectionalLightComponent*, CameraComponent*>> m_ticketedDirLights;

        int currentTicket;

    };

    class BLACORE_API DebugRenderingManager
    {
    public:

        DebugRenderingManager() {};
        ~DebugRenderingManager() {};

        blaVector<blaPair<blaVector<blaVec3>, blaVector<blaVec3>>> m_lineMeshes;
		blaVector<blaPair<blaVector<blaVec3>, blaVector<blaVec4>>> m_filledMeshes;

        blaMap<blaU32, MeshRendererComponent*> m_ticketedMeshRenderers;

        void LoadDebugLineMesh(const blaPair<blaVector<blaVec3>, blaVector<blaVec3>>& lineMesh);
		void LoadDebugFilledMesh(const blaPair<blaVector<blaVec3>, blaVector<blaVec4>>& mesh);

        void Update();
    };




}