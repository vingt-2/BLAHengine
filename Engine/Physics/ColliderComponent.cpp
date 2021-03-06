// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "ColliderComponent.h"
#include "Core/TransformComponent.h"

using namespace BLA;

BeginComponentDescription(ColliderComponent)
EndComponentDescription()

inline blaVec3 Barycentric(blaVec3 p, blaVec3 a, blaVec3 b, blaVec3 c)
{
    blaVec3 r;
    blaVec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    r.y = (d11 * d20 - d01 * d21) / denom;
    r.z = (d00 * d21 - d01 * d20) / denom;
    r.x = 1.0f - r.y - r.z;
    return r;
}

MeshCollider::~MeshCollider()
{
    delete m_vertPosIndices;
    delete m_vertNormalIndices;
    delete m_collisionMesh;
}

void MeshCollider::SetColliderMesh(TriangleMesh* mesh)
{
    m_vertPosIndices = new blaVector<blaU32>;
    m_vertNormalIndices = new blaVector<blaU32>;
    mesh->GenerateTopoTriangleIndices(*m_vertPosIndices, *m_vertNormalIndices);
    m_triVertices = &(mesh->m_vertexPos);
    m_triNormals = &(mesh->m_vertexNormals);

    GenerateBoundingRadius();
    GenerateCollisionModel();
}

blaBool MeshCollider::CollideWithRay(const blaScaledTransform& transform, const Ray& ray, ColliderComponent::CollisionContact& outCollision)
{
    // blaScaledTransform transform = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();
    
    blaMat4 TransformComponentForCollider;
    transform.GetScaledTransformMatrix(TransformComponentForCollider);

    this->m_collisionMesh->setTransform(&(TransformComponentForCollider[0][0]));

    int triangleIndex;
    float colT;
    blaVec3 colPointL;

    bool collision = this->m_collisionMesh->threadSafeClosestRayCollision(&(ray.m_origin.x), &(ray.m_direction.x), triangleIndex, colT, &(colPointL.x));

    if (!collision)
    {
        return false;
    }

    outCollision.m_colPositionL = colPointL;
    outCollision.m_colPositionW = transform.LocalPositionToWorld(colPointL);

    blaVec3 contactNormals[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };
    blaVec3 contactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

    for (int k = 0; k < 3; k++)
    {
        blaU32 vertPosIndex = this->m_vertPosIndices->at(3 * triangleIndex + k);
        contactVertices[k] = this->m_triVertices->at((int)vertPosIndex);
        blaU32 vertNormalIndex = this->m_vertNormalIndices->at(3 * triangleIndex + k);
        if (m_triNormals->size() != 0) contactNormals[k] = m_triNormals->at((int)vertNormalIndex);
    }
    blaVec3 bar = Barycentric(outCollision.m_colPositionL, contactVertices[0], contactVertices[1], contactVertices[2]);
    //contactPoint.m_colNormalL = 0.3333333333f * (contactNormals[0] + contactNormals[1] + contactNormals[2]);
    outCollision.m_colNormalL = bar.x * contactNormals[0] + bar.y * contactNormals[1] + bar.z * contactNormals[2];
    outCollision.m_colNormalW = transform.LocalDirectionToWorld(outCollision.m_colNormalL);
    outCollision.m_t = colT;


    return true;
}

blaBool MeshCollider::CollideWithCollider(const ColliderComponent& collider, ColliderComponent::CollisionContact& outCollision)
{
    return false;
}

void MeshCollider::GenerateBoundingRadius()
{
    blaVec3 maxVert = blaVec3(0);

    for (auto v : *m_triVertices)
    {
        if (length(v) > length(maxVert))
        {
            maxVert = v;
        }
    }

    m_boundingRadius = length(maxVert);
}

void MeshCollider::GenerateCollisionModel()
{
    m_collisionMesh = newCollisionModel3D();
    for (size_t i = 0; i < m_vertPosIndices->size(); i += 3)
    {
        blaU32 i0 = m_vertPosIndices->at(i + 0);
        blaU32 i1 = m_vertPosIndices->at(i + 1);
        blaU32 i2 = m_vertPosIndices->at(i + 2);

        m_collisionMesh->addTriangle(&(m_triVertices->at(i0).x),
            &(m_triVertices->at(i1).x),
            &(m_triVertices->at(i2).x));

    }
    m_collisionMesh->finalize();
}

blaBool SphereCollider::CollideWithRay(const blaScaledTransform& transform, const Ray& ray, ColliderComponent::CollisionContact& outCollision)
{
    //blaScaledTransform transform = GetOwnerObject().GetComponent<TransformComponent>()->GetTransform();

    blaVec3 op = transform.GetPosition() - ray.m_origin;
    float t, eps = 1e-4f;
    float b = glm::dot(op, ray.m_direction);
    float det = b * b - glm::dot(op, op) + this->m_boundingRadius * this->m_boundingRadius;

    if (det < 0)
    {
        return false;
    }

    det = sqrt(det);
    t = (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);

    if (t == 0)
    {
        return false;
    }
    else
    {
        outCollision.m_colPositionW = ray.m_origin + ((float)t) * ray.m_direction;
        outCollision.m_colPositionL = transform.WorldPositionToLocal(outCollision.m_colPositionW);

        //printVector(contactPoint.m_colPositionW);

        outCollision.m_colNormalW = (outCollision.m_colPositionW - transform.GetPosition()) / m_boundingRadius;

        outCollision.m_colNormalL = (outCollision.m_colPositionL - transform.GetPosition());

        outCollision.m_t = t;
    }

    return true;
}

blaBool SphereCollider::CollideWithCollider(const ColliderComponent& collider, ColliderComponent::CollisionContact& outCollision)
{
    return false;
}
