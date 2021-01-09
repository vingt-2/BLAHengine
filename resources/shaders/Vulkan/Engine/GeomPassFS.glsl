#version 450

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 TexCoord0;
layout(location = 1) in vec3 WorldPos0;
layout(location = 2) in mat3 TangentSpace0;

layout (location = 0) out vec3 DiffuseOut;
layout (location = 1) out vec3 NormalOut;
layout (location = 2) out vec3 WorldPosOut;
layout (location = 3) out vec3 TexCoordOut;

layout(set = 0, binding = 1) uniform sampler2D diffuseMap;
layout(set = 0, binding = 2) uniform sampler2D normalMap;
layout(set = 0, binding = 3) uniform sampler2D alphaMap;

void main()
{
	if(texture(alphaMap, TexCoord0).a < 0.5f)
		discard;

    WorldPosOut = WorldPos0;
	
    DiffuseOut = texture(diffuseMap, TexCoord0).rgb;
	
	vec3 normalMapSample = texture(normalMap, TexCoord0).rgb;

	normalMapSample = normalize(2.0f * normalMapSample - vec3(1.f,1.f,1.f));
	
	NormalOut = TangentSpace0 * normalMapSample;
	
	TexCoordOut = vec3(TexCoord0, 0.0);
} 