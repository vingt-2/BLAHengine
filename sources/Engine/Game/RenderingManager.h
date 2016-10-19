#pragma once
#include "../Game/GameComponents/MeshRenderer.h"
#include "../../Common/StdInclude.h"
#include "../Renderer/Renderer.h"
#include "./GameAlgebra/Ray.h"

namespace BLAengine
{
	BLACORE_API typedef int RenderTicket;

	class BLACORE_API RenderingManager
	{
	public:
		enum RenderManagerType{ Game = 0, EditorGizmos = 1 };

		RenderingManager(Renderer* renderer, RenderManagerType type);
		~RenderingManager();

		RenderTicket RequestRenderTicket(MeshRenderer* object);
		bool		 CancelRenderTicket(MeshRenderer* object);

		void Update();


	private:

		Renderer* m_renderer;
		RenderManagerType m_renderManagerType;

		std::map<RenderTicket, MeshRenderer* > m_ticketedObjects;
		std::map<RenderTicket, const RenderObject* >  m_renderObjects;

		int currentTicket;

		void LoadObject(GameObject& object);
	};

	class BLACORE_API DebugRenderingManager
	{
	public:

		DebugRenderingManager(Renderer* renderer);
		~DebugRenderingManager();

		Renderer* m_renderer;

		void LoadDebugLineMesh(pair<vector<vec3>, vector<vec3>>& lineMesh);

		void Update();
	};




}