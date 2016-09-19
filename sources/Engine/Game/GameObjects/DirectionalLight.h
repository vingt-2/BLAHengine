#pragma once
#include "GameObject.h"
#include "..\..\..\Common\StdInclude.h"

namespace BLAengine
{
	class BLACORE_API DirectionalLight : GameObject
	{
	public:

		DirectionalLight(vec3 direction);
		~DirectionalLight(void);

		void SetDirection(vec3 direction);
		vec3 GetDirection();
		Transform* GetTransform() { return m_transform; };
		void Update();

	private:
		vec3 m_lightDirection;

	};
}
