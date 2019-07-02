#pragma once

#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
    class IKChainJoint;

    class BLACORE_API IKComponent : public GameComponent
    {
    public:

        void Update() override;

        IKChainJoint* m_ikChain;

        IKComponent(GameObjectReference parentObject);
        ~IKComponent(void) override;

    private:
    };
}
