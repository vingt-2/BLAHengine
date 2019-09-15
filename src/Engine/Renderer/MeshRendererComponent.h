#pragma once
#include <Common\StdInclude.h>
#include <Engine/Assets/MeshAsset.h>
#include <Engine/Assets/Material.h>

#include <Engine/Core/GameComponent.h>

#define BLA_LINE_RENDER 0x0003

namespace BLAengine
{
	BEGIN_COMPONENT_DECLARATION(MeshRendererComponent)
        int m_renderTicket;

        MeshAsset* m_mesh;

        blaVector<Material*> m_materials;

        blaS32 m_renderType;

        MeshRendererComponent(GameObjectReference parentObject);

        bool AssignTriangleMesh(MeshAsset* mesh);
        bool AssignMaterial(Material* material, int matIndx);
        
		const blaMat4* GetTransformMatrix() const;

        void Update();

    private:

        blaMat4 m_modelTransformMatrix;
	END_DECLARATION()
}