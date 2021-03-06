#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertex_color;

uniform mat4 MVP;
out vec3 vertexColor;

void main()
{   
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
	
	vertexColor = vertex_color;
}