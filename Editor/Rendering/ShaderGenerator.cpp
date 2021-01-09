// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "ShaderGenerator.h"
#include "Rendering/Gpu/RenderPassDescription.h"
#include "Rendering/Gpu/RenderPassProgram.h"
#include "Core/InspectableVariables.h"
#include "System/Vulkan/Context.h"

using namespace BLA;

blaMap<blaStringId, blaString> g_exposedVarTypeToGLSL =
{
    { BlaStringId("blaVec3"), "vec3" },
    { BlaStringId("blaVec2"), "vec2" },
    { BlaStringId("blaU32"), "uint" },
	{ BlaStringId("blaMat4"), "mat4"}
};

void ShaderGenerator::GenerateVertexShader(ShaderCode& shader)
{
    shader.m_code = "#version 450\n";

    for(blaSize i = 0; i < m_pDescriptor->m_vertexAttributesDescriptors.size(); ++i)
    {
        Core::InspectableVariables::ExposedVarTypeDescriptor* type = m_pDescriptor->m_vertexAttributesDescriptors[i];

        shader.m_code += "layout(location = " + std::to_string(i) + ") in " + g_exposedVarTypeToGLSL[type->m_typeID] + " vertexAtt_" + std::to_string(i) + "\n";
    }

    for (blaSize i = 0; i < m_pDescriptor->m_uniformValuesDescriptors.size(); ++i)
    {
        Core::InspectableVariables::ExposedVarTypeDescriptor* type = m_pDescriptor->m_uniformValuesDescriptors[i];

        shader.m_code += "layout(set = " + std::to_string(i) + ", binding = 0) uniform " + g_exposedVarTypeToGLSL[type->m_typeID] + "uniform_" + std::to_string(i) + "\n";
    }
}
