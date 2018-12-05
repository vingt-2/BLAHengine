#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\..\..\Common\Maths\Maths.h"
#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API PointLight : public GameComponent
    {
    public:

        void    SetLightPosition(blaVec3 position)    { m_lightPosition = position; };
        void    SetLightRadius(float radius)       { m_lightRadius = radius; };
        void    SetLightIntensity(float intensity) { m_lightIntensity = intensity; };
        blaVec3    GetLightPosition()      { return m_lightPosition; };
        float   GetLightRadius()        { return m_lightRadius; };
        float   GetLightIntensity()     { return m_lightIntensity; };

        void Update();

        PointLight();
        PointLight(blaVec3 position, float radius, float intensity);
        ~PointLight();
    private:
        blaVec3 m_lightPosition;
        float m_lightRadius;
        float m_lightIntensity;
    };
}