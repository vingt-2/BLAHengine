#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>

#include "GameComponent.h"

namespace BLAengine
{
    class IKChainJoint;

    class BLACORE_API IKComponent : public GameComponent
    {
    public:

        void Update() override;

        IKChainJoint* m_ikChain;

        IKComponent(GameObjectReference parentObject);
        ~IKComponent(void);

    private:
    };
}
