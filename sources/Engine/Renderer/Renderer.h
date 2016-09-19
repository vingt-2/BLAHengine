#pragma once
#include "../../Common/StdInclude.h"
#include "../System/RenderWindow.h"
#include "../Game/GameObjects/Camera.h"
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

		vector<RenderObject*> m_renderPool;
		vector<RenderObject*> m_gizmoRenderPool;

		// Rendering Related
		virtual void InitializeRenderer(RenderWindow* renderWindow) = 0;
		virtual bool Update() = 0;
		virtual RenderObject* LoadRenderObject(const MeshRenderer& object, int type) = 0;
		virtual bool	CancelRender(const MeshRenderer& object) = 0;
		virtual bool	LoadDebugLines(pair<vector<vec3>, vector<vec3>>& lineMesh) = 0;

		bool GetStatus()        const   { return m_isContextEnabled; }
		vec2 GetRenderSize()    const   { return m_renderSize; }

		void SetCamera(Camera* camera);

		Renderer();
		~Renderer(void);

	protected:

		RenderWindow* m_renderWindow;

		ivec2 m_renderSize;
		bool m_isContextEnabled;
		bool m_isFullScreen;
	};


}