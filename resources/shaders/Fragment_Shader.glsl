#version 330 core

// Interpolated values from the vertex shaders
in vec2 UVs;
in vec3 normal;
in vec3 lightVector;

// Ouput data
out vec3 color;

uniform sampler2D texture;

void main()
{

// Output color = color specified in the vertex shader,
// interpolated between all 3 surrounding vertices
	vec3 diffuse = texture2D( texture, UVs ).rgb;

	color = diffuse * dot(normal,lightVector);
}