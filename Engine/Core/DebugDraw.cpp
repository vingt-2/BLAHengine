#include "DebugDraw.h"

#include "Geometry/TriangleMesh.h"

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(DebugDraw)

DebugDraw::DebugDraw(DebugRenderingManager* manager)
{
    m_drawDebugLines = true;
    m_drawDebugMeshes = true;
    this->m_debugRenderManager = manager;
}

DebugDraw::~DebugDraw(void)
{}

void DebugDraw::Update()
{
    if (m_drawDebugLines)
    {
        m_debugRenderManager->m_lineMeshes.clear();
        m_debugRenderManager->LoadDebugLineMesh(m_lineMeshVertsAndColor);
    }

    if (m_drawDebugMeshes)
    {
        m_debugRenderManager->m_filledMeshes.clear();
        m_debugRenderManager->LoadDebugFilledMesh(m_debugTrianglesVertsAndColorA);
    }

    m_lineMeshVertsAndColor.first.clear();
    m_lineMeshVertsAndColor.second.clear();

    m_debugTrianglesVertsAndColorA.first.clear();
    m_debugTrianglesVertsAndColorA.second.clear();
}

void DebugDraw::DrawLine(const blaVec3& origin, const blaVec3& destination)
{
    GetSingletonInstance()->InstanceDrawLine(origin, destination, blaVec3(0.f));
}

void DebugDraw::DrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color)
{
    GetSingletonInstance()->InstanceDrawLine(origin, destination, color);
}

void DebugDraw::DrawRay(const Ray& ray)
{
    GetSingletonInstance()->InstanceDrawRay(ray, blaVec3(0.f));
}

void DebugDraw::DrawRay(const Ray& ray, const blaVec3& color)
{
    GetSingletonInstance()->InstanceDrawRay(ray, color);
}

void DebugDraw::DrawCircle(const blaVec3& origin, const blaVec3& normal, blaF32 radius, const blaVec3& color)
{
    GetSingletonInstance()->InstanceDrawCircle(origin, normal, radius, color);
}

void DebugDraw::DrawCameraHalfCircle(const blaVec3& origin, const blaVec3& normal, blaVec3& cameraForward, blaF32 radius, const blaVec3& color)
{
    GetSingletonInstance()->InstanceDrawCameraHalfCircle(origin, normal, cameraForward, radius, color);
}

void DebugDraw::DrawSphere(const blaVec3& origin, blaF32 radius, int resolution)
{
    GetSingletonInstance()->InstanceDrawSphere(origin, radius, blaVec3(0.f), resolution);
}

void DebugDraw::DrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color, int resolution)
{
    GetSingletonInstance()->InstanceDrawSphere(origin, radius, color, resolution);
}

void DebugDraw::DrawAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec3& color)
{
    GetSingletonInstance()->InstanceDrawAABB(origin, halfExtent, color);
}

void DebugDraw::DrawOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec3& color)
{
    GetSingletonInstance()->InstanceDrawOBB(transform, halfExtent, color);
}

void DebugDraw::DrawBasis(const blaPosQuat& transform)
{
    GetSingletonInstance()->InstanceDrawBasis(transform, 1.f, 1.f);
}

void DebugDraw::DrawBasis(const blaPosQuat& transform, float opacity)
{
    GetSingletonInstance()->InstanceDrawBasis(transform, 1.f, opacity);
}

void DebugDraw::DrawBasis(const blaPosQuat& transform, float size, float opacity)
{
    GetSingletonInstance()->InstanceDrawBasis(transform, size, opacity);
}

void DebugDraw::DrawPlainSphere(const blaVec3& origin, blaF32 radius, const blaVec4& colorAndAlpha, blaU32 resolution)
{
    GetSingletonInstance()->InstanceDrawPlainSphere(origin, radius, colorAndAlpha, resolution);
}

void DebugDraw::DrawPlainAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec4& colorAndAlpha)
{
    GetSingletonInstance()->InstanceDrawPlainAABB(origin, halfExtent, colorAndAlpha);
}

