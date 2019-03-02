#include "Scene.h"

#include <Engine/Game/PBRendering/PBRRenderer.h>

using namespace BLAengine;

Scene::Scene()
{
    this->m_enableSimulation = true;
    this->m_rigidBodySystem = new RigidBodySystem(nullptr);
    this->m_camera = nullptr;
    this->m_renderingManager = nullptr;
    m_sceneObjectsVector.reserve(1000);
}

GameObjectReference Scene::CreateObject(std::string name)
{
    m_sceneObjectsVector.emplace_back(GameObject(name));

    return GameObjectReference { m_sceneObjectsVector.size() - (blaIndex)1, &m_sceneObjectsVector };
}

bool BLAengine::Scene::DeleteObject(std::string name)
{
	//TODO: Keep a bitfield of valid game objects in the vector (don't actually clear it)
    /*GameObjectReference object = this->FindObjectByName(name);
    if (!object)
        return false;

    for (auto meshRenderer : object->GetComponents<MeshRendererComponent>())
    {
        m_renderingManager->CancelMeshRendererTicket(meshRenderer);
    }*/

    return true;
}

GameObjectReference BLAengine::Scene::FindObjectByName(std::string name)
{
    for (size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        if (m_sceneObjectsVector[i].GetName().compare(name) == 0)
        {
            return GameObjectReference(i, &m_sceneObjectsVector);
        }
    }
    return GameObjectReference::InvalidReference();
}

vector<GameObjectReference> Scene::FindObjectsMatchingName(std::string name)
{
    std::vector<GameObjectReference> results;

    for (size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        if (m_sceneObjectsVector[i].GetName().find(name) != std::string::npos)
        {
            results.emplace_back(GameObjectReference(i, &m_sceneObjectsVector));
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
        GameObjectReference object(i, &m_sceneObjectsVector);

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

CameraComponent* Scene::GetMainCamera()
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

GameObjectReference Scene::PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact)
{
    float minDistance = MAX_NORMAL_FLOAT;
	GameObjectReference pickedObject;
	ColliderComponent::CollisionContact contactPoint;

    for (blaU32 i = 0; i < m_sceneObjectsVector.size(); ++i)
    {
		GameObject& obj = m_sceneObjectsVector[i];

        ColliderComponent* collider = obj.GetComponent<ColliderComponent>();

        if (collider == nullptr)
            continue;

        const blaBool collision = collider->CollideWithRay(inRay, contactPoint);

        if (!collision)
            continue;

        const float distance = glm::dot(contactPoint.m_colPositionW - inRay.m_origin, inRay.m_direction);

        if (distance > 0 && distance < minDistance)
        {
            minDistance = distance;
            pickedObject = GameObjectReference(i, &m_sceneObjectsVector);
            outContact = contactPoint;
        }
    }

	return pickedObject;
}