#include <Common/StdInclude.h>

namespace BLAengine
{
    class GL33Shader
    {
    public:
        GL33Shader() : m_loaded_id(0){};
        GL33Shader(string name) : m_loaded_id(0), m_name(name) {};
        ~GL33Shader() {};

        std::string m_name;

        unsigned int m_loaded_id;

        std::string m_vertexShader;
        std::string m_fragmentShader;

        std::vector<pair<std::string, std::string>> m_textureSamplerAttributes;

        bool LoadShaderCode(string fragFile, string vertFile);
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
        GL33Shader m_shadowMapPgrm;

        bool LoadDebugRaysProgram(string vertFile, string fragFile);
        bool LoadDrawSphereStencilProgram(string vertFile, string fragFile);
        bool LoadGeometryPassProgram(string vertFile, string fragFile);
        bool LoadDrawColorBufferProgram(string vertFile, string fragFile);
        bool LoadDepthBufferProgram(string vertFile, string fragFile);
        bool LoadShadowMapProgram(string vertFile, string fragFile);
    };
}