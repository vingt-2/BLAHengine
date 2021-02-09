// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderPass.h"
#include "Gpu/Interface.h"

using namespace BLA;

void BaseRenderPassInstance::Setup(DefaultTODOToSpecifyRenderProgram& program)
{
    m_pImplementation = Gpu::Interface::GetSingletonInstance()->SetupRenderPass(m_pRenderPassDescriptor, program);
}

void BaseRenderPassInstance::BaseResetAttachment(const Gpu::BaseRenderPassAttachment* rpAttachment) const
{
    Gpu::Interface::GetSingletonInstance()->AttachToRenderPass(m_pImplementation, rpAttachment);
}

void BaseRenderPassInstance::BaseRegisterRenderPassObject(const BaseRenderPassObject& object) const
{
    Gpu::Interface::GetSingletonInstance()->RegisterRenderPassObjectBase(m_pImplementation, object);
}
