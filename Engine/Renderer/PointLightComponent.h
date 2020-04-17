#pragma once

#include <Maths/Maths.h>
#include <Core/GameComponent.h>

namespace BLAengine
{
	BeginComponentDeclaration(BLAEngineNative, PointLightComponent)      
        void    SetRadiosity(const blaVec3& radiosity) { m_radiosity = radiosity; };
        blaVec3   GetRadiosity() const     { return m_radiosity; };

		blaVec3* GetPosition() { return &m_position; }

		blaF32* GetLightRadius() { return &m_radius; }

		blaU32 m_renderTicket = 0;

        void Init() override;
        void Shutdown() override;
        void Update() override;

    private:

		blaF32 m_radius;
		blaVec3 m_position;
        blaVec3 m_radiosity;
    EndComponentDeclaration()
}