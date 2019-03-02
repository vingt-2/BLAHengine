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
        struct CollisionContact
        {
            blaVec3 m_colPositionL;
            blaVec3 m_colPositionW;
            blaVec3 m_colNormalW;
            blaVec3 m_colNormalL;
			float m_t;
        };

        ColliderComponent(GameObjectReference parentObject):
            GameComponent(parentObject)
        {};

        ~ColliderComponent() {};

        virtual blaBool CollideWithRay(const Ray& ray, CollisionContact& outCollision ) = 0;
        virtual blaBool CollideWithCollider(const ColliderComponent& collider, CollisionContact& outCollision) = 0;
        virtual float GetBoundingRadius() = 0;
    };

    class BLACORE_API MeshColliderComponent : public ColliderComponent
    {
    public:
        MeshColliderComponent(GameObjectReference gameObject);
        ~MeshColliderComponent();

        void SetColliderMesh(TriangleMesh* mesh);

        void Update() {};

		blaBool CollideWithRay(const Ray &ray, CollisionContact& outCollision);
		blaBool CollideWithCollider(const ColliderComponent& collider, CollisionContact& outCollision);

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
        SphereColliderComponent(GameObjectReference parentObject): ColliderComponent(parentObject) {}

        void SetSize(float size) { m_boundingRadius = size; }

        ~SphereColliderComponent() {};

        void Update() {};

        float GetBoundingRadius() { return m_boundingRadius; }

		blaBool CollideWithRay(const Ray &ray, CollisionContact& outCollision);
		blaBool CollideWithCollider(const ColliderComponent& collider, CollisionContact& outCollision);

    private:
        float m_boundingRadius;
    };
}
