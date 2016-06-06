#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 lightDirection;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 lightVector;

void main(){
	gl_Position =  vec4(vertexPosition_modelspace,1);
	UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;
	lightVector = lightDirection;
}

