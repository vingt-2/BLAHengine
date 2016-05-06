#version 330 core

// Interpolated values from the vertex shaders
in vec2 UVs;
in vec3 normal;
in vec3 lightVector;
in vec3 shadowPos;
flat in vec3 tangent;
flat in vec3 biTangent;

// Ouput data
out vec3 color;

uniform sampler2D texture;
uniform sampler2D normals;
uniform sampler2D shadowMap;
uniform mat4 modelView;

void main()
{
	vec3 diffuse = texture2D( texture, UVs ).rgb;
	vec3 nrms = texture2D( normals, UVs ).rgb;
    float depth = texture2D( shadowMap, shadowPos.xy ).z;
    
    float vis = 1.0;
    if(depth < shadowPos.z){
        vis = 0.5; 
    }
	color =  vis * diffuse;
}