#include "IKComponent.h"

#include <Animation/IK/IKTools.h>

using namespace BLAengine;

BEGIN_COMPONENT_DESCRIPTION(IKComponent)
END_DESCRIPTION()

void IKComponent::Update()
{

}

void IKComponent::Shutdown()
{
    delete m_ikChain;
}
