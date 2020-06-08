#version 330 core

// Ouput data
out vec3 color;

uniform vec4 singleColor;

void main()
{
    color = singleColor;
}