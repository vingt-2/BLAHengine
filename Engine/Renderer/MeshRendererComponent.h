// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Assets/MeshAsset.h"
#include "Assets/Material.h"
#include "Renderer/GPU/StaticBuffer.h"

#include "Core/BehaviorComponent.h"

#define BLA_LINE_RENDER 0x0003

#include "Renderer/Renderer.h"
#include "Renderer\RenderCamera.h"

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
        blaMat4 m_modelTransformMatrix;

        bool init = false;
        PerspectiveCamera m_camera;

        Gpu::Resource<Gpu::StaticBuffer<blaVec3>> m_vertPos;
        Gpu::Resource<Gpu::StaticBuffer<blaVec3>> m_vertNormal;
        Gpu::Resource<Gpu::StaticBuffer<blaU32>> m_indices;

    EndBehaviorDeclaration()
}