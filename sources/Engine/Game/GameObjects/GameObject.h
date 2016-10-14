#pragma once
#include "../GameComponents/MeshRenderer.h"
#include "../GameComponents/RigidBody.h"
#include "..\..\..\Common\StdInclude.h"

namespace BLAengine
{
	class BLACORE_API GameObject
	{
	public:
		Transform* m_transform;
		string m_objectName;

		virtual void Update() = 0;

		void UpdateTransform();
		void SetParent(GameObject* parent) { m_parent = parent; }
		GameObject* GetParent() { return m_parent; }


		GameObject(void);
		~GameObject(void);
	
	private:

		GameObject* m_parent;
	};
}
