// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/MeshAsset.h"
#include "Assets/Material.h"
#include "Rendering/Gpu/StaticBuffer.h"
#include "Rendering/Gpu/DynamicBuffer.h"
#include "Core/BehaviorComponent.h"
#include "Pointer.h"

#include "Rendering/Renderer.h"
#include "Rendering/RenderCamera.h"

namespace BLA
{
    BeginBehaviorDeclaration(BLAEngine, MeshRendererComponent)
        friend class Renderer;

        DeclareRenderPassAttachment(TestMeshPassAttachment, ColorAttachments(Gpu::Formats::R8G8B8A8_UNORM), Gpu::Formats::D32_SFLOAT)

        DeclareRenderPassAttachment(GeometryPassAttachment, ColorAttachments(Gpu::Formats::R8G8B8A8_UNORM, Gpu::Formats::R8G8B8A8_UNORM, Gpu::Formats::R8G8B8A8_UNORM, Gpu::Formats::R8G8B8A8_UNORM), Gpu::Formats::D32_SFLOAT)

        typedef BLA::RenderPass<(MM<sizeof("MeshGeometryPass")-1>::crc32("MeshGeometryPass")), TestMeshPassAttachment, BLA::_RenderPassTemplateHelpers::RPIS<blaVec3, blaVec2, blaVec3, blaVec3, blaVec3>, BLA::_RenderPassTemplateHelpers::RPIS<blaMat4, blaMat4>, BLA::_RenderPassTemplateHelpers::RPIS<>> MeshGeometryPass; // model

        DeclareRenderPass(
            TestMeshPass,
            TestMeshPassAttachment,
            VertexAttributes(
                blaVec3), // ModelPos
            UniformBufferObjects(
                blaMat4, // mvp
                blaMat4)) // model

        int m_renderTicket = 0;

        blaBool m_Render;
        
        const blaMat4* GetTransformMatrix() const;

        void Init() override;
        void Shutdown() override;

        const MeshAsset* GetMeshAsset() const { return m_mesh; }

        blaString m_meshAssetName;

    private:
        bool AssignTriangleMesh(MeshAsset* mesh);
 
        MeshAsset* m_mesh = nullptr;
        Gpu::DynamicBuffer<blaMat4> m_modelTransformMatrix;
        Gpu::DynamicBuffer<blaMat4> m_MVP;
    
        bool init = false;
        PerspectiveCamera m_camera;

        Gpu::StaticBuffer<blaVec3>* m_vertPos;
        Gpu::StaticBuffer<blaVec2>* m_vertUV;
        Gpu::StaticBuffer<blaVec3>* m_vertNormal;
        Gpu::StaticBuffer<blaVec3>* m_vertTangent;
        Gpu::StaticBuffer<blaVec3>* m_vertBiTangent;
        Gpu::StaticBuffer<blaU32>* m_indices;

        blaOwnedPtr<TestMeshPass::RenderPassObject> m_renderPassObject;

    EndBehaviorDeclaration()
}