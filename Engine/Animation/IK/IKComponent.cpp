// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "IKComponent.h"

#include "Animation/IK/IKTools.h"

using namespace BLA;

BeginBehaviorDescription(IKComponent, Dependencies(RootSystem))
EndBehaviorDescription()

void IKComponent::Update()
{

}

void IKComponent::Shutdown()
{
    delete m_ikChain;
}
