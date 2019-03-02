#pragma once
#include <Common\StdInclude.h>
#include <Engine/Assets/MeshAsset.h>
#include <Engine/Assets/Material.h>

#include "GameComponent.h"

#define BLA_LINE_RENDER 0x0003

namespace BLAengine
{
    class BLACORE_API MeshRendererComponent : public GameComponent
    {
    public:

        int m_renderTicket;

        MeshAsset* m_mesh;

        vector<Material*> m_materials;

        blaU32 m_renderType;

        MeshRendererComponent(GameObjectReference parentObject);
        ~MeshRendererComponent(void);

        bool AssignTriangleMesh(MeshAsset* mesh);
        bool AssignMaterial(Material* material, int matIndx);
        
        blaMat4* GetTransformMatrix() const;
        
        string ToString(void);

        void Update();

    private:

        blaMat4* m_modelTransformMatrix;
    };
}