#pragma once
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths/Maths.h"

#include "../Animation/IKTools.h"
#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API IKComponent : public GameComponent
    {
    public:

        void Update() override;

        IKChainJoint* m_ikChain;

        IKComponent(GameObject* parentObject);
        ~IKComponent(void);

    private:
    };
}
