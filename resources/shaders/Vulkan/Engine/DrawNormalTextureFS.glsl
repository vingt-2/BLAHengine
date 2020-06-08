#version 330 core

// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D colorTexture;

in vec2 UV;

void main(){
	color = 0.5f * (vec4(1.f) + texture2D(colorTexture, UV));
}