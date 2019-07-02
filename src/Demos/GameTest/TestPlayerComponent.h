#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>
#include <Engine/Animation/SkeletonAnimationData.h>

//TODO: Replace with contorller implementation from the input manager ...
#include <Engine/System/ControllerInputs/Dualshock4.h>

#include <Engine/Core/GameComponent.h>

namespace BLAengine
{
    class BLACORE_API TestPlayerComponent : public GameComponent
    {
    public:

        void Update() override;

		TestPlayerComponent(GameObjectReference parentObject);
        ~TestPlayerComponent(void) override;

    private:

		Dualshock4 m_controller;
    };
}
