#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\..\Assets\PolygonalMesh.h"
#include "../GameAlgebra/Ray.h"

// Collision detection Library !
#include "../../Ext/libcoldet/coldet.h"

#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API ColliderComponent : public GameComponent
    {
    public:
        struct RayCollision
        {
            bool m_isValid;
            blaVec3 m_colPositionL;
            blaVec3 m_colPositionW;
            blaVec3 m_colNormalW;
            blaVec3 m_colNormalL;
            float m_t;
        };

        ColliderComponent() {};
        ~ColliderComponent() {};

        virtual RayCollision CollideWithRay(Ray &ray) = 0;
        virtual RayCollision CollideWithCollider(ColliderComponent& collider) = 0;
        virtual float GetBoundingRadius() = 0;
    };

    class BLACORE_API MeshCollider : public ColliderComponent
    {
    public:
        MeshCollider(TriangleMesh* mesh);
        ~MeshCollider();

        void Update() {};

        RayCollision CollideWithRay(Ray &ray);
        RayCollision CollideWithCollider(ColliderComponent& collider);
        float GetBoundingRadius() { return m_boundingRadius; }

        CollisionModel3D* m_collisionMesh;
        vector<glm::uint32>* m_vertPosIndices;
        vector<glm::uint32>* m_vertNormalIndices;
        vector<blaVec3>* m_triVertices;
        vector<blaVec3>* m_triNormals;
    private:
        float m_boundingRadius;

        void GenerateBoundingRadius();
        void GenerateCollisionModel();
    };

    class BLACORE_API SphereCollider : public ColliderComponent
    {
    public:
        SphereCollider(float size) 
        {
            m_boundingRadius = size;
        }

        ~SphereCollider() {};

        void Update() {};

        float GetBoundingRadius() { return m_boundingRadius; }

        RayCollision CollideWithRay(Ray &ray);
        RayCollision CollideWithCollider(ColliderComponent& collider);
    private:
        float m_boundingRadius;
    };
}
