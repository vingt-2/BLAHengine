#pragma once
#include "GameObject.h"


class PointLight : GameObject
{
public:
    
    void    SetLightPosition(vec3 position)    { m_lightPosition = position;      };
    void    SetLightRadius(float radius)       { m_lightRadius = radius;          };
    void    SetLightIntensity(float intensity) { m_lightIntensity = intensity;    };
    vec3    GetLightPosition()      { return m_lightPosition;     };
    float   GetLightRadius()        { return m_lightRadius;       };
    float   GetLightIntensity()     { return m_lightIntensity;    };
    
	Transform* GetTransform() { return m_transform; };
	void Update();
    
    PointLight();
    PointLight(vec3 position,float radius,float intensity);
    ~PointLight();
private:
    vec3 m_lightPosition;
    float m_lightRadius;
    float m_lightIntensity;
};