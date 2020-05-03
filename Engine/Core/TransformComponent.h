#pragma once
#include "Maths/Maths.h"
#include "Core/GameComponent.h"

namespace BLA
{
    BeginComponentDeclaration(BLAEngine, TransformComponent)

        BLACORE_API blaScaledTransform GetTransform();
		BLACORE_API blaScaledTransform GetLocalTransform() const;

        BLACORE_API void SetTransform(const blaScaledTransform& transform);
        BLACORE_API void SetLocalTransform(const blaScaledTransform& transform);

    private:
        blaScaledTransform m_localTransform;
        blaS32 m_setCounter = 0;

        blaScaledTransform m_cachedWorldTransform;
        blaS32 m_cachedParentSetCounter = -1;
        blaBool m_dirtyCachedTransform = true;

    EndComponentDeclaration();
}