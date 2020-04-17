#pragma once
#include <Common/Maths/Maths.h>
#include "Engine/Core/GameComponent.h"

namespace BLAengine
{
    BEGIN_COMPONENT_DECLARATION(BLAEngineNative, TransformComponent)

        blaScaledTransform GetTransform();
        blaScaledTransform GetLocalTransform();

        void SetTransform(const blaScaledTransform& transform);
        void SetLocalTransform(const blaScaledTransform& transform);

        void Update() override;

        TransformComponent(GameObjectReference parent);
        ~TransformComponent(void);

    private:
        blaScaledTransform m_localTransform;
        blaS32 m_setCounter;

        blaScaledTransform m_cachedWorldTransform;
        blaS32 m_cachedParentSetCounter;
        blaBool m_dirtyCachedTransform;

    END_DECLARATION();
}