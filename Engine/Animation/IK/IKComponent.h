#pragma once

#include <Core/GameComponent.h>

namespace BLAengine
{
    class IKChainJoint;

    BeginComponentDeclaration(BLAEngineNative, IKComponent)
        void Update() override;
        void Shutdown() override;

        IKChainJoint* m_ikChain = nullptr;
    EndComponentDeclaration()
}
