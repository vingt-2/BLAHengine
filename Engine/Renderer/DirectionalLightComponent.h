// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "Maths/Maths.h"
#include "Core/GameComponent.h"

namespace BLA
{
    BeginComponentDeclaration(BLAEngine, DirectionalLightComponent)
	
        void Init() override;
        void Shutdown() override;
        void Update();	
	
    private:
        int m_renderTicket;
	
    EndComponentDeclaration()
}
