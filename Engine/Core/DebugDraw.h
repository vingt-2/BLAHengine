// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Ray.h"

#include "StdInclude.h"
#include "BLASingleton.h"
/*
TODO: Move this the fuck out of core
SEPARATE render m_debug, for a specific CONTEXT (when added multiple windows and context)
And Common m_debug asserts
*/

namespace BLA
{
    class TriangleMesh;

    class DebugDraw
    {
        BLA_DECLARE_EXPORTED_ACCESS_SINGLETON(DebugDraw)

    public:

        DebugDraw();
        ~DebugDraw();

        void BLACORE_API Update(); //TODO: Do not expose Update()...

        static void BLACORE_API DrawLine(const blaVec3& origin, const blaVec3& destination);
        static void BLACORE_API DrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color);

        static void BLACORE_API DrawRay(const Ray& ray);
        static void BLACORE_API DrawRay(const Ray& ray, const blaVec3& color);

        static void BLACORE_API DrawCircle(const blaVec3& origin, const blaVec3& normal, blaF32 radius, const blaVec3& color);
        static void BLACORE_API DrawCameraHalfCircle(const blaVec3& origin, const blaVec3& normal, blaVec3& cameraForward, blaF32 radius, const blaVec3& color);

        static void BLACORE_API DrawSphere(const blaVec3& origin, blaF32 radius, int resolution = 12);
        static void BLACORE_API DrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color, int resolution = 12);

        static void BLACORE_API DrawAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec3& color);
        static void BLACORE_API DrawOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec3& color);

        static void BLACORE_API DrawBasis(const blaPosQuat& transform);
        static void BLACORE_API DrawBasis(const blaPosQuat& transform, float opacity);
        static void BLACORE_API DrawBasis(const blaPosQuat& transform, float size, float opacity);

        static void BLACORE_API DrawPlainSphere(const blaVec3& origin, blaF32 radius, const blaVec4& colorAndAlpha, blaU32 resolution = 12);
        static void BLACORE_API DrawPlainAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec4& colorAndAlpha);
        static void BLACORE_API DrawPlainOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec4& colorAndAlpha);

        static void BLACORE_API DrawArbitraryGeometry(const blaPosQuat& transform, blaVec3& scale, const TriangleMesh& arbitraryMesh, const blaVec4& colorAndAlpha);

    private:
        void InstanceDrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color);

        void InstanceDrawRay(const Ray& ray, const blaVec3& color);

        void InstanceDrawCircle(const blaVec3& origin, const blaVec3& normal, blaF32 radius, const blaVec3& color, blaU32 resolution = 64);

        void InstanceDrawCameraHalfCircle(const blaVec3& origin, const blaVec3& normal, const blaVec3& cameraForward, blaF32 radius, const blaVec3& color, blaU32 resolution = 64);

        void InstanceDrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color, blaU32 resolution = 12);

        void InstanceDrawPlainSphere(const blaVec3& origin, blaF32 radius, const blaVec4& colorAndAlpha, blaU32 resolution = 1000);

        void InstanceDrawAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec3& color);

        void InstanceDrawPlainAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec4& colorAndAlpha);

        void InstanceDrawOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec3& color);

        void InstanceDrawPlainOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec4& colorAndAlpha);

        void InstanceDrawBasis(const blaPosQuat& transform, float size, float opacity);

        void InstanceDrawArbitraryGeometry(const blaPosQuat& transform, blaVec3& scale, const TriangleMesh& arbitraryMesh, const blaVec4& colorAndAlpha);

        blaPair<blaVector<blaVec3>, blaVector<blaVec3>> m_lineMeshVertsAndColor;
        blaPair<blaVector<blaVec3>, blaVector<blaVec4>> m_debugTrianglesVertsAndColorA;

        bool m_drawDebugLines;
        bool m_drawDebugMeshes;
    };

}