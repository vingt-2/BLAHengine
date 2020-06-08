// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "StdInclude.h"

namespace BLA
{
    class VulkanShader
    {
    public:
        VulkanShader() : m_loaded_id(0) {};
        VulkanShader(blaString name) : m_loaded_id(0), m_name(name) {};
        ~VulkanShader() {};

        blaString m_name;

        unsigned int m_loaded_id;

        blaString m_vertexShader;
        blaString m_fragmentShader;

        blaVector<blaPair<blaString, blaString>> m_textureSamplerAttributes;

        bool LoadShaderCode(blaString fragFile, blaString vertFile);
    };

    class VulkanSystemShaders
    {
    public:
        VulkanSystemShaders() {};
        ~VulkanSystemShaders() {};

        VulkanShader m_drawColorBufferPrgm;
        VulkanShader m_drawNormalBufferPrgm;
        VulkanShader m_drawDepthBufferPrgm;
        VulkanShader m_geometryPassPrgm;
        VulkanShader m_drawSphereStencilPgrm;
        VulkanShader m_debugRayPgrm;
        VulkanShader m_debugMeshPgrm;
        VulkanShader m_shadowMapPgrm;

        bool LoadDebugRaysProgram(blaString vertFile, blaString fragFile);
        bool LoadDebugMeshesProgram(blaString vertFile, blaString fragFile);
        bool LoadDrawSphereStencilProgram(blaString vertFile, blaString fragFile);
        bool LoadGeometryPassProgram(blaString vertFile, blaString fragFile);
        bool LoadDrawColorBufferProgram(blaString vertFile, blaString fragFile);
        bool LoadDrawNormalBufferProgram(blaString vertFile, blaString fragFile);
        bool LoadDepthBufferProgram(blaString vertFile, blaString fragFile);
        bool LoadShadowMapProgram(blaString vertFile, blaString fragFile);
    };
}