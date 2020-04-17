#include "IKComponent.h"

#include <Animation/IK/IKTools.h>

using namespace BLAengine;

BeginComponentDescription(IKComponent)
EndComponentDescription()

void IKComponent::Update()
{

}

void IKComponent::Shutdown()
{
    delete m_ikChain;
}
