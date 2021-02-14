// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderPass.h"
#include "Gpu/Interface.h"

using namespace BLA;

void BaseRenderPassInstance::Render() const
{
    Gpu::Interface::GetSingletonInstance()->Render(m_pImplementation, GetIterator());
}

void BaseRenderPassInstance::Setup(DefaultTODOToSpecifyRenderProgram& program)
{
    m_pImplementation = Gpu::Interface::GetSingletonInstance()->SetupRenderPass(m_pRenderPassDescriptor, program);
}

void BaseRenderPassInstance::BaseResetAttachment(const Gpu::BaseRenderPassAttachment* rpAttachment) const
{
    Gpu::Interface::GetSingletonInstance()->AttachToRenderPass(m_pImplementation, rpAttachment);
}

void BaseRenderPassInstance::BaseRegisterRenderPassObject(BaseRenderPassObject& object) const
{
    object.m_rpObjectHandle = Gpu::Interface::GetSingletonInstance()->RegisterRenderPassObjectBase(m_pImplementation, object);
}