void DebugDraw::DrawPlainOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec4& colorAndAlpha)
{
    GetSingletonInstance()->InstanceDrawPlainOBB(transform, halfExtent, colorAndAlpha);
}

void DebugDraw::DrawArbitraryGeometry(const blaPosQuat& transform, blaVec3& scale, const TriangleMesh& arbitraryMesh, const blaVec4& colorAndAlpha)
{
    GetSingletonInstance()->InstanceDrawArbitraryGeometry(transform, scale, arbitraryMesh, colorAndAlpha);
}

void DebugDraw::InstanceDrawLine(const blaVec3& origin, const blaVec3& destination, const blaVec3& color)
{
    m_lineMeshVertsAndColor.first.push_back(origin);
    m_lineMeshVertsAndColor.first.push_back(destination);
    m_lineMeshVertsAndColor.second.push_back(color);
    m_lineMeshVertsAndColor.second.push_back(color);
}

void DebugDraw::InstanceDrawRay(const Ray& ray, const blaVec3& color)
{
    blaVec3 destination = ray.m_origin + ray.m_length* ray.m_direction;

    InstanceDrawLine(ray.m_origin, destination, color);
}

// Draw Disc ...
//for (blaU32 i = 0; i < resolution; i++)
//{
//	const blaF32 angle = (i / (float)resolution) * M_TWO_PI;
//
//	cone.m_vertexPos.emplace_back(blaVec3(cosf(angle), 0.f, sinf(angle)));
//	cone.m_vertexNormals.emplace_back(blaVec3(cosf(angle), 0.f, sinf(angle)));
//	cone.m_vertexUVs.emplace_back(blaVec2(cosf(angle), sinf(angle)));
//}

void DebugDraw::InstanceDrawCircle(const blaVec3& origin, const blaVec3& normal, blaF32 radius, const blaVec3& color, blaU32 resolution)
{
    blaVec3 n = SafeNormalize(normal);
    blaVec3 d = blaVec3(0.f, 0.f, 1.f);
    if (abs(glm::dot(n, d)) > 0.999f)
    {
        d = blaVec3(0.f, 1.f, 0.f);
    }

    blaVec3 l = SafeNormalize(glm::cross(n, d));
    blaVec3 r = glm::cross(n, l);

    for (blaU32 i = 0; i < resolution; i++)
    {
        const blaF32 angle0 = M_TWO_PI * i / (float)resolution;
        const blaF32 angle1 = M_TWO_PI * (i + 1) / (float)resolution;

        blaVec3 p0 = origin + radius * (cosf(angle0) * l + sinf(angle0) * r);
        blaVec3 p1 = origin + radius * (cosf(angle1) * l + sinf(angle1) * r);

        InstanceDrawLine(p0, p1, color);
    }
}

void DebugDraw::InstanceDrawCameraHalfCircle(const blaVec3& origin, const blaVec3& normal, const blaVec3& cameraForward, blaF32 radius, const blaVec3& color, blaU32 resolution)
{
    blaVec3 n = SafeNormalize(normal);
    blaVec3 d = blaVec3(0.f, 0.f, 1.f);
    if (abs(glm::dot(n, d)) > 0.999f)
    {
        d = blaVec3(0.f, 1.f, 0.f);
    }

    blaVec3 l = SafeNormalize(glm::cross(n, d));
    blaVec3 r = glm::cross(n, l);

    for (blaU32 i = 0; i < resolution; i++)
    {
        const blaF32 angle0 = M_TWO_PI * i / (float)resolution;
        const blaF32 angle1 = M_TWO_PI * (i + 1) / (float)resolution;

        blaVec3 dir1 = (cosf(angle0) * l + sinf(angle0) * r);
        blaVec3 dir2 = (cosf(angle1) * l + sinf(angle1) * r);

        if (glm::dot(dir1, cameraForward) > 0.1f || glm::dot(dir2, cameraForward) > 0.1f)
        {
            blaVec3 p0 = origin + radius * dir1;
            blaVec3 p1 = origin + radius * dir2;

            InstanceDrawLine(p0, p1, color);
        }
    }
}

