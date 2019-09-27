#pragma once

#include <Engine/Core/GameObject.h>

namespace BLAengine
{
    class Scene;
    class GizmoManager
    {
    public:
		GizmoManager();

		void Update();

    private:

		blaF32 m_sizeMultiplier;
		blaF32 m_arrowThickness;
		blaF32 m_arrowLengths;
		blaF32 m_distanceToCenter;
    };
}
