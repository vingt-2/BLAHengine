#pragma once
#include <Common/Maths/Maths.h>
#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
    class BLACORE_API DirectionalLight : public GameComponent
    {
    public:

        DirectionalLight(GameObjectReference parentObject);
        ~DirectionalLight(void) override;

        void SetDirection(blaVec3 direction);
        blaVec3 GetDirection();

        void Update();

        blaU32 m_renderTicket;

    private:
        blaVec3 m_lightDirection;

    };
}