void DebugDraw::InstanceDrawSphere(const blaVec3& origin, blaF32 radius, const blaVec3& color, blaU32 resolution)
{
    for (blaU32 thetaCount = 0; thetaCount < resolution; ++thetaCount)
    {
        const float theta = float(thetaCount) / (resolution - 1) * float(M_PI);

        blaVec3 previousPosPhi;
        for (blaU32 phiCount = 0; phiCount < resolution; ++phiCount)
        {
            const float phi = float(phiCount) / (resolution - 1) * 2 * float(M_PI);

            const blaVec3 normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            blaVec3 pos = origin + normal * radius;

            if (phiCount != 0)
            {
                InstanceDrawLine(previousPosPhi, pos, color);
            }

            previousPosPhi = pos;
        }
    }

    for (blaU32 phiCount = 0; phiCount < resolution; ++phiCount)
    {
        const float phi = float(phiCount) / (resolution - 1) * 2 * float(M_PI);

        blaVec3 previousPosTetha;
        for (blaU32 thetaCount = 0; thetaCount < resolution; ++thetaCount)
        {
            const float theta = float(thetaCount) / (resolution - 1) * float(M_PI);

            const blaVec3 normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            blaVec3 pos = origin + normal * radius;

            if (phiCount != 0)
            {
                InstanceDrawLine(previousPosTetha, pos, color);
            }

            previousPosTetha = pos;
        }
    }
}

void DebugDraw::InstanceDrawPlainSphere(const blaVec3& origin, blaF32 radius, const blaVec4& colorAndAlpha, blaU32 resolution)
{
    blaVector<blaU32> triangleIndices;
    blaVector<blaVec3> trianglesVertices;

    for (blaU32 thetaCount = 0; thetaCount < resolution; ++thetaCount)
    {
        const float theta = float(thetaCount) / (resolution - 1) * float(M_PI);

        for (blaU32 phiCount = 0; phiCount < resolution; ++phiCount)
        {
            const float phi = float(phiCount) / (resolution - 1) * 2 * float(M_PI);

            const blaVec3 normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            trianglesVertices.push_back(normal * radius);
        }
    }

    for (blaU32 thetaCount = 0; thetaCount < resolution; ++thetaCount)
    {
        const blaU32 un = (thetaCount + 1) % resolution;
        for (blaU32 phiCount = 0; phiCount < resolution; ++phiCount)
        {
            const blaU32 vn = (phiCount + 1) % resolution;

            triangleIndices.push_back(thetaCount * resolution + phiCount);
            triangleIndices.push_back(un * resolution + phiCount);
            triangleIndices.push_back(un * resolution + vn);

            triangleIndices.push_back(thetaCount * resolution + phiCount);
            triangleIndices.push_back(un * resolution + vn);
            triangleIndices.push_back(thetaCount * resolution + vn);
        }
    }

    for (int i = 0; i < triangleIndices.size(); i += 3)
    {
        m_debugTrianglesVertsAndColorA.first.push_back(origin + trianglesVertices[triangleIndices[i / 3]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + trianglesVertices[triangleIndices[i / 3 + 1]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + trianglesVertices[triangleIndices[i / 3 + 2]]);

        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
    }
}

void DebugDraw::InstanceDrawAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec3& color)
{
    blaVec3 a = blaVec3(1.f, 1.f, 1.f) * halfExtent;
    blaVec3 b = blaVec3(1.f, -1.f, 1.f) * halfExtent;
    blaVec3 c = blaVec3(-1.f, -1.f, 1.f) * halfExtent;
    blaVec3 d = blaVec3(-1.f, 1.f, 1.f) * halfExtent;

    blaVec3 e = blaVec3(1.f, 1.f, -1.f) * halfExtent;
    blaVec3 f = blaVec3(1.f, -1.f, -1.f)  * halfExtent;
    blaVec3 g = blaVec3(-1.f, -1.f, -1.f) * halfExtent;
    blaVec3 h = blaVec3(-1.f, 1.f, -1.f)  * halfExtent;

    InstanceDrawLine(origin + a, origin + b, color);
    InstanceDrawLine(origin + a, origin + d, color);
    InstanceDrawLine(origin + b, origin + c, color);
    InstanceDrawLine(origin + c, origin + d, color);

    InstanceDrawLine(origin + e, origin + f, color);
    InstanceDrawLine(origin + e, origin + h, color);
    InstanceDrawLine(origin + f, origin + g, color);
    InstanceDrawLine(origin + g, origin + h, color);

    InstanceDrawLine(origin + a, origin + e, color);
    InstanceDrawLine(origin + b, origin + f, color);
    InstanceDrawLine(origin + c, origin + g, color);
    InstanceDrawLine(origin + d, origin + h, color);
}

void DebugDraw::InstanceDrawPlainAABB(const blaVec3& origin, const blaVec3& halfExtent, const blaVec4& colorAndAlpha)
{
    blaVec3 verts[] =
    {
        blaVec3(-1, -1, -1) * halfExtent, blaVec3(-1, -1, +1) * halfExtent, blaVec3(-1, +1, +1) * halfExtent, blaVec3(-1, +1, -1) * halfExtent,
        blaVec3(+1, -1, +1) * halfExtent, blaVec3(+1, -1, -1) * halfExtent, blaVec3(+1, +1, -1) * halfExtent, blaVec3(+1, +1, +1) * halfExtent,
        blaVec3(-1, -1, -1) * halfExtent, blaVec3(+1, -1, -1) * halfExtent, blaVec3(+1, -1, +1) * halfExtent, blaVec3(-1, -1, +1) * halfExtent,
        blaVec3(+1, +1, -1) * halfExtent, blaVec3(-1, +1, -1) * halfExtent, blaVec3(-1, +1, +1) * halfExtent, blaVec3(+1, +1, +1) * halfExtent,
        blaVec3(-1, -1, -1) * halfExtent, blaVec3(-1, +1, -1) * halfExtent, blaVec3(+1, +1, -1) * halfExtent, blaVec3(+1, -1, -1) * halfExtent,
        blaVec3(-1, +1, +1) * halfExtent, blaVec3(-1, -1, +1) * halfExtent, blaVec3(+1, -1, +1) * halfExtent, blaVec3(+1, +1, +1) * halfExtent };

    blaVector<blaVector<blaU32>> quads = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 8, 9, 10, 11 }, { 12, 13, 14, 15 }, { 16, 17, 18, 19 }, { 20, 21, 22, 23 } };

    blaVector<blaU32> triangleIndices;
    for (auto & q : quads)
    {
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[0]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[1]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[2]]);

        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[0]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[2]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[3]]);

        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);

        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
    }
}

