#version 330 core

// Interpolated values from the vertex shaders
in vec2 UVs;
in vec3 normal;
in vec3 lightVector;
flat in vec3 tangent;
flat in vec3 biTangent;
in mat4 inverseTransform;

// Ouput data
out vec3 color;

uniform sampler2D texture;
uniform sampler2D normals;
uniform mat4 modelTransform;

void main()
{
    vec3 tangent1;
    if (abs(dot(normal,vec3(0,1,0))) > 0.9f)
    {
        tangent1 = vec3(0,1,0);
    }
    else
    {
        tangent1 = normalize(cross(normal,vec3(0,1,0)));
    }
    
    vec3 tangent2 = normalize(cross(normal,tangent1));
	vec3 diffuse = texture2D( texture, UVs ).rgb;
	vec3 nrms = texture2D( normals, UVs ).rgb;
    
    vec3 finalNormal = normal + tangent1*nrms.y + tangent2*nrms.z;
    
	//color = diffuse * (dot(normal,lightVector) );
	color = diffuse * dot(-finalNormal,lightVector);
}