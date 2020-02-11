#pragma once

#include <Core/GameComponent.h>

namespace BLAengine
{
    class IKChainJoint;

    BEGIN_COMPONENT_DECLARATION(BLAEngineNative, IKComponent)
        void Update() override;
        void Shutdown() override;

        IKChainJoint* m_ikChain = nullptr;
    END_DECLARATION()
}
