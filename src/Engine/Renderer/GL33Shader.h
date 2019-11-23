#include <Common/StdInclude.h>

namespace BLAengine
{
    class GL33Shader
    {
    public:
        GL33Shader() : m_loaded_id(0) {};
        GL33Shader(blaString name) : m_loaded_id(0), m_name(name) {};
        ~GL33Shader() {};

        blaString m_name;

        unsigned int m_loaded_id;

        blaString m_vertexShader;
        blaString m_fragmentShader;

        blaVector<blaPair<blaString, blaString>> m_textureSamplerAttributes;

        bool LoadShaderCode(blaString fragFile, blaString vertFile);
    };

    class GL33SystemShaders
    {
    public:
        GL33SystemShaders() {};
        ~GL33SystemShaders() {};

        GL33Shader m_drawColorBufferPrgm;
        GL33Shader m_drawDepthBufferPrgm;
        GL33Shader m_geometryPassPrgm;
        GL33Shader m_drawSphereStencilPgrm;
        GL33Shader m_debugRayPgrm;
        GL33Shader m_debugMeshPgrm;
        GL33Shader m_shadowMapPgrm;

        bool LoadDebugRaysProgram(blaString vertFile, blaString fragFile);
        bool LoadDebugMeshesProgram(blaString vertFile, blaString fragFile);
        bool LoadDrawSphereStencilProgram(blaString vertFile, blaString fragFile);
        bool LoadGeometryPassProgram(blaString vertFile, blaString fragFile);
        bool LoadDrawColorBufferProgram(blaString vertFile, blaString fragFile);
        bool LoadDepthBufferProgram(blaString vertFile, blaString fragFile);
        bool LoadShadowMapProgram(blaString vertFile, blaString fragFile);
    };
}