#pragma once
#include "GameObject.h"
#include "GameManager.h"
#include "..\..\..\Common\StdInclude.h"

namespace BLAengine
{
	class BLACORE_API GameManager : public GameObject
	{
	public:

		GameManager(void);
		~GameManager(void);

		void Update();
	};


}
