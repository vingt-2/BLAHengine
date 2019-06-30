#pragma once
#include <Engine/Game/RenderingManager.h>

#include <Engine/Game/GameAlgebra/Ray.h>

#include <Common/StdInclude.h>
#include <Common/BLASingleton.h>
/*
TODO:
SEPARATE render m_debug, for a specific CONTEXT (when added multiple windows and context)
And Common m_debug asserts
*/

namespace BLAengine
{
	class BLACORE_API DebugDraw
	{
		BLA_DECLARE_SINGLETON(DebugDraw)

	public:

		DebugDraw(DebugRenderingManager* renderManager);
		~DebugDraw();

		void Update();

		static void DrawLine(const blaVec3& origin, const blaVec3& destination);
		static void DrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color);

		static void DrawRay(const Ray& ray);
		static void DrawRay(const Ray& ray, const blaVec3& color);
		
		static void DrawSphere(const blaVec3& origin, blaF32 radius);
		static void DrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color);

		static void DrawBasis(const blaPosQuat& transform);
		static void DrawBasis(const blaPosQuat& transform, float opacity);
		static void DrawBasis(const blaPosQuat& transform, float size, float opacity);

	private:
		void InstanceDrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color);
			 
		void InstanceDrawRay(const Ray& ray, const blaVec3& color);
			 
		void InstanceDrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color);
			 
		void InstanceDrawBasis(const blaPosQuat& transform, float size, float opacity);

		pair<vector<blaVec3>, vector<blaVec3>> m_lineMeshVertsAndColor;
		DebugRenderingManager* m_debugRenderManager;

		bool m_drawDebugRays;
	};

}