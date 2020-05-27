// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "Core/BehaviorComponent.h"

namespace BLA
{
    BeginBehaviorDeclaration(BLAEngine, PointLightComponent)
        void    SetRadiosity(const blaVec3& radiosity) { m_radiosity = radiosity; };
        blaVec3   GetRadiosity() const     { return m_radiosity; };

        blaVec3* GetPosition() {return &m_position; }

		blaF32* GetLightRadius() { return &m_radius; }

		blaU32 m_renderTicket = 0;
        blaBool m_draw = false;

        void Init() override;
        void Shutdown() override;

    private:

		blaF32 m_radius;
        blaVec3 m_position;
        blaVec3 m_radiosity;
    EndBehaviorDeclaration()
}