// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Renderer.h"
#include "RenderPass.h"

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(RenderPassRegistry)

void RenderPassRegistry::__RegisterRenderPass(blaStringId stringId, blaU32 id, blaU32 attachmentCount,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& vertexAttributesDescriptors,
    blaVector<BLA::Core::InspectableVariables::ExposedVarTypeDescriptor*>& uniformValuesDescriptor)
{
    BLA_ASSERT(m_registry.find(id) == m_registry.end());

    m_registry.insert(std::make_pair(id, Gpu::RenderPassDescriptor{ stringId, attachmentCount, nullptr, vertexAttributesDescriptors, uniformValuesDescriptor }));
}

Gpu::RenderPassDescriptor* RenderPassRegistry::GetRenderPassEntry(blaU32 id)
{
    RenderPassRegistryStorage::iterator it = m_registry.find(id);
    if(it != m_registry.end())
    {
        return &it->second;
    }
    return nullptr;
}

const Gpu::RenderPassDescriptor* RenderPassRegistry::GetRenderPassEntry(blaU32 id) const
{
    RenderPassRegistryStorage::const_iterator it = m_registry.find(id);
    if (it != m_registry.end())
    {
        return &it->second;
    }
    return nullptr;
}

void RenderPassRegistry::GetAllRenderPassIDs(blaVector<blaU32>& stringIds) const
{
    for(auto e : m_registry)
    {
        stringIds.push_back(e.first);
    }
}
