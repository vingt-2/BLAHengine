#include "GL33Shader.h"
#include "Engine/System/Console.h"
using namespace BLAengine;

bool GL33Shader::LoadShaderCode(string vertexCodeFile, string fragmentCodeFile)
{
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertexCodeFile, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    else
    {
		Console::LogError(fragmentCodeFile + " could not be opened.");
        return false;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragmentCodeFile, std::ios::in);
    if (FragmentShaderStream.is_open())
    {

        std::string Line = "";
        while (getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    else
    {
        return false;
    }

    m_vertexShader = VertexShaderCode;
    m_fragmentShader = FragmentShaderCode;

    return true;
}

bool BLAengine::GL33SystemShaders::LoadDebugRaysProgram(string vert, string frag)
{
    m_debugRayPgrm = GL33Shader("SystemDebugRays");
    return m_debugRayPgrm.LoadShaderCode(vert, frag);
}

bool BLAengine::GL33SystemShaders::LoadDrawSphereStencilProgram(string vert, string frag)
{
    m_drawSphereStencilPgrm = GL33Shader("DrawSphereStencil");
    return m_drawSphereStencilPgrm.LoadShaderCode(vert, frag);
}

bool BLAengine::GL33SystemShaders::LoadGeometryPassProgram(string vert, string frag)
{
    m_geometryPassPrgm = GL33Shader("GeometryPass");
    return m_geometryPassPrgm.LoadShaderCode(vert, frag);
}

bool BLAengine::GL33SystemShaders::LoadDepthBufferProgram(string vert, string frag)
{
    m_drawDepthBufferPrgm = GL33Shader("DrawDepthBuffer");
    return m_drawDepthBufferPrgm.LoadShaderCode(vert, frag);
}

bool BLAengine::GL33SystemShaders::LoadShadowMapProgram(string vertFile, string fragFile)
{
    m_shadowMapPgrm = GL33Shader("ShadowMap");
    return m_shadowMapPgrm.LoadShaderCode(vertFile, fragFile);
}

bool BLAengine::GL33SystemShaders::LoadDrawColorBufferProgram(string vert, string frag)
{
    m_drawColorBufferPrgm = GL33Shader("DrawColorBuffer");
    return m_drawColorBufferPrgm.LoadShaderCode(vert, frag);
}
