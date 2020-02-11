#pragma once

#include <Geometry/TriangleMesh.h>

namespace BLAengine
{
    class OBJImport
    {
    public:
        bool ImportMesh(const blaString filename, TriangleMesh& mesh, bool swapNormals, bool normalizeScale);

        OBJImport(void);
        ~OBJImport(void) = default;

    private:

        long m_currentMaxVertexPos;
        long m_currentMaxUVPos;
        long m_currentMaxNormalPos;

        long FindVertexAtIndex(long index);
        long FindUVAtIndex(long index);
        long FindNormalAtIndex(long index);
    };


}