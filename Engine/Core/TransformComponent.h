// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

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

        // Cached world transform (used to be called m_cachedWorldTransform which I like better, but now it's exposed so unless I make it possible to have a custom name...)
        blaScaledTransform m_worldTransform;
        blaS32 m_cachedParentSetCounter = -1;
        blaBool m_dirtyCachedTransform = true;

    EndComponentDeclaration();
}