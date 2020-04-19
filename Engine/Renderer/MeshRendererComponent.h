#pragma once
#include <StdInclude.h>
#include <Assets/MeshAsset.h>
#include <Assets/Material.h>

#include <Core/BehaviorComponent.h>

#define BLA_LINE_RENDER 0x0003

namespace BLAengine
{
    BeginBehaviorDeclaration(BLAEngineNative, MeshRendererComponent)
        friend class Renderer;
        int m_renderTicket = 0;

        blaVector<Material*> m_materials;

        blaS32 m_renderType = 0x0004; //GL_TRIANGLES

        blaString MeshAssetName;
        blaString MaterialName;
        
		const blaMat4* GetTransformMatrix() const;

        void Init() override;
        void Shutdown() override;

        const MeshAsset* GetMeshAsset() const { return m_mesh; }

    private:
        bool AssignTriangleMesh(MeshAsset* mesh);
        bool AssignMaterial(Material* material, int matIndx);

        MeshAsset* m_mesh = nullptr;
        blaMat4 m_modelTransformMatrix;
	EndBehaviorDeclaration()
}