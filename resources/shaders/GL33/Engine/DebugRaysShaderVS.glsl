#version 330

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertex_color;

uniform mat4 MVP;
out vec3 vertexColor;
out vec4 position;

void main()
{   
    position = MVP * vec4(vertexPosition_modelspace, 1.0);
	gl_Position = position;
	vertexColor = vertex_color;
}