#version 330 core

// Ouput data
layout(location = 0) out vec4 fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    fragmentdepth = vec4(1,1,0,1);
}