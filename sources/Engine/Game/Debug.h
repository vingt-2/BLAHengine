#pragma once
#include "../Game/RenderingManager.h"
#include "GameAlgebra\Ray.h"
#include "..\..\Common\StdInclude.h"

/*
 TODO: 
    SEPARATE render m_debug, for a specific CONTEXT (when added multiple windows and context)
    And Common m_debug asserts
*/

namespace BLAengine
{
    class BLACORE_API Debug
    {
    public:

        Debug(DebugRenderingManager* renderManager);
        ~Debug();

        DebugRenderingManager* m_debugRenderManager;

        bool m_drawDebugRays;

        void Update();

        //Render m_debug:
        void DrawLine(const blaVec3 origin, const blaVec3 destination);
        void DrawLine(const blaVec3 origin, const blaVec3 destination, const blaVec3 color);
        void DrawRay(Ray ray, const blaVec3 color);
        void DrawRay(Ray ray);

        void DrawGrid(int size, float spacing, const blaVec3 color);
        void DrawBasis(ObjectTransform* transform, float opacity);

        //Common m_debug::
        static void OutputToDebug(char* m_debug);

    private:
        pair<vector<blaVec3>, vector<blaVec3>> m_lineMeshVertsAndColor;
    };

}