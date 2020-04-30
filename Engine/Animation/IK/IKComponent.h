#pragma once

#include <Core/BehaviorComponent.h>

namespace BLA
{
    class IKChainJoint;

    BeginBehaviorDeclaration(BLAEngine, IKComponent)
        void Shutdown() override;

        IKChainJoint* m_ikChain = nullptr;
    EndBehaviorDeclaration()
}
