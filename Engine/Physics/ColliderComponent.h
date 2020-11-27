// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "Geometry/TriangleMesh.h"
#include "Maths/Ray.h"
#include "Core/GameComponent.h"

// Todo: Abstract around it for easier debug routines ?
// TODO: Also take the debug collisions out of the gizmo manager...
// Collision detection Library !
#include "External/libcoldet/coldet.h"

namespace BLA
{
    class MeshCollider;
    class SphereCollider;

    BeginComponentDeclaration(BLAEngine, ColliderComponent)

        struct CollisionContact
        {
            blaVec3 m_colPositionL;
            blaVec3 m_colPositionW;
            blaVec3 m_colNormalW;
            blaVec3 m_colNormalL;
            float m_t;
        };

        ~ColliderComponent() {};

        virtual blaBool CollideWithRay(const Ray& ray, CollisionContact& outCollision) { return false; };
        virtual blaBool CollideWithCollider(const ColliderComponent& collider, CollisionContact& outCollision) { return false; };
        virtual float GetBoundingRadius() { return 0; };
    EndComponentDeclaration()

    class MeshCollider
    {
    public:
        BLACORE_API ~MeshCollider();

        BLACORE_API void SetColliderMesh(TriangleMesh* mesh);

        BLACORE_API void Update() {};

        blaBool CollideWithRay(const blaScaledTransform& transform, const Ray& ray, ColliderComponent::CollisionContact& outCollision);
        blaBool CollideWithCollider(const ColliderComponent& collider, ColliderComponent::CollisionContact& outCollision);

        BLACORE_API float GetBoundingRadius() { return m_boundingRadius; }

        CollisionModel3D* m_collisionMesh;
        blaVector<blaU32>* m_vertPosIndices;
        blaVector<blaU32>* m_vertNormalIndices;
        blaVector<blaVec3>* m_triVertices;
        blaVector<blaVec3>* m_triNormals;
    private:
        float m_boundingRadius;

        void GenerateBoundingRadius();
        void GenerateCollisionModel();
    };

    class SphereCollider
    {
    public:
        BLACORE_API SphereCollider(Core::GameObject parentObject) {}

        BLACORE_API void SetSize(float size) { m_boundingRadius = size; }

        BLACORE_API ~SphereCollider() {};

        BLACORE_API void Update() {};

        BLACORE_API float GetBoundingRadius() { return m_boundingRadius; }

        blaBool CollideWithRay(const blaScaledTransform& transform, const Ray& ray, ColliderComponent::CollisionContact& outCollision);
        blaBool CollideWithCollider(const ColliderComponent& collider, ColliderComponent::CollisionContact& outCollision);

    private:
        float m_boundingRadius;
    };
}
