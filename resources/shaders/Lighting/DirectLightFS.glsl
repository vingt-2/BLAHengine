
#version 330 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D worldPosMap;
uniform sampler2D shadowMap;
uniform sampler2D depthMap;

uniform mat4 shadowMV;
uniform vec3 lightDirection;

in vec2 UV;
in vec3 lightVector;

void main(){
	vec3 diffuse = texture2D(diffuseMap, UV).rgb;
	vec3 normal = texture2D(normalMap, UV).rgb;
	vec3 worldPos = texture2D(worldPosMap, UV).rgb;
	float depth = texture2D(depthMap, UV).x;
    vec4 shadowPos = shadowMV * vec4(worldPos, 1.0);
    vec2 shadowUV = shadowPos.xy;
    
    float closestObjDepth = texture2D(shadowMap, shadowUV).r;
    
    float sunOrientation = 1 - clamp(0.3 + dot(vec3(0,1,0),lightDirection),0,1);
    sunOrientation *= sunOrientation;
    
    vec3 overalSunColor = vec3(sunOrientation, sunOrientation*0.5, sunOrientation*0.2);
    
    if(length(worldPos) > 1000)
    {
        float altitude = abs(0.4 + dot(normalize(worldPos),vec3(0.0,1.0,0.0)));
        
        vec3 skyColor = vec3(0.3 + (0.2 * altitude + 1.3*sunOrientation) ,0.3 + (0.5 * altitude + 0.6*sunOrientation) ,0.3 + altitude);
        
        float sunAligned = clamp(dot(normalize(worldPos),lightDirection),0.0,1.0);
        
        vec3 sunColor = clamp(pow(sunAligned,500),0,1) * vec3(3.2,0.8,0.5);
        
        color = (1-sunOrientation) * (skyColor + (1-sunOrientation) * (sunColor * (1+overalSunColor)));
    }
    else
    {
        float ambientComp = 0.1;
    
        float vis = max(1.5 - (3*shadowPos.z), ambientComp);
		
		//float bias = 0.001*tan(acos(dot(normal, lightVector)));
		//bias = clamp(bias, 0,0.01);

        if(shadowUV.x < 0.0 || shadowUV.x > 1.0){
            vis = ambientComp;
        }
        if(shadowUV.y < 0.0 || shadowUV.y > 1.0){
            vis = ambientComp;
        }
        if(shadowPos.z - 0.0001 > closestObjDepth ) {
            vis = ambientComp;
        }

		vec3 fogColor = (depth/0.99) * vec3(1,1,1);
		
        color = diffuse * (0.1 + 2 * vis * max(dot(normal, lightDirection),0) * (1-sunOrientation)*(1+overalSunColor));
    }
}