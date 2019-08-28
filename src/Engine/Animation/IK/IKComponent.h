#pragma once

#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
    class IKChainJoint;

    BEGIN_COMPONENT_DECLARATION(IKComponent)
        void Update() override;
        IKChainJoint* m_ikChain;

        IKComponent(GameObjectReference parentObject);
        ~IKComponent(void) override;
    END_DECLARATION()
}
