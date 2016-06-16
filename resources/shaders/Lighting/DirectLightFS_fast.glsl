
#version 330 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D worldPosMap;
uniform sampler2D shadowMap;

uniform mat4 shadowMV;
uniform vec3 lightDirection;

in vec2 UV;
in vec3 lightVector;

void main(){
	vec3 diffuse = texture2D(diffuseMap, UV).rgb;
	vec3 normal = texture2D(normalMap, UV).rgb;
	vec3 worldPos = texture2D(worldPosMap, UV).rgb;
    vec4 shadowPos = shadowMV * vec4(worldPos, 1.0);
    vec2 shadowUV = shadowPos.xy;
    
    if(length(worldPos) > 1000)
    {
        color = vec3(0.3,0.3,0.7);
    }
    else
    {
	    float closestObjDepth = texture2D(shadowMap, shadowUV).r;
		
        float ambientComp = 0.1;
    
        float vis = max(1.5 - (3*shadowPos.z), ambientComp);

        if(shadowUV.x < 0.0 || shadowUV.x > 1.0){
            vis = ambientComp;
        }
        if(shadowUV.y < 0.0 || shadowUV.y > 1.0){
            vis = ambientComp;
        }
        if(shadowPos.z - 0.0001 > closestObjDepth ) {
            vis = ambientComp;
        }

        color = diffuse * vis;
    }
}