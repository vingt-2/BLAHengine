#pragma once
#include "./GameComponents/Camera.h"
#include "./GameComponents/DirectionalLight.h"
#include "./GameComponents/MeshRenderer.h"
#include "./GameComponents/PointLight.h"
#include "../../Common/StdInclude.h"

namespace BLAengine
{
	BLACORE_API typedef int RenderTicket;

	class BLACORE_API RenderingManager
	{
	public:
		enum RenderManagerType{ Game = 0, EditorGizmos = 1 };

		RenderingManager(RenderManagerType type);
		~RenderingManager();

		RenderTicket RegisterMeshRenderer(MeshRenderer* object);
		bool		 CancelMeshRendererTicket(MeshRenderer* object);

		RenderTicket RegisterDirectionalLight(DirectionalLight* dirLight, Camera* shadowCamera);
		RenderTicket CancelDirectionalLightTicket(DirectionalLight* dirLight);

		std::unordered_map<RenderTicket, MeshRenderer*>* GetTicketedMeshRenderers();
		std::unordered_map<RenderTicket, std::pair<DirectionalLight*, Camera*>>* GetTicketedDirectionalLights();

		void Update();


	private:

		RenderManagerType m_renderManagerType;

		std::unordered_map<RenderTicket, MeshRenderer*> m_ticketedMeshRenderers;
		std::unordered_map<RenderTicket, pair<DirectionalLight*, Camera*>> m_ticketedDirLights;

		int currentTicket;

	};

	class BLACORE_API DebugRenderingManager
	{
	public:

		DebugRenderingManager() {};
		~DebugRenderingManager() {};


		void LoadDebugLineMesh(pair<vector<vec3>, vector<vec3>>& lineMesh);

		void Update();
	};




}