
#version 330 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D worldPosMap;
uniform vec4 lightPR;
uniform vec3 radiosity;

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
	
	vec3 toLight = lightPR.xyz - worldPos;

	float d = length(toLight);
	float d2 = d * d;
	
	float dORadius = d / lightPR.w;
	float dORadius4 = dORadius * dORadius * dORadius * dORadius;
	
    float lightFallOff = clamp(1 - dORadius4,0.0f , 1.0f);
	lightFallOff *= lightFallOff;
	
	lightFallOff /= (d2 + 1);
    
    color = (diffuse * radiosity * lightFallOff * max(dot(toLight / d, normal), 0));
}