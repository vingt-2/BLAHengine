#pragma once
#include "./GameComponents/Camera.h"
#include "./GameComponents/DirectionalLight.h"
#include "./GameComponents/MeshRenderer.h"
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

		uint RegisterMeshRenderer(MeshRenderer* object);
		bool		 CancelMeshRendererTicket(MeshRenderer* object);

		uint RegisterDirectionalLight(DirectionalLight* dirLight, Camera* shadowCamera);
		uint CancelDirectionalLightTicket(DirectionalLight* dirLight);

		std::unordered_map<uint, MeshRenderer*>* GetTicketedMeshRenderers();
		std::unordered_map<uint, std::pair<DirectionalLight*, Camera*>>* GetTicketedDirectionalLights();

		void Update();


	private:

		RenderManagerType m_renderManagerType;

		std::unordered_map<uint, MeshRenderer*> m_ticketedMeshRenderers;
		std::unordered_map<uint, pair<DirectionalLight*, Camera*>> m_ticketedDirLights;

		int currentTicket;

	};

	class BLACORE_API DebugRenderingManager
	{
	public:

		DebugRenderingManager() {};
		~DebugRenderingManager() {};

		vector<pair<vector<vec3>, vector<vec3>>> m_lineMeshes;

		void LoadDebugLineMesh(pair<vector<vec3>, vector<vec3>>& lineMesh);

		void Update();
	};




}