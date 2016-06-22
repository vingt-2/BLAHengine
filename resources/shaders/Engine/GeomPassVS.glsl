#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;
layout(location = 3) in vec3 vertexTangent;

uniform mat4 MVP;
uniform mat4 modelTransform;

out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;
out mat3 TangentSpace0;
out vec3 Tangent0;
out vec3 BiTangent0;

void main()
{   
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
    TexCoord0 = vertexUV;
    Normal0 = (modelTransform * vec4(vertexNormals, 0.0)).xyz;
    WorldPos0 = (modelTransform * vec4(vertexPosition_modelspace, 1.0)).xyz;
	
	Tangent0 = (modelTransform * vec4(vertexTangent, 0.0)).xyz;
	BiTangent0 = cross(Normal0,Tangent0);
	TangentSpace0 = mat3(Tangent0, -cross(Normal0, Tangent0), Normal0);
}