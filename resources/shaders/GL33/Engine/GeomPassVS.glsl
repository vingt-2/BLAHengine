#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBiTangent;

uniform mat4 MVP;
uniform mat4 modelTransform;

out vec2 TexCoord0;
out vec3 WorldPos0;
out mat3 TangentSpace0;

void main()
{   
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    TexCoord0 = vertexUV;
    WorldPos0 = (modelTransform * vec4(vertexPosition_modelspace, 1.0)).xyz;
	
	vec3 Normal0 = normalize(modelTransform * vec4(vertexNormal, 0.0)).xyz;
	vec3 Tangent0 = normalize(modelTransform * vec4(vertexTangent, 0.0)).xyz;
	vec3 BiTangent0 = normalize(modelTransform * vec4(vertexBiTangent, 0.0)).xyz;
		
	TangentSpace0 = mat3(Tangent0, BiTangent0, Normal0);
}