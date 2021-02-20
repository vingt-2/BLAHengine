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

        DeclareRenderPass(MeshGeometryPass, 
            TestMeshPassAttachment, 
            VertexAttributes(blaVec3 /*Positions*/, blaVec2 /*UVs*/, blaVec3 /*Normals*/, blaVec3 /*Tangents*/, blaVec3 /*Bi-Tangents*/),
            UniformBufferObjects(blaMat4, blaMat4), 
            OpaqueUniforms(Gpu::Sampler))

        DeclareRenderPass(
            TestMeshPass,
            TestMeshPassAttachment,
            VertexAttributes(
                blaVec3), // ModelPos
            UniformBufferObjects(
                blaMat4, // mvp
                blaMat4))
            //OpaqueUniforms(Gpu::Sampler)) // model

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

        Gpu::Sampler* m_diffuseTextureSampler;

        blaOwnedPtr<TestMeshPass::RenderPassObject> m_renderPassObjects;

    EndBehaviorDeclaration()
}