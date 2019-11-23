#include "PrimitiveGeometry.h"

using namespace BLAengine;

TriangleMesh PrimitiveGeometry::MakeCube()
{
    TriangleMesh cube;

    const struct CubeVertex { blaVec3 position, normal; blaVec2 texCoord; } verts[] =
    {
        { { -1, -1, -1 }, { -1, 0, 0 }, { 0, 0 } }, { { -1, -1, +1 }, { -1, 0, 0 }, { 1, 0 } }, { { -1, +1, +1 }, { -1, 0, 0 }, { 1, 1 } }, { { -1, +1, -1 }, { -1, 0, 0 }, { 0, 1 } },
        { { +1, -1, +1 }, { +1, 0, 0 }, { 0, 0 } }, { { +1, -1, -1 }, { +1, 0, 0 }, { 1, 0 } }, { { +1, +1, -1 }, { +1, 0, 0 }, { 1, 1 } }, { { +1, +1, +1 }, { +1, 0, 0 }, { 0, 1 } },
        { { -1, -1, -1 }, { 0, -1, 0 }, { 0, 0 } }, { { +1, -1, -1 }, { 0, -1, 0 }, { 1, 0 } }, { { +1, -1, +1 }, { 0, -1, 0 }, { 1, 1 } }, { { -1, -1, +1 }, { 0, -1, 0 }, { 0, 1 } },
        { { +1, +1, -1 }, { 0, +1, 0 }, { 0, 0 } }, { { -1, +1, -1 }, { 0, +1, 0 }, { 1, 0 } }, { { -1, +1, +1 }, { 0, +1, 0 }, { 1, 1 } }, { { +1, +1, +1 }, { 0, +1, 0 }, { 0, 1 } },
        { { -1, -1, -1 }, { 0, 0, -1 }, { 0, 0 } }, { { -1, +1, -1 }, { 0, 0, -1 }, { 1, 0 } }, { { +1, +1, -1 }, { 0, 0, -1 }, { 1, 1 } }, { { +1, -1, -1 }, { 0, 0, -1 }, { 0, 1 } },
        { { -1, +1, +1 }, { 0, 0, +1 }, { 0, 0 } }, { { -1, -1, +1 }, { 0, 0, +1 }, { 1, 0 } }, { { +1, -1, +1 }, { 0, 0, +1 }, { 1, 1 } }, { { +1, +1, +1 }, { 0, 0, +1 }, { 0, 1 } },
    };

    blaVector<blaVector<blaU32>> quads = { { 0, 1, 2, 3 }, { 4, 5, 6, 7 }, { 8, 9, 10, 11 }, { 12, 13, 14, 15 }, { 16, 17, 18, 19 }, { 20, 21, 22, 23 } };

    blaVector<blaU32> triangleIndices;
    for (auto & q : quads)
    {
        triangleIndices.push_back(q[0]);
        triangleIndices.push_back(q[1]);
        triangleIndices.push_back(q[2]);

        triangleIndices.push_back(q[0]);
        triangleIndices.push_back(q[2]);
        triangleIndices.push_back(q[3]);
    }

    for (int i = 0; i < 24; ++i)
    {
        cube.m_vertexPos.push_back(verts[i].position);
        cube.m_vertexNormals.push_back(verts[i].normal);
        cube.m_vertexUVs.push_back(verts[i].texCoord);
    }

    cube.BuildMeshTopo(triangleIndices, triangleIndices, triangleIndices, false);
    cube.ComputeFaceTangents();
    cube.GenerateRenderData();

    return cube;
}

TriangleMesh PrimitiveGeometry::MakeSphere(blaF32 radius)
{
    TriangleMesh sphere;

    const blaU32 thetaSamples = 32, phiSamples = 32;

    for (blaU32 thetaCount = 0; thetaCount < thetaSamples; ++thetaCount)
    {
        const float theta = float(thetaCount) / (thetaSamples - 1) * float(M_PI);

        for (blaU32 phiCount = 0; phiCount < phiSamples; ++phiCount)
        {
            const float phi = float(phiCount) / (phiSamples - 1) * 2 * float(M_PI);

            const blaVec3 normal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            sphere.m_vertexPos.push_back(normal * radius);
            sphere.m_vertexNormals.push_back(normal);

            sphere.m_vertexUVs.push_back(blaVec2(1.f - float(thetaCount) / (thetaSamples - 1), 1.f - float(phiCount) / (phiSamples - 1)));
        }
    }

    blaVector<blaU32> triangleIndices;
    for (blaU32 thetaCount = 0; thetaCount < thetaSamples; ++thetaCount)
    {
        const blaU32 un = (thetaCount + 1) % thetaSamples;
        for (blaU32 phiCount = 0; phiCount < phiSamples; ++phiCount)
        {
            const blaU32 vn = (phiCount + 1) % phiSamples;

            triangleIndices.push_back(thetaCount * phiSamples + phiCount);
            triangleIndices.push_back(un * phiSamples + phiCount);
            triangleIndices.push_back(un * phiSamples + vn);

            triangleIndices.push_back(thetaCount * phiSamples + phiCount);
            triangleIndices.push_back(un * phiSamples + vn);
            triangleIndices.push_back(thetaCount * phiSamples + vn);
        }
    }

    sphere.BuildMeshTopo(triangleIndices, triangleIndices, triangleIndices, true);
    sphere.ComputeFaceTangents();
    sphere.GenerateRenderData();

    return sphere;
}

