// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Maths/Maths.h"
#include "Core/BehaviorComponent.h"

namespace BLA
{
    BeginBehaviorDeclaration(BLAEngine, PointLightComponent)

        blaU32 m_renderTicket = 0;
        blaBool m_draw = false;

        void Init() override;
        void Shutdown() override;

        blaVec3 m_radiosity = blaVec3(1.f,1.f,1.f);

    private:
        blaVec3 m_position;
    EndBehaviorDeclaration()
}