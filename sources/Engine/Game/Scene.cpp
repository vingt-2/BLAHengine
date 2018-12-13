#include "Scene.h"
#include "PBRendering\PBRRenderer.h"

using namespace BLAengine;


Scene::Scene()
{
    this->m_enableSimulation = true;
    this->m_rigidBodySystem = new RigidBodySystem(nullptr);
    this->m_sceneObjectsVector = vector<GameObject*>();
    this->m_camera = nullptr;
    this->m_renderingManager = nullptr;
}

Scene::~Scene()
{
    // TODO: DO SOME STUFF. YO
}

GameObject* Scene::CreateObject(std::string name)
{
    GameObject* newObject = new GameObject(name);

    m_sceneObjectsVector.push_back(newObject);

    return newObject;
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
        GameObject* object = m_sceneObjectsVector[i];
        if (object->GetName().compare(name) == 0)
        {
            return object;
        }
    }
    return nullptr;
}

void BLAengine::Scene::SetTimeObject(Time * time)
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
        GameObject* object = m_sceneObjectsVector[i];

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
            CameraComponent* shadowCamera = dirLightComp->m_parentObject->GetComponent<CameraComponent>();
            if (dirLightComp->m_renderTicket == 0 && shadowCamera != nullptr)
            {
                m_renderingManager->RegisterDirectionalLight(dirLightComp, shadowCamera);
            }
        }

        for (auto pbrenderer : object->GetComponents<PBRCamera>())
        {
            if (!pbrenderer->m_shouldRender)
                continue;
            for (auto object : m_sceneObjectsVector)
            {
                if (PBRSurface* obj = object->GetComponent<PBRSurface>())
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
        for (auto object : m_sceneObjectsVector)
        {
            CameraComponent* camera = object->GetComponent<CameraComponent>();
            if (camera != nullptr && !camera->m_isShadowMapCamera)
            {
                m_camera = camera;
                break;
            }
        }
    }
    return m_camera;
}

vector<Contact>* Scene::GetContacts()
{
    return &(this->m_rigidBodySystem->m_collisionProcessor->m_currentContacts);
}

std::pair<GameObject*, ColliderComponent::RayCollision>  Scene::PickGameObjectInScene(Ray ray)
{
    vector<GameObject*> objects = m_sceneObjectsVector;

    float minDistance = MAX_NORMAL_FLOAT;
    GameObject* pickedObject = nullptr;
    ColliderComponent::RayCollision closestContact;
    for (auto obj : objects)
    {
        ColliderComponent* collider = obj->GetComponent<ColliderComponent>();

        if (collider == nullptr)
            continue;

        ColliderComponent::RayCollision contactPoint = collider->CollideWithRay(ray);

        if (!contactPoint.m_isValid)
            continue;

        float distance = glm::dot(contactPoint.m_colPositionW - ray.m_origin, ray.m_direction);

        if (distance > 0 && distance < minDistance)
        {
            minDistance = distance;
            pickedObject = obj;
            closestContact = contactPoint;
        }
    }

    return std::pair<GameObject*, ColliderComponent::RayCollision>(pickedObject, closestContact);
}