/*TriangleMesh MakeQuad(blaF32 widht, blaF32 height, blaU8 axis)
{
    if(axis > 2)
    {
        return TriangleMesh("InvalidTriangle");
    }

    const struct CubeVertex { blaVec3 position, normal; blaVec2 texCoord; } verts[] =
    {
        { { -1, -1, -1 },{ -1, 0, 0 },{ 0, 0 } },{ { -1, -1, +1 },{ -1, 0, 0 },{ 1, 0 } },{ { -1, +1, +1 },{ -1, 0, 0 },{ 1, 1 } },{ { -1, +1, -1 },{ -1, 0, 0 },{ 0, 1 } }
    };

    blaVector<blaVector<blaU32>> quads = { { 0, 1, 2, 3 },{ 4, 5, 6, 7 } }, { 8, 9, 10, 11 }, { 12, 13, 14, 15 }, { 16, 17, 18, 19 }, { 20, 21, 22, 23 }
};

    blaVector<blaU32> triangleIndices;
    for (auto & q : quads)
    {
        triangleIndices.push_back(q[0]);
        triangleIndices.push_back(q[1]);
        triangleIndices.push_back(q[2]);

        triangleIndices.push_back(q[0]);
        triangleIndices.push_back(q[2]);
        triangleIndices.push_back(q[3]);
    }
}*/

//Make cone towards up axis
TriangleMesh PrimitiveGeometry::MakeCone(blaU32 resolution)
{
    TriangleMesh cone;

    blaVector<blaVec3> vertexPos, vertexNormals;
    blaVector<blaVec2> vertexUVs;

    cone.m_vertexPos.emplace_back(blaVec3(0.f, 0.f, 0.f));
    cone.m_vertexPos.emplace_back(blaVec3(0.f, 1.f, 0.f));
    cone.m_vertexNormals.emplace_back(blaVec3(0.f, -1.f, 0.f));
    cone.m_vertexNormals.emplace_back(blaVec3(0.f, 1.f, 0.f));
    cone.m_vertexUVs.emplace_back(blaVec2(0.f, 0.f));
    cone.m_vertexUVs.emplace_back(blaVec2(0.f, 0.f));


    for (blaU32 i = 0; i < resolution; i++)
    {
        const blaF32 angle = (i / (float)resolution) * M_TWO_PI;

        cone.m_vertexPos.emplace_back(blaVec3(cosf(angle), 0.f, sinf(angle)));
        cone.m_vertexNormals.emplace_back(blaVec3(cosf(angle), 0.f, sinf(angle)));
        cone.m_vertexUVs.emplace_back(blaVec2(cosf(angle), sinf(angle)));
    }

    blaVector<blaU32> triangleIndices;
    blaVector<blaU32> normalIndices;
    for (blaU32 i = 2; i < resolution + 1; i++)
    {
        triangleIndices.push_back(0);
        triangleIndices.push_back(i);
        triangleIndices.push_back(i + 1);

        normalIndices.push_back(0);
        normalIndices.push_back(0);
        normalIndices.push_back(0);
    }

    triangleIndices.push_back(0);
    triangleIndices.push_back(resolution + 1);
    triangleIndices.push_back(2);

    normalIndices.push_back(0);
    normalIndices.push_back(0);
    normalIndices.push_back(0);

    for (blaU32 i = 2; i < resolution + 1; i++)
    {
        triangleIndices.push_back(1);
        triangleIndices.push_back(i + 1);
        triangleIndices.push_back(i);

        normalIndices.push_back(1);
        normalIndices.push_back(i + 1);
        normalIndices.push_back(i);
    }

    triangleIndices.push_back(1);
    triangleIndices.push_back(2);
    triangleIndices.push_back(resolution + 1);

    normalIndices.push_back(1);
    normalIndices.push_back(2);
    normalIndices.push_back(resolution + 1);

    cone.BuildMeshTopo(triangleIndices, normalIndices, triangleIndices, false);
    cone.ComputeFaceTangents();
    cone.GenerateRenderData();

    return cone;
}

TriangleMesh PrimitiveGeometry::MakeDisc(blaU32 resolution)
{
    TriangleMesh disc;

    blaVector<blaVec3> vertexPos, vertexNormals;
    blaVector<blaVec2> vertexUVs;

    disc.m_vertexPos.emplace_back(blaVec3(0.f, 0.f, 0.f));
    disc.m_vertexNormals.emplace_back(blaVec3(0.f, 1.f, 0.f));

    for (blaU32 i = 0; i < resolution; i++)
    {
        const blaF32 angle = (i / (float)resolution) * M_TWO_PI;

        disc.m_vertexPos.emplace_back(blaVec3(cosf(angle), 0.f, sinf(angle)));
        disc.m_vertexUVs.emplace_back(blaVec2(cosf(angle), sinf(angle)));
    }

    blaVector<blaU32> triangleIndices;
    blaVector<blaU32> normalIndices;
    for (blaU32 i = 1; i < resolution; i++)
    {
        triangleIndices.push_back(0);
        triangleIndices.push_back(i + 1);
        triangleIndices.push_back(i);

        normalIndices.push_back(0);
        normalIndices.push_back(0);
        normalIndices.push_back(0);
    }

    triangleIndices.push_back(0);
    triangleIndices.push_back(resolution);
    triangleIndices.push_back(1);

    normalIndices.push_back(0);
    normalIndices.push_back(0);
    normalIndices.push_back(0);

    disc.BuildMeshTopo(triangleIndices, normalIndices, triangleIndices, false);
    disc.ComputeFaceTangents();
    disc.GenerateRenderData();

    return disc;
}