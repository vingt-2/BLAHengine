#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertexPos;
layout(location = 1) in vec4 inVertexRGBA;

layout(set = 0, binding = 0) uniform MVPUbo
{
	mat4 MVP;
};

layout(location = 0) out vec4 outVertexRGBA;

void main()
{   
    gl_Position = MVP * vec4(inVertexPos, 1.0);
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
    gl_Position.y = -gl_Position.y;
    
    outVertexRGBA = inVertexRGBA;
}