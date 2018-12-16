#pragma once
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths/Maths.h"

#include "../Animation/SkeletalAnimation.h"
#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API AnimationComponent : public GameComponent
    {
    public:

        void Update() override;

        SkeletalMotion* m_animation;

        AnimationComponent(GameObject* parentObject);
        ~AnimationComponent(void);

    private:
    };
}
