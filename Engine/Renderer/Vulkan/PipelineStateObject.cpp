// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "PipelineStateObject.h"
#include "System/Console.h"
using namespace BLA;

bool VulkanShader::LoadShaderCode(blaString vertexCodeFile, blaString fragmentCodeFile)
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

bool VulkanSystemShaders::LoadDebugRaysProgram(blaString vert, blaString frag)
{
    m_debugRayPgrm = VulkanShader("SystemDebugRays");
    return m_debugRayPgrm.LoadShaderCode(vert, frag);
}

bool VulkanSystemShaders::LoadDebugMeshesProgram(blaString vert, blaString frag)
{
    m_debugMeshPgrm = VulkanShader("SystemDebugMeshes");
    return m_debugMeshPgrm.LoadShaderCode(vert, frag);
}

bool VulkanSystemShaders::LoadDrawSphereStencilProgram(blaString vert, blaString frag)
{
    m_drawSphereStencilPgrm = VulkanShader("DrawSphereStencil");
    return m_drawSphereStencilPgrm.LoadShaderCode(vert, frag);
}

bool VulkanSystemShaders::LoadGeometryPassProgram(blaString vert, blaString frag)
{
    m_geometryPassPrgm = VulkanShader("GeometryPass");
    return m_geometryPassPrgm.LoadShaderCode(vert, frag);
}

bool VulkanSystemShaders::LoadDepthBufferProgram(blaString vert, blaString frag)
{
    m_drawDepthBufferPrgm = VulkanShader("DrawDepthBuffer");
    return m_drawDepthBufferPrgm.LoadShaderCode(vert, frag);
}

bool VulkanSystemShaders::LoadShadowMapProgram(blaString vertFile, blaString fragFile)
{
    m_shadowMapPgrm = VulkanShader("ShadowMap");
    return m_shadowMapPgrm.LoadShaderCode(vertFile, fragFile);
}

bool VulkanSystemShaders::LoadDrawColorBufferProgram(blaString vert, blaString frag)
{
    m_drawColorBufferPrgm = VulkanShader("DrawColorBuffer");
    return m_drawColorBufferPrgm.LoadShaderCode(vert, frag);
}

bool VulkanSystemShaders::LoadDrawNormalBufferProgram(blaString vert, blaString frag)
{
    m_drawNormalBufferPrgm = VulkanShader("DrawNormalBuffer");
    return m_drawNormalBufferPrgm.LoadShaderCode(vert, frag);
}
