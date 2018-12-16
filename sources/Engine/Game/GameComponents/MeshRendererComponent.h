#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "../../Assets/PolygonalMesh.h"
#include "../../Assets/Material.h"
#include "GameComponent.h"

#define BLA_LINE_RENDER 0x0003

namespace BLAengine
{
    class BLACORE_API MeshRendererComponent : public GameComponent
    {
    public:

        int m_renderTicket;

        TriangleMesh* m_mesh;

        vector<Material*> m_materials;

        GLuint m_renderType;

        MeshRendererComponent(GameObject* parentObject);
        ~MeshRendererComponent(void);

        bool AssignTriangleMesh(TriangleMesh* mesh);
        bool AssignMaterial(Material* material, int matIndx);
        
        blaMat4* GetTransformMatrix() const;
        
        string ToString(void);

        void Update();

    private:

        blaMat4* m_modelTransformMatrix;
    };
}