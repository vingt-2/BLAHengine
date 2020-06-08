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
uniform float time;

void main()
{
	float alpha = 0.005f * time;
	
	vec2 uv = mat2(vec2(cos(alpha), sin(alpha)), vec2(-sin(alpha), cos(alpha))) * TexCoord0;
	
	if(texture(alphaMap, uv).a < 0.5f)
		discard;

    WorldPosOut = WorldPos0;
	
    DiffuseOut = texture(diffuseMap, uv).rgb;
	
	vec3 normalMapSample = texture(normalMap, uv).rgb;

	normalMapSample = normalize(2.0f * normalMapSample - vec3(1.f,1.f,1.f));
	
	NormalOut = TangentSpace0 * normalMapSample;
	
	TexCoordOut = vec3(uv, 0.0);
} 