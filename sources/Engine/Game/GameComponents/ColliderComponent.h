#pragma once
#include<Common\StdInclude.h>
#include <Engine/Assets/PolygonalMesh.h>
#include <Engine/Game/GameAlgebra/Ray.h>

// Collision detection Library !
#include <External/libcoldet/coldet.h>

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

        ColliderComponent(GameObject* parentObject):
            GameComponent(parentObject)
        {};

        ~ColliderComponent() {};

        virtual RayCollision CollideWithRay(Ray &ray) = 0;
        virtual RayCollision CollideWithCollider(ColliderComponent& collider) = 0;
        virtual float GetBoundingRadius() = 0;
    };

    class BLACORE_API MeshColliderComponent : public ColliderComponent
    {
    public:
        MeshColliderComponent(GameObject* gameObject);
        ~MeshColliderComponent();

        void SetColliderMesh(TriangleMesh* mesh);

        void Update() {};

        RayCollision CollideWithRay(Ray &ray);
        RayCollision CollideWithCollider(ColliderComponent& collider);
        float GetBoundingRadius() { return m_boundingRadius; }

        CollisionModel3D* m_collisionMesh;
        vector<blaU32>* m_vertPosIndices;
        vector<blaU32>* m_vertNormalIndices;
        vector<blaVec3>* m_triVertices;
        vector<blaVec3>* m_triNormals;
    private:
        float m_boundingRadius;

        void GenerateBoundingRadius();
        void GenerateCollisionModel();
    };

    class BLACORE_API SphereColliderComponent : public ColliderComponent
    {
    public:
        SphereColliderComponent(GameObject* parentObject): ColliderComponent(parentObject) {}

        void SetSize(float size) { m_boundingRadius = size; }

        ~SphereColliderComponent() {};

        void Update() {};

        float GetBoundingRadius() { return m_boundingRadius; }

        RayCollision CollideWithRay(Ray &ray);
        RayCollision CollideWithCollider(ColliderComponent& collider);
    private:
        float m_boundingRadius;
    };
}
