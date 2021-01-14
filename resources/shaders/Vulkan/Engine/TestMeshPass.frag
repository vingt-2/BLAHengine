#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 WorldPos0;

layout (location = 0) out vec4 WorldPosOut;

void main()
{
    WorldPosOut = vec4((WorldPos0.x + 5)/10, (WorldPos0.y + 5)/10, (WorldPos0.z + 5)/10, 1.f);	
} 