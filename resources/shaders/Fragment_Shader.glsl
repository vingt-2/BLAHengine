#version 330 core

// Interpolated values from the vertex shaders
in vec2 UVs;
in vec3 normal;
in vec3 lightVector;
in vec3 shadowPos;
flat in vec3 tangent;
flat in vec3 biTangent;

// Ouput data
out vec3 color;

uniform sampler2D texture;
uniform sampler2D normals;
uniform sampler2D shadowMap;
uniform mat4 modelView;

void main()
{
	vec3 diffuse = texture2D( texture, UVs ).rgb;
	vec3 nrms = texture2D( normals, UVs ).rgb;
	vec2 shadowUV = (shadowPos.xy);
    float closestObjDepth = texture2D( shadowMap, shadowUV ).r;

    float ambientComp = 0.5;
    
    float vis = max(1 - (3*shadowPos.z),ambientComp);

    if(shadowUV.x < 0.0 || shadowUV.x > 1.0){
        vis = ambientComp;
    }
    if(shadowUV.y < 0.0 || shadowUV.y > 1.0){
        vis = ambientComp;
    }
    if(shadowPos.z - 0.00001 > closestObjDepth ) {
        vis = ambientComp;
    }

    color = vis * diffuse * (1 + (dot(normal,lightVector) ));
}