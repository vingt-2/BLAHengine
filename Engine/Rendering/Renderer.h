// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved
#pragma once

#include "Core/InspectableVariables.h"
#include "Gpu/Image.h"
#include "Gpu/Resource.h"
#include "System/RenderWindow.h"
#include "Pointer.h"
#include "BLASingleton.h"

// Temp include
#include "RenderPass.h"

namespace BLA
{
    using BLARgba = blaU32;
    
    struct RenderTarget
    {
        typedef void (*OnChange)(RenderTarget*);

        void RegisterOnChangeCallback(OnChange onchangeCb) { m_callbacks.push_back(onchangeCb); }

    protected:
        void OnChangeCalls();

    private:
        blaVector<OnChange> m_callbacks;
    };
    
    struct OffscreenRenderTarget : RenderTarget
    {
        void OnChange() { OnChangeCalls(); }

        blaOwnedPtr<Gpu::Image<Gpu::Formats::R8G8B8A8_UNORM>> m_color;
        blaOwnedPtr<Gpu::Image<Gpu::Formats::D32_SFLOAT>> m_depth;
    };

    class RenderWindow;
    struct BaseRenderPassInstance;
    
    namespace Gpu
    {
        struct RenderPassDescriptor;
    }
    
    class Renderer
    {
        BLA_DECLARE_SINGLETON(Renderer)

    public:
        Renderer(GLFWRenderWindow* pRenderWindow);

        BLACORE_API bool Update();

        //TODO: Remove, improve DevGuiwindow design ...
        void SetViewportSize(blaIVec2 viewportSize);
        RenderWindow* GetRenderWindow();
        ~Renderer(void) = default;

        OffscreenRenderTarget m_offscreenBuffer;

        template<typename RenderPass>
        void AddRenderPassInstance(blaStringId instanceId, const typename RenderPass::RenderPassAttachment& renderPassAttachment, const DefaultTODOToSpecifyRenderProgram& renderProgram)
        {
            // TODO: static_assert validity of RenderPass

            const Gpu::RenderPassDescriptor* descriptor = RenderPass::GetSingletonInstance()->m_pRenderPassDescriptor;

            m_renderPassInstances.push_back(std::make_pair(instanceId, new typename RenderPass::RenderPassInstance(renderPassAttachment, renderProgram)));
        }

        // TODO: Make it go via an abstraction GetRenderPassInstance which is exported because we shouldn't be working with other dlls vectors ...
        // (This code is generated on the calling DLL via templating...)
        template<typename RenderPass>
        typename RenderPass::RenderPassInstance* GetRenderPassInstance(blaStringId instanceId)
        {
            for(blaSize i = 0; i < m_renderPassInstances.size(); i++)
            {
                if(m_renderPassInstances[i].first == instanceId)
                {
                    return static_cast<typename RenderPass::RenderPassInstance*>(m_renderPassInstances[i].second->m_instanceTypeId == RenderPass::ms_renderPassId ? m_renderPassInstances[i].second : nullptr);
                }
            }
            return nullptr;
        }

    protected:

        void CreateOrUpdateRenderTargets();
        GLFWRenderWindow* m_renderWindow;
        blaIVec2 m_viewPortExtents;

        //TODO: This will become the render pass graph, eventually ...
        blaVector<blaPair<blaStringId, BaseRenderPassInstance*>> m_renderPassInstances;

        bool m_firstUpdate = true;
    };
}
