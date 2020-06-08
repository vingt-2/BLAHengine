#version 330

in vec2 TexCoord0;
in vec3 WorldPos0;
in mat3 TangentSpace0;

layout (location = 0) out vec3 DiffuseOut;
layout (location = 1) out vec3 NormalOut;
layout (location = 2) out vec3 WorldPosOut;
layout (location = 3) out vec3 TexCoordOut;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D alphaMap;

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