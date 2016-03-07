#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBiTangent;

// Output data ; will be interpolated for each fragment.
out vec2 UVs;
out vec3 normal;
flat out  vec3 tangent;
flat out  vec3 biTangent;
out vec3 lightVector;
out mat4 inverseTransform;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 modelTransform;
uniform vec3 directionalLight;

void main()
{	

	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	UVs = vertexUV;
	normal = vertexNormals;

	inverseTransform  = inverse(modelTransform);
	
	lightVector = normalize((inverseTransform * vec4(directionalLight,0)).rgb);
    
    tangent = vertexTangent;
    biTangent = biTangent;
}

