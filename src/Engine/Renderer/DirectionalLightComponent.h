#pragma once
#include <Common/Maths/Maths.h>
#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(DirectionalLightComponent)

        DirectionalLightComponent(GameObjectReference parentObject);
        ~DirectionalLightComponent(void) override;

        void SetDirection(blaVec3 direction);
        blaVec3 GetDirection();

        void Update();

        blaU32 m_renderTicket;

    private:
        blaVec3 m_lightDirection;

    END_DECLARATION()
}
