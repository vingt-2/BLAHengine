#version 330 core

// Input Data
in vec3 vertexColor;

// Ouput data
layout(location = 0) out vec4 color;


void main()
{
    color = vec4(vertexColor,1);
}