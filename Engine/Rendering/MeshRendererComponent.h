// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/MeshAsset.h"
#include "Assets/Material.h"
#include "Rendering/GPU/StaticBuffer.h"

#include "Core/BehaviorComponent.h"

#define BLA_LINE_RENDER 0x0003

#include "Rendering/Renderer.h"
#include "Rendering\RenderCamera.h"

namespace BLA
{
    BeginBehaviorDeclaration(BLAEngine, MeshRendererComponent)
        
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
        Gpu::DynamicBuffer<blaMat4> m_modelTransformMatrix;

        bool init = false;
        PerspectiveCamera m_camera;

        Gpu::StaticBuffer<blaVec3>* m_vertPos;
        Gpu::StaticBuffer<blaVec2>* m_vertUV;
        Gpu::StaticBuffer<blaVec3>* m_vertNormal;
        Gpu::StaticBuffer<blaVec3>* m_vertTangent;
        Gpu::StaticBuffer<blaVec3>* m_vertBiTangent;
        Gpu::StaticBuffer<blaU32>* m_indices;

    EndBehaviorDeclaration()
}