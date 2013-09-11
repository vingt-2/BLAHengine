#version 330 core

// Interpolated values from the vertex shaders
in vec2 UVs;
in vec3 normal;
in vec3 lightVector;
in mat4 inverseTransform;

// Ouput data
out vec3 color;

uniform sampler2D texture;
uniform sampler2D normals;
uniform mat4 modelTransform;

void main()
{
	vec3 diffuse = texture2D( texture, UVs ).rgb;
	vec3 nrms = texture2D( normals, UVs ).rgb;
	//color = diffuse * (dot(normal,lightVector) );
	color = diffuse * (dot(-normal,lightVector) * dot((inverseTransform * vec4(-nrms,0.f)).rgb,lightVector));
}