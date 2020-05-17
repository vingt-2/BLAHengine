
#version 330 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D worldPosMap;
uniform sampler2D depthMap;
uniform vec4 lightPR;

in vec4 position;

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
    vec2 UV = screenToUV(clipToScreen(position).xy);
    
	vec3 diffuse = texture2D(diffuseMap, UV).rgb;
	vec3 normal = texture2D(normalMap, UV).rgb;
	vec3 worldPos = texture2D(worldPosMap, UV).rgb;
	float depth = texture2D(depthMap, UV).x;
    
    float lightAmount = (1 - clamp(length(worldPos - lightPR.xyz) / lightPR.w ,0.0f , 1.0f));
    
    color = vec3(1,0,0,1);//diffuse * lightAmount * dot(normalize(lightPR.xyz - worldPos),normalize(normal));
}