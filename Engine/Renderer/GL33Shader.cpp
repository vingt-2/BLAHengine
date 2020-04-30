#include "GL33Shader.h"
#include "System/Console.h"
using namespace BLA;

bool GL33Shader::LoadShaderCode(blaString vertexCodeFile, blaString fragmentCodeFile)
{
    // Read the Vertex Shader code from the file
    blaString VertexShaderCode;
    std::ifstream VertexShaderStream(vertexCodeFile, std::ios::in);
    if (VertexShaderStream.is_open())
    {
        blaString Line = "";
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
    blaString FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragmentCodeFile, std::ios::in);
    if (FragmentShaderStream.is_open())
    {

        blaString Line = "";
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

bool BLA::GL33SystemShaders::LoadDebugRaysProgram(blaString vert, blaString frag)
{
    m_debugRayPgrm = GL33Shader("SystemDebugRays");
    return m_debugRayPgrm.LoadShaderCode(vert, frag);
}

bool BLA::GL33SystemShaders::LoadDebugMeshesProgram(blaString vert, blaString frag)
{
    m_debugMeshPgrm = GL33Shader("SystemDebugMeshes");
    return m_debugMeshPgrm.LoadShaderCode(vert, frag);
}

bool BLA::GL33SystemShaders::LoadDrawSphereStencilProgram(blaString vert, blaString frag)
{
    m_drawSphereStencilPgrm = GL33Shader("DrawSphereStencil");
    return m_drawSphereStencilPgrm.LoadShaderCode(vert, frag);
}

bool BLA::GL33SystemShaders::LoadGeometryPassProgram(blaString vert, blaString frag)
{
    m_geometryPassPrgm = GL33Shader("GeometryPass");
    return m_geometryPassPrgm.LoadShaderCode(vert, frag);
}

bool BLA::GL33SystemShaders::LoadDepthBufferProgram(blaString vert, blaString frag)
{
    m_drawDepthBufferPrgm = GL33Shader("DrawDepthBuffer");
    return m_drawDepthBufferPrgm.LoadShaderCode(vert, frag);
}

bool BLA::GL33SystemShaders::LoadShadowMapProgram(blaString vertFile, blaString fragFile)
{
    m_shadowMapPgrm = GL33Shader("ShadowMap");
    return m_shadowMapPgrm.LoadShaderCode(vertFile, fragFile);
}

bool BLA::GL33SystemShaders::LoadDrawColorBufferProgram(blaString vert, blaString frag)
{
    m_drawColorBufferPrgm = GL33Shader("DrawColorBuffer");
    return m_drawColorBufferPrgm.LoadShaderCode(vert, frag);
}
