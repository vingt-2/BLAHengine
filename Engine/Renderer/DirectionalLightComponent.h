#pragma once
#include "Maths/Maths.h"
#include "Core/GameComponent.h"

namespace BLA
{
    BeginComponentDeclaration(BLAEngine, DirectionalLightComponent)

        friend class RenderingManager;
	
        void Init() override;
        void Shutdown() override;
        void Update();	
	
    private:
        int m_renderTicket;
	
    EndComponentDeclaration()
}
