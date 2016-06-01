#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D texture;

in vec2 UV;

void main(){
	float depthColor = texture2D(texture, UV).r;
	depthColor =  10 * (depthColor);
	color = vec4(depthColor,depthColor,depthColor,1);
}