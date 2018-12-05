#include "ColliderComponent.h"
using namespace BLAengine;

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

MeshCollider::MeshCollider(TriangleMesh* mesh)
{
    m_vertPosIndices = new vector<glm::uint32>;
    m_vertNormalIndices = new vector<glm::uint32>;
    mesh->GenerateTopoTriangleIndices(*m_vertPosIndices, *m_vertNormalIndices);
    m_triVertices = &(mesh->m_vertexPos);
    m_triNormals = &(mesh->m_vertexNormals);

    GenerateBoundingRadius();
    GenerateCollisionModel();
}

MeshCollider::~MeshCollider()
{
    m_collisionMesh->~CollisionModel3D();
    m_vertPosIndices->clear();
    m_vertNormalIndices->clear();
    m_vertPosIndices->~vector();
    m_vertNormalIndices->~vector();
}

ColliderComponent::RayCollision MeshCollider::CollideWithRay(Ray & ray)
{
    ObjectTransform transform = this->GetObjectTransform();

    blaMat4 objectTransformForCollider = transform.GetScaledTransformMatrix();

    this->m_collisionMesh->setTransform(&(objectTransformForCollider[0][0]));

    int triangleIndex;
    float colT;
    blaVec3 colPointL;

    bool collision = this->m_collisionMesh->threadSafeClosestRayCollision(&(ray.m_origin.x), &(ray.m_direction.x), triangleIndex, colT, &(colPointL.x));

    MeshCollider::RayCollision contactPoint;
    if (!collision)
    {
        contactPoint.m_isValid = false;
    }
    else
    {
        contactPoint.m_isValid = true;
        contactPoint.m_colPositionL = colPointL;
        contactPoint.m_colPositionW = transform.LocalPositionToWorld(colPointL);

        blaVec3 contactNormals[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };
        blaVec3 contactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

        for (int k = 0; k < 3; k++)
        {
            glm::uint32 vertPosIndex = this->m_vertPosIndices->at(3 * triangleIndex + k);
            contactVertices[k] = this->m_triVertices->at((int)vertPosIndex);
            glm::uint32 vertNormalIndex = this->m_vertNormalIndices->at(3 * triangleIndex + k);
            if (m_triNormals->size() != 0) contactNormals[k] = m_triNormals->at((int)vertNormalIndex);
        }
        blaVec3 bar = Barycentric(contactPoint.m_colPositionL, contactVertices[0], contactVertices[1], contactVertices[2]);
        //contactPoint.m_colNormalL = 0.3333333333f * (contactNormals[0] + contactNormals[1] + contactNormals[2]);
        contactPoint.m_colNormalL = bar.x * contactNormals[0] + bar.y * contactNormals[1] + bar.z * contactNormals[2];
        contactPoint.m_colNormalW = transform.LocalDirectionToWorld(contactPoint.m_colNormalL);
        contactPoint.m_t = colT;
    }
    return contactPoint;
}

ColliderComponent::RayCollision MeshCollider::CollideWithCollider(ColliderComponent & collider)
{
    return RayCollision();
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
        glm::uint32 i0 = m_vertPosIndices->at(i + 0);
        glm::uint32 i1 = m_vertPosIndices->at(i + 1);
        glm::uint32 i2 = m_vertPosIndices->at(i + 2);

        m_collisionMesh->addTriangle(&(m_triVertices->at(i0).x),
            &(m_triVertices->at(i1).x),
            &(m_triVertices->at(i2).x));

    }
    m_collisionMesh->finalize();
}

ColliderComponent::RayCollision SphereCollider::CollideWithRay(Ray& ray)
{
    ColliderComponent::RayCollision contactPoint;
    ObjectTransform transform = this->GetObjectTransform();

    blaVec3 op = transform.GetPosition() - ray.m_origin;
    float t, eps = 1e-4f;
    float b = glm::dot(op, ray.m_direction);
    float det = b * b - glm::dot(op,op) + this->m_boundingRadius * this->m_boundingRadius;

    if (det < 0)
    {
        contactPoint.m_isValid = false;
    }
    else
    {
        det = sqrt(det);
        t = (t = b - det) > eps ? t : ((t = b + det) > eps ? t : 0);

        if (t == 0)
        {
            contactPoint.m_isValid = false;
        }
        else
        {
            contactPoint.m_isValid = true;
            contactPoint.m_colPositionW = ray.m_origin + ((float) t) * ray.m_direction;
            contactPoint.m_colPositionL = transform.WorldPositionToLocal(contactPoint.m_colPositionW);
            
            //printVector(contactPoint.m_colPositionW);

            contactPoint.m_colNormalW = (contactPoint.m_colPositionW - transform.GetPosition()) / m_boundingRadius;

            contactPoint.m_colNormalL = (contactPoint.m_colPositionL - transform.GetPosition());

            contactPoint.m_t = t;
        }
    }
    return contactPoint;
}

ColliderComponent::RayCollision SphereCollider::CollideWithCollider(ColliderComponent & collider)
{
    return RayCollision();
}
