// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/MeshAsset.h"
#include "Assets/Material.h"

#include "Core/BehaviorComponent.h"

#define BLA_LINE_RENDER 0x0003

#if NEW_VULKAN_RENDERER
#include "Renderer/Vulkan/VulkanRenderer.h"
#endif
namespace BLA
{
    BeginBehaviorDeclaration(BLAEngine, MeshRendererComponent)

        DeclareRenderPass(
            MeshTestRenderPass, 
            VertexAttributes(
                blaVec3, // Model Pos
                blaVec3), // normal 
            UniformValues(blaMat4), 
            1)
        
        friend class Renderer;
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
        blaMat4 m_modelTransformMatrix;

#if NEW_VULKAN_RENDERER 
        bool init = false;
        PerspectiveCamera m_camera;
#endif

	EndBehaviorDeclaration()
}