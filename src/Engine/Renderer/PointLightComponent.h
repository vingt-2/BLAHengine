#pragma once

#include <Common/Maths/Maths.h>
#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
	BEGIN_COMPONENT_DECLARATION(PointLightComponent)

        blaVec3 GetLightPosition() const;
        float   GetLightRadius() const;        
        
        void    SetRadiosity(const blaVec3& radiosity) { m_radiosity = radiosity; };
        blaVec3   GetRadiosity() const     { return m_radiosity; };

        void Update();

        PointLightComponent(GameObjectReference parentObject);
        
        ~PointLightComponent() override
        {};
    private:

        blaVec3 m_radiosity;
    END_DECLARATION()
}