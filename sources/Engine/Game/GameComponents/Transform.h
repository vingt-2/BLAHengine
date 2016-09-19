#pragma once
#include "../../../Common/Maths.h"
#include  "../../../Common/StdInclude.h"

namespace BLAengine
{

	class BLACORE_API Transform
	{
	public:
		vec3 m_position;
		mat3 m_rotation;
		vec3 m_scale;

		mat4 m_transformMatrix;


		void UpdateTransform();
		void SetRotationUsingEuler(vec3 eulerAngles);

		vec3 LocalDirectionToWorld(vec3 direction);
		vec3 LocalPositionToWorld(vec3 position);
		vec3 WorldDirectionToLocal(vec3 direction);
		vec3 WorldlPositionToLocal(vec3 position);


		Transform(void);
		~Transform(void);

	private:

	};

}