void DebugDraw::InstanceDrawOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec3& color)
{
    blaVec3 origin = transform.GetTranslation3();
    blaQuat r = transform.GetRotation();

    blaVec3 a = r * (blaVec3(1.f, 1.f, 1.f) * halfExtent);
    blaVec3 b = r * (blaVec3(1.f, -1.f, 1.f) * halfExtent);
    blaVec3 c = r * (blaVec3(-1.f, -1.f, 1.f) * halfExtent);
    blaVec3 d = r * (blaVec3(-1.f, 1.f, 1.f) * halfExtent);

    blaVec3 e = r * (blaVec3(1.f, 1.f, -1.f) * halfExtent);
    blaVec3 f = r * (blaVec3(1.f, -1.f, -1.f)  * halfExtent);
    blaVec3 g = r * (blaVec3(-1.f, -1.f, -1.f) * halfExtent);
    blaVec3 h = r * (blaVec3(-1.f, 1.f, -1.f)  * halfExtent);

    InstanceDrawLine(origin + a, origin + b, color);
    InstanceDrawLine(origin + a, origin + d, color);
    InstanceDrawLine(origin + b, origin + c, color);
    InstanceDrawLine(origin + c, origin + d, color);

    InstanceDrawLine(origin + e, origin + f, color);
    InstanceDrawLine(origin + e, origin + h, color);
    InstanceDrawLine(origin + f, origin + g, color);
    InstanceDrawLine(origin + g, origin + h, color);

    InstanceDrawLine(origin + a, origin + e, color);
    InstanceDrawLine(origin + b, origin + f, color);
    InstanceDrawLine(origin + c, origin + g, color);
    InstanceDrawLine(origin + d, origin + h, color);
}

