#version 330 core

// Input Data
in vec4 vertexColor;
in vec4 position;
// Ouput data
layout(location = 0) out vec4 color;

uniform mat4 MVP;
uniform sampler2D worldPosMap;
uniform sampler2D displayBuffer;

vec3 clipToScreen( vec4 v ) 
{
    return ( vec3( v.xyz ) / ( v.w * 2.0 ) );
}

vec2 screenToUV( vec2 v )
{
    return 0.5 - vec2( v.xy ) * -1.0;
}

void main()
{
    vec3 p = clipToScreen(position);
    
    vec4 bufferPosProjected = MVP * vec4(texture(worldPosMap, screenToUV(p.xy)).rgb, 1.0f);
	
    vec3 backPixelColor = texture(displayBuffer, screenToUV(p.xy)).rgb;

    color = vec4((vertexColor.a) * vertexColor.rgb + (1.0 - vertexColor.a) * backPixelColor, 1);
}