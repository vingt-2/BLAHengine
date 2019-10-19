#pragma once

#include <Engine/Core/GameObject.h>

namespace BLAengine
{
    class Scene;
	class GizmoControl;

	class GizmoControl
	{
	public:
		GizmoControl(GameObjectReference object) : m_controlledObject(object), m_sizeMultiplier(1.f){}

		virtual void Update();

		bool ControlsObject(GameObjectReference obj) { return obj == m_controlledObject; }

	protected:
		GameObjectReference m_controlledObject;
		blaF32 m_sizeMultiplier;
	};

	class TranslationGizmoControl : public GizmoControl
	{
	public:
		TranslationGizmoControl(GameObjectReference object): GizmoControl(object), m_arrowLengths(1.f), m_arrowThickness(0.1f), m_distanceToCenter(0.2f), m_selectedAxis(NONE)
		{}

		void Update() override;

	protected:
		blaF32 m_arrowThickness;
		blaF32 m_arrowLengths;
		blaF32 m_distanceToCenter;

		blaVec3 m_startingObjectOrigin;
		blaVec3 m_startingClickPosition;
		enum ESelectedAxis : blaU8
		{
			NONE = 0,
			X_AXIS,
			Y_AXIS,
			Z_AXIS,
			CENTER_AXIS
		} m_selectedAxis;
	};

    class GizmoManager
    {
    public:
		GizmoManager();

		void Update();

    private:

		GizmoControl* m_gizmoController;
    };
}