void DebugDraw::InstanceDrawPlainOBB(const blaPosQuat& transform, const blaVec3& halfExtent, const blaVec4& colorAndAlpha)
{
    blaVec3 origin = transform.GetTranslation3();
    blaQuat r = transform.GetRotation();

    blaVec3 verts[] =
    {
        r * (blaVec3(-1, -1, -1) * halfExtent), r * (blaVec3(-1, -1, +1) * halfExtent), r * (blaVec3(-1, +1, +1) * halfExtent), r * (blaVec3(-1, +1, -1) * halfExtent),
        r * (blaVec3(+1, -1, +1) * halfExtent), r * (blaVec3(+1, -1, -1) * halfExtent), r * (blaVec3(+1, +1, -1) * halfExtent), r * (blaVec3(+1, +1, +1) * halfExtent),
        r * (blaVec3(-1, -1, -1) * halfExtent), r * (blaVec3(+1, -1, -1) * halfExtent), r * (blaVec3(+1, -1, +1) * halfExtent), r * (blaVec3(-1, -1, +1) * halfExtent),
        r * (blaVec3(+1, +1, -1) * halfExtent), r * (blaVec3(-1, +1, -1) * halfExtent), r * (blaVec3(-1, +1, +1) * halfExtent), r * (blaVec3(+1, +1, +1) * halfExtent),
        r * (blaVec3(-1, -1, -1) * halfExtent), r * (blaVec3(-1, +1, -1) * halfExtent), r * (blaVec3(+1, +1, -1) * halfExtent), r * (blaVec3(+1, -1, -1) * halfExtent),
        r * (blaVec3(-1, +1, +1) * halfExtent), r * (blaVec3(-1, -1, +1) * halfExtent), r * (blaVec3(+1, -1, +1) * halfExtent), r * (blaVec3(+1, +1, +1) * halfExtent)
    };

    blaVector<blaVector<blaU32>> quads = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 8, 9, 10, 11 }, { 12, 13, 14, 15 }, { 16, 17, 18, 19 }, { 20, 21, 22, 23 } };

    for (auto & q : quads)
    {
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[0]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[1]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[2]]);

        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[0]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[2]]);
        m_debugTrianglesVertsAndColorA.first.push_back(origin + verts[q[3]]);

        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);

        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
    }
}

void DebugDraw::InstanceDrawBasis(const blaPosQuat& transform, float size, float opacity)
{
    InstanceDrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(size, 0.f, 0.f)), opacity * blaVec3(1.f, 0.f, 0.f));
    InstanceDrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(0.f, size, 0.f)), opacity * blaVec3(0.f, 1.f, 0.f));
    InstanceDrawLine(transform.GetTranslation3(), transform.TransformPoint(blaVec3(0.f, 0.f, size)), opacity * blaVec3(0.f, 0.f, 1.f));
}

void DebugDraw::InstanceDrawArbitraryGeometry(const blaPosQuat& transform, blaVec3& scale, const TriangleMesh& arbitraryMesh, const blaVec4& colorAndAlpha)
{
    const blaVector<blaU32>&  triangleIndices = arbitraryMesh.m_renderData.m_triangleIndices;
    const blaVector<blaVec3>& vertPos = arbitraryMesh.m_renderData.m_vertPos;

    for (int i = 0; i < arbitraryMesh.m_renderData.m_triangleIndices.size(); i += 3)
    {
        m_debugTrianglesVertsAndColorA.first.push_back(transform * blaVec4(scale * vertPos[triangleIndices[i]], 1.f));
        m_debugTrianglesVertsAndColorA.first.push_back(transform * blaVec4(scale * vertPos[triangleIndices[i + 1]], 1.f));
        m_debugTrianglesVertsAndColorA.first.push_back(transform * blaVec4(scale * vertPos[triangleIndices[i + 2]], 1.f));

        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
        m_debugTrianglesVertsAndColorA.second.push_back(colorAndAlpha);
    }
}
