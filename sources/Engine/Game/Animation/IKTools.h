#pragma once
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths/Maths.h"

class IKChain
{
public:
	std::vector<IKChain*> m_children;

	blaPosQuat m_localTransform;
};