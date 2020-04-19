#include "IKComponent.h"

#include <Animation/IK/IKTools.h>

using namespace BLAengine;

BeginBehaviorDescription(IKComponent, Dependencies(RootSystem))
EndBehaviorDescription()

void IKComponent::Update()
{

}

void IKComponent::Shutdown()
{
    delete m_ikChain;
}
