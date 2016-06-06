#version 330 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D worldPosMap;
uniform sampler2D texCoordMap;

in vec2 UV;
in vec3 lightVector;

void main(){
	vec3 diffuse = texture2D(diffuseMap, UV).rgb;
	vec3 normal = texture2D(normalMap, UV).rgb;
	
	color = diffuse * (1 + dot(normal, lightVector) );
}