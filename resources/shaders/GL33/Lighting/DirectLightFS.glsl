
#version 330 core

// Ouput data
layout(location = 0) out vec3 color;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D worldPosMap;
uniform sampler2DShadow shadowMap;

uniform mat4 shadowMV;
uniform vec3 lightDirection;
uniform vec3 eyePosition;

in vec2 UV;

void main()
{
	vec3 diffuse = texture2D(diffuseMap, UV).rgb;
	vec3 normal = texture2D(normalMap, UV).rgb;
	vec3 worldPos = texture2D(worldPosMap, UV).rgb;
    
    float sunOrientation = 1 - clamp(0.2 + dot(vec3(0,1,0),lightDirection),0,1);
    float sunOrientationNoOff =  1 - clamp(dot(vec3(0,1,0),lightDirection),0,1);
    sunOrientation *= sunOrientation;
    sunOrientationNoOff *= sunOrientationNoOff;
    
    vec3 overalSunColor = vec3(sunOrientation, sunOrientation*0.5, sunOrientation*0.2);
    
	// This is for the sky rendering ...
    if(length(worldPos) > 3000)
    {
        float altitude = dot(normalize(worldPos),vec3(0.0,1.0,0.0));
        
        vec3 skyBottomColor = (1-sunOrientationNoOff) * vec3(0.7,0.8,1)  + sunOrientationNoOff * vec3(1.5,0.4,0);
        vec3 skyTopColor = (1-sunOrientationNoOff) * vec3(0.5,0.6,1.5)  + sunOrientationNoOff * vec3(0.6,0.2,0.2);
        vec3 skyColor = (1-sunOrientationNoOff) * vec3(0.1) + 
						clamp(altitude,0,1) * skyTopColor +
						clamp((1-altitude),0,1) * skyBottomColor;
        
        float sunAligned = clamp(dot(normalize(worldPos),lightDirection),0.0,1.0);
        
        vec3 sunColor = clamp(altitude,0,1) * ((2-sunOrientation) * clamp(pow(sunAligned,700),0,1)) * vec3(3.2,0.8,0.5) +  (0.1f * clamp(pow(sunAligned,3),0,1) * vec3(1));
        
        color = clamp((1-sunOrientation),0,1) * (skyColor + (1-sunOrientation) * (sunColor * (1+overalSunColor))) + clamp((sunOrientation),0,1) * vec3(0,0,0.1);
    }
	// And this is the standard shader...
    else
    {
        float ambientLight = 0.5f;
		
        vec4 shadowPos = shadowMV * vec4(worldPos, 1.0);
        //shadowPos /= shadowPos.w;
        vec2 shadowUV = shadowPos.xy;
		float xOffset = 1.0f/8192;
        float yOffset = 1.0f/8192;
        
        float shadowFactor = 0.0;
        
        float bias = 0.0001*tan(acos(dot(normal, lightDirection)));
		bias = clamp(bias, 0,0.0002);
        
        for (int y = -2 ; y <= 2 ; y++) 
        {
            for (int x = -2 ; x <= 2 ; x++) 
            {
                vec2 Offsets = vec2(x * xOffset, y * yOffset);
                vec3 UVC = vec3(shadowUV + Offsets, shadowPos.z - bias);
                shadowFactor += texture(shadowMap, UVC);
            }
        }

        float vis = shadowFactor / 16.0f;
		
       // color = diffuse * (3 * vis * max(dot(normal, lightDirection),0.3f)) * (1-sunOrientation) * ((0.5-sunOrientation) + overalSunColor);
	   color = diffuse * (max(vis,0.1f) * max(dot(normal, lightDirection),ambientLight)+ambientLight);
	}
}