#pragma once
#include "../../../Common/StdInclude.h"
#include "Transform.h"

namespace BLAengine
{
	class GameObject;
	class BLACORE_API GameComponent
	{
	public:

		GameComponent() : m_parentObject(nullptr){}

		virtual void Update() = 0;

		const Transform& GetObjectTransform();

		GameObject* m_parentObject;
	};
}