#include "PBRSurfaceComponent.h"
#include <random>

#include <Engine/Core/GameObject.h>

using namespace BLAengine;

std::random_device rd;
std::mt19937 gen(rd());

PBRSurfaceComponent::PBRSurfaceComponent(GameObjectReference parentObject):
    GameComponent(parentObject)
{
    this->m_associatedCollider = nullptr;
} 

PBRSurfaceComponent::~PBRSurfaceComponent(void)
{}


void PBRSurfaceComponent::Update()
{
    //TODO: Might want to figure that one out if I want to use the pbr rendere again...
   /* if(!m_parentObject->GetComponent<ColliderComponent>())
    {
        m_parentObject->AddComponent(m_associatedCollider);
    }*/
}

PBRMeshComponent::PBRMeshComponent(GameObjectReference parentObject):
    PBRSurfaceComponent(parentObject)
{
    m_surfaceArea = BLA_EPSILON;
}

void BLAengine::PBRMeshComponent::SampleSurface(blaVec3 &pos, float& prob)
{
    if (MeshColliderComponent* mesh = dynamic_cast<MeshColliderComponent*>(m_associatedCollider))
    {
        std::uniform_int_distribution<> randIndx(0, (mesh->m_vertPosIndices->size() / 3)-1);
        
        blaVec3 contactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

        int randomIndex = randIndx(gen);

        for (int k = 0; k < 3; k++)
        {
            blaU32 vertPosIndex = mesh->m_vertPosIndices->at(3 * randomIndex + k);
            contactVertices[k] = mesh->m_triVertices->at((int)vertPosIndex);
        }

        std::uniform_real_distribution<float> randDist(0.f, 1.f);
        float s = randDist(gen);
        blaVec3 sample = s*(contactVertices[1] - contactVertices[0]) + (1.f-s)*(contactVertices[2] - contactVertices[0]);

        pos = GetObjectTransform().LocalPositionToWorld(sample);
        prob = 1.0f / m_surfaceArea;
    }
    else
    {
        pos = blaVec3(0.f);
        prob = 0.f;
    }
}

void BLAengine::PBRMeshComponent::SampleSurfaceWithNormal(blaVec3 & position, blaVec3 & normal, float & prob)
{
    if (MeshColliderComponent* mesh = dynamic_cast<MeshColliderComponent*>(m_associatedCollider))
    {
        std::uniform_int_distribution<> randIndx(0, (mesh->m_vertPosIndices->size() / 3) - 1);

        blaVec3 contactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };
        blaVec3 contactNormals[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

        int randomIndex = randIndx(gen);

        for (int k = 0; k < 3; k++)
        {
            blaU32 vertPosIndex = mesh->m_vertPosIndices->at(3 * randomIndex + k);
            contactVertices[k] = mesh->m_triVertices->at((int)vertPosIndex);
            blaU32 vertNormalIndex = mesh->m_vertNormalIndices->at(3 * randomIndex + k);
            if (mesh->m_triNormals->size() != 0) contactNormals[k] = mesh->m_triNormals->at((int)vertNormalIndex);
        }

        std::uniform_real_distribution<float> randDist(0.f, 1.f);
        float s = randDist(gen);
        blaVec3 sample = s*(contactVertices[1] - contactVertices[0]) + (1 - s)*(contactVertices[2] - contactVertices[0]);

        position = GetObjectTransform().LocalPositionToWorld(sample);
        prob = 1.0f / m_surfaceArea;
        normal = 0.3333333f * contactNormals[0] + 0.3333333f * contactNormals[1] + 0.3333333f * contactNormals[2];
    }
    else
    {
        position = blaVec3(0);
        prob = 0;
    }
}

float BLAengine::PBRMeshComponent::GetSurfaceArea()
{
    return m_surfaceArea;
}

void PBRMeshComponent::CreateAndSetMeshCollider(TriangleMesh* mesh)
{
    MeshColliderComponent* collider = BLA_CREATE_COMPONENT(MeshColliderComponent, GetOwnerObject());

    collider->SetColliderMesh(mesh);
    
    m_associatedCollider = collider;

    ComputeSurfaceArea();
}

PBRMeshComponent::~PBRMeshComponent(void) {}

void PBRMeshComponent::ComputeSurfaceArea()
{
    if (MeshColliderComponent* meshColliderComponent = dynamic_cast<MeshColliderComponent*>(m_associatedCollider))
    {
        float totalArea = 0;
        for (size_t i = 0; i < meshColliderComponent->m_vertPosIndices->size() / 3; i++)
        {
            blaVec3 contactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

            for (int k = 0; k < 3; k++)
            {
                blaU32 vertPosIndex = meshColliderComponent->m_vertPosIndices->at(3 * i + k);
                contactVertices[k] = meshColliderComponent->m_triVertices->at((int)vertPosIndex);
            }
            float triArea = length(cross(contactVertices[1] - contactVertices[0], contactVertices[2] - contactVertices[0])) / 2.0f;
            totalArea += triArea;
        }
        m_surfaceArea = totalArea;
    }
}

void BLAengine::PBRSphereComponent::SampleSurface(blaVec3 &position, float &prob)
{
    std::uniform_real_distribution<float> randDist(-1.f, 1.f);

    float x = randDist(gen);
    float y = randDist(gen);
    float z = randDist(gen);

    blaVec3 pOnSphere(x, y, z);
    float radius = m_associatedCollider->GetBoundingRadius();
    position = GetObjectTransform().GetPosition() + pOnSphere * radius;
    prob = 1.0f / (2.0f * M_PI * radius * radius);
}

void BLAengine::PBRSphereComponent::SampleSurfaceWithNormal(blaVec3 &position, blaVec3& normal, float &prob)
{
    std::uniform_real_distribution<float> randDist(-1.f, 1.f);

    float x = randDist(gen);
    float y = randDist(gen);
    float z = randDist(gen);

    blaVec3 pOnSphere(x, y, z);
    float radius = m_associatedCollider->GetBoundingRadius();
    position = GetObjectTransform().GetPosition() + pOnSphere * radius;
    prob = 1.0f / (2.0f * M_PI * radius * radius);
    normal = pOnSphere;
}

float BLAengine::PBRSphereComponent::GetSurfaceArea()
{
    return m_surfaceArea;
}

void PBRSphereComponent::CreateAndSetSphereCollider(float radius)
{
    SphereColliderComponent* sphereColliderComponent = BLA_CREATE_COMPONENT(SphereColliderComponent, GetOwnerObject());
    sphereColliderComponent->SetSize(radius);

    m_surfaceArea = (4.0f * M_PI * M_PI) * radius * radius;
}

float BLAengine::PBRMaterial::LambertianBRDF::SampleBRDF(blaVec3& outDir, blaMat3& tangentSpace, blaVec3& inDir)
{
    auto randDist = std::uniform_real_distribution<float>(0, 1);
    // Sample the unit disk!
    float r = randDist(gen);
    float theta = 2 * M_PI * randDist(gen);

    float x = r * cosf(theta);
    float y = r * sinf(theta);

    // Project it on the unit sphere
    blaVec3 directionL = blaVec3(sqrt(1 - (x*x + y*y)), x, y); // positive z cap

    //Convert to World Direction
    outDir = tangentSpace * directionL;

    return 1 / M_PI;
}

float BLAengine::PBRMaterial::MirrorBRDF::SampleBRDF(blaVec3& outDir, blaMat3& tangentSpace, blaVec3 & inDir)
{
    blaVec3 normal = column(tangentSpace, 0);
    outDir = -2.0f * glm::dot(normal, inDir)*normal + inDir;
    return 1.0f;
}
