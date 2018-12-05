#pragma once
#include "./GameComponents/CameraComponent.h"
#include "./GameComponents/DirectionalLight.h"
#include "./GameComponents/MeshRendererComponent.h"
#include "./GameComponents/PointLight.h"
#include "../../Common/StdInclude.h"

namespace BLAengine
{
    class BLACORE_API RenderingManager
    {
    public:
        enum RenderManagerType{ Game = 0, EditorGizmos = 1 };

        RenderingManager(RenderManagerType type);
        ~RenderingManager();

        glm::uint RegisterMeshRenderer(MeshRendererComponent* object);
        bool         CancelMeshRendererTicket(MeshRendererComponent* object);

        glm::uint RegisterDirectionalLight(DirectionalLight* dirLight, CameraComponent* shadowCamera);
        glm::uint CancelDirectionalLightTicket(DirectionalLight* dirLight);

        std::unordered_map<glm::uint, MeshRendererComponent*>* GetTicketedMeshRenderers();
        std::unordered_map<glm::uint, std::pair<DirectionalLight*, CameraComponent*>>* GetTicketedDirectionalLights();

        void Update();


    private:

        RenderManagerType m_renderManagerType;

        std::unordered_map<glm::uint, MeshRendererComponent*> m_ticketedMeshRenderers;
        std::unordered_map<glm::uint, pair<DirectionalLight*, CameraComponent*>> m_ticketedDirLights;

        int currentTicket;

    };

    class BLACORE_API DebugRenderingManager
    {
    public:

        DebugRenderingManager() {};
        ~DebugRenderingManager() {};

        vector<pair<vector<blaVec3>, vector<blaVec3>>> m_lineMeshes;

        void LoadDebugLineMesh(pair<vector<blaVec3>, vector<blaVec3>>& lineMesh);

        void Update();
    };




}