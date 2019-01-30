#include "Scene.h"

#include <Engine/Game/PBRendering/PBRRenderer.h>

using namespace BLAengine;

#pragma optimize("", off)

Scene::Scene()
{
    this->m_enableSimulation = true;
    this->m_rigidBodySystem = new RigidBodySystem(nullptr);
    this->m_camera = nullptr;
    this->m_renderingManager = nullptr;
    m_sceneObjectsVector.reserve(1000);
}

GameObject* Scene::CreateObject(std::string name)
{
    m_sceneObjectsVector.emplace_back(GameObject(name));

    return &(m_sceneObjectsVector[m_sceneObjectsVector.size() - 1]);
}

bool BLAengine::Scene::DeleteObject(std::string name)
{
    GameObject* object = this->FindObjectByName(name);
    if (!object)
        return false;

    for (auto meshRenderer : object->GetComponents<MeshRendererComponent>())
    {
        m_renderingManager->CancelMeshRendererTicket(meshRenderer);
    }

    return true;
}

GameObject* BLAengine::Scene::FindObjectByName(std::string name)
{
    for (size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        GameObject* object = &(m_sceneObjectsVector[i]);
        if (object->GetName().compare(name) == 0)
        {
            return object;
        }
    }
    return nullptr;
}

vector<GameObject*> Scene::FindObjectsMatchingName(std::string name)
{
    std::vector<GameObject*> results;

    for (size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        GameObject* object = &(m_sceneObjectsVector[i]);
        if (object->GetName().find(name) != std::string::npos)
        {
            results.push_back(object);
        }
    }
    return results;
}

void BLAengine::Scene::SetTimeObject(Timer * time)
{
    m_rigidBodySystem->SetTimeObject(time);
}


void BLAengine::Scene::Initialize(RenderingManager* renderingManager)
{
    this->m_renderingManager = renderingManager;
}

void Scene::Update()
{
    if (!m_renderingManager) 
    {
        std::cout << "Scene object not initialized !\n";
        return;
    }

    if (m_enableSimulation)
    {
        m_rigidBodySystem->EnableSimulation();
    }
    else
    {
        m_rigidBodySystem->DisableSimulation();
    }

    m_rigidBodySystem->UpdateSystem();
    //m_camera->m_rigidBody->Update();

    for(size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        GameObject* object = &(m_sceneObjectsVector[i]);

        object->Update();

        for (auto meshRenderer : object->GetComponents<MeshRendererComponent>())
        {
            if (meshRenderer->m_renderTicket == 0)
            {
                m_renderingManager->RegisterMeshRenderer(meshRenderer);
            }
        }

        for (auto dirLightComp : object->GetComponents<DirectionalLight>())
        {
            CameraComponent* shadowCamera = dirLightComp->GetParentObject()->GetComponent<CameraComponent>();
            if (dirLightComp->m_renderTicket == 0 && shadowCamera != nullptr)
            {
                m_renderingManager->RegisterDirectionalLight(dirLightComp, shadowCamera);
            }
        }

        for (auto pbrenderer : object->GetComponents<PBRCamera>())
        {
            if (!pbrenderer->m_shouldRender)
                continue;
            for (auto& object : m_sceneObjectsVector)
            {
                if (PBRSurfaceComponent* obj = object.GetComponent<PBRSurfaceComponent>())
                {
                    pbrenderer->AddObject(obj);
                }
            }
            pbrenderer->Render();
        }
    }
}

CameraComponent* BLAengine::Scene::GetMainCamera()
{
    if (m_camera == nullptr)
    {
        for (auto& object : m_sceneObjectsVector)
        {
            CameraComponent* camera = object.GetComponent<CameraComponent>();
            if (camera != nullptr && !camera->m_isShadowMapCamera)
            {
                m_camera = camera;
                break;
            }
        }
    }
    return m_camera;
}

vector<Contact>* Scene::GetContacts() const
{
    return &(this->m_rigidBodySystem->m_collisionProcessor->m_currentContacts);
}

std::pair<GameObject*, ColliderComponent::RayCollision>  Scene::PickGameObjectInScene(Ray ray)
{
    vector<GameObject>& objects = m_sceneObjectsVector;

    float minDistance = MAX_NORMAL_FLOAT;
    GameObject* pickedObject = nullptr;
    ColliderComponent::RayCollision closestContact;
    for (auto& obj : objects)
    {
        ColliderComponent* collider = obj.GetComponent<ColliderComponent>();

        if (collider == nullptr)
            continue;

        ColliderComponent::RayCollision contactPoint = collider->CollideWithRay(ray);

        if (!contactPoint.m_isValid)
            continue;

        const float distance = glm::dot(contactPoint.m_colPositionW - ray.m_origin, ray.m_direction);

        if (distance > 0 && distance < minDistance)
        {
            minDistance = distance;
            pickedObject = &obj;
            closestContact = contactPoint;
        }
    }

    return{ pickedObject, closestContact };
}