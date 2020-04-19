#pragma once

#include <Core/BehaviorComponent.h>

namespace BLAengine
{
    class IKChainJoint;

    BeginBehaviorDeclaration(BLAEngineNative, IKComponent)
        void Shutdown() override;

        IKChainJoint* m_ikChain = nullptr;
    EndBehaviorDeclaration()
}
