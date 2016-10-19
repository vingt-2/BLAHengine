#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\..\..\Common\Maths.h"
#include "GameComponent.h"

namespace BLAengine
{
	class BLACORE_API DirectionalLight : public GameComponent
	{
	public:

		DirectionalLight(vec3 direction);
		~DirectionalLight(void);

		void SetDirection(vec3 direction);
		vec3 GetDirection();
		const Transform& GetTransform();
		void Update();

	private:
		vec3 m_lightDirection;

	};
}
