// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/BehaviorComponent.h"

namespace BLA
{
    class IKChainJoint;

    BeginBehaviorDeclaration(BLAEngine, IKComponent)
        void Shutdown() override;

        IKChainJoint* m_ikChain = nullptr;
    EndBehaviorDeclaration()
}
