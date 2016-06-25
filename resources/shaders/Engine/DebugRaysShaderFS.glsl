#version 330 core

// Input Data
in vec3 vertexColor;
in vec4 position;
// Ouput data
layout(location = 0) out vec4 color;

uniform sampler2D depthMap;
uniform sampler2D displayBuffer;

// Transform a clipspace coordinate to a screenspace one.
vec3 clipToScreen( vec4 v ) 
{
    return ( vec3( v.xyz ) / ( v.w * 2.0 ) );
}

// Transform a screenspace coordinate to a 2d vector for
// use as a texture UV lookup.
vec2 screenToUV( vec2 v )
{
    return 0.5 - vec2( v.xy ) * -1.0;
}

void main()
{
    vec3 p = clipToScreen(position);
    
    float depth = texture(depthMap, screenToUV(p.xy)).r;
    vec3 backPixelColor = texture(displayBuffer, screenToUV(p.xy)).rgb;
    
    float alphablend = gl_FragCoord.z > depth ? 0.1f : 1.0f; 

    color = vec4((alphablend) * vertexColor + (1-alphablend) * backPixelColor, 1);
}