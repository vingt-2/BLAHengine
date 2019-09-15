#version 330

layout(location = 0) in vec3 vertexPosition_worldSpace;
layout(location = 1) in vec4 vertex_color;

uniform mat4 MVP;
out vec4 vertexColor;
out vec4 position;

void main()
{   
    position = MVP * vec4(vertexPosition_worldSpace, 1.0);
	gl_Position = position;
	vertexColor = vertex_color;
}