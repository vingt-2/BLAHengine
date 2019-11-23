#pragma once

#include <Common/Maths/Maths.h>
#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
	BEGIN_COMPONENT_DECLARATION(PointLightComponent)      
        void    SetRadiosity(const blaVec3& radiosity) { m_radiosity = radiosity; };
        blaVec3   GetRadiosity() const     { return m_radiosity; };

		blaVec3* GetPosition() { return &m_position; }

		blaF32* GetLightRadius() { return &m_radius; }

		blaU32 m_renderTicket;

        void Update();

        PointLightComponent(GameObjectReference parentObject);
        
        ~PointLightComponent() override
        {};
    private:

		blaF32 m_radius;
		blaVec3 m_position;
        blaVec3 m_radiosity;
    END_DECLARATION()
}