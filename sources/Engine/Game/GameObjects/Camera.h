#pragma once
#include "GameChar.h"
#include "..\..\..\Common\StdInclude.h"

namespace BLAengine
{
	class BLACORE_API Camera : public GameObject
	{
	public:
		// Members
		Transform	m_viewTransform;
		vec2		m_fovSize;

		// States
		bool m_isControlEnabled;


		// Functions
		void UpdateView();
		void Update();

		Camera();
		~Camera(void);

	private:
	};


}