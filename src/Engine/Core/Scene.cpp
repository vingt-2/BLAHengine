#include "Scene.h"

//Todo: fix me, remove indivudual component logic ...
#include <Demos/PBRendering/PBRRenderer.h>
#include <Engine/Physics/RigidBodyComponent.h>

using namespace BLAengine;

Scene::Scene()
{
    this->m_enableSimulation = true;
    this->m_rigidBodySystem = new RigidBodySystem(nullptr);
    this->m_camera = nullptr;
    this->m_renderingManager = nullptr;
    m_sceneObjectsVector.reserve(1000);
}

GameObjectReference Scene::CreateObject(blaString name, const GameObjectReference &parentObject)
{
    m_sceneObjectsVector.emplace_back(GameObject(name, GameObjectReference::InvalidReference()));

	m_sceneFlags |= ESceneFlags::DIRTY_SCENE_STRUCTURE;

    return GameObjectReference { m_sceneObjectsVector.size() - (blaIndex)1, &m_sceneObjectsVector };
}

bool Scene::DeleteObject(blaString name)
{
	//TODO: Keep a bitfield of valid game objects in the vector (don't actually clear it)
    /*GameObjectReference object = this->FindObjectByName(name);
    if (!object)
        return false;

    for (auto meshRenderer : object->ListComponentNames<MeshRendererComponent>())
    {
        m_renderingManager->CancelMeshRendererTicket(meshRenderer);
    }*/

    return true;
}

GameObjectReference Scene::FindObjectByName(blaString name)
{
    for (size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        if (m_sceneObjectsVector[i].GetName().compare(name) == 0)
        {
            return { i, &m_sceneObjectsVector };
        }
    }
    return GameObjectReference::InvalidReference();
}

blaVector<GameObjectReference> Scene::FindObjectsMatchingName(blaString name)
{
    blaVector<GameObjectReference> results;

    for (size_t i = 0; i < m_sceneObjectsVector.size(); i++)
    {
        if (m_sceneObjectsVector[i].GetName().find(name) != blaString::npos)
        {
            results.emplace_back(GameObjectReference(i, &m_sceneObjectsVector));
        }
    }
    return results;
}

void Scene::SetTimeObject(Timer * time) const
{
    m_rigidBodySystem->SetTimeObject(time);
}


void Scene::Initialize(RenderingManager* renderingManager)
{
    this->m_renderingManager = renderingManager;
}

void Scene::Update()
{
	m_sceneFlags = 0;
    if (!m_renderingManager) 
    {
        return;
    }

    if (m_enableSimulation)
    {
        m_rigidBodySystem->EnableSimulation();
		m_rigidBodySystem->m_enableGravity = true;
    }
    else
    {
        m_rigidBodySystem->DisableSimulation();
    }

    m_rigidBodySystem->UpdateSystem();

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

        for (auto dirLightComp : object->GetComponents<DirectionalLightComponent>())
        {
            CameraComponent* shadowCamera = dirLightComp->GetOwnerObject()->GetComponent<CameraComponent>();
            if (dirLightComp->m_renderTicket == 0 && shadowCamera != nullptr)
            {
                m_renderingManager->RegisterDirectionalLight(dirLightComp, shadowCamera);
            }
        }

		for (auto rigidBody: object->GetComponents<RigidBodyComponent>())
		{
			//Use ticket system for rigidbody as well
			if (rigidBody && !rigidBody->m_registered)
			{
				m_rigidBodySystem->RegisterRigidBody(*rigidBody);
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

blaVector<Contact>* Scene::GetContacts() const
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

void Scene::SetGameObjectParent(const GameObjectReference& parent, const GameObjectReference& child)
{
    if(parent.IsValid() && child.IsValid())
    {
        child->m_parent = parent;
    }
}

blaVector<GameObjectReference> Scene::GetObjects()
{
	blaVector<GameObjectReference> ret;
	for(int i = 0; i < m_sceneObjectsVector.size(); ++i)
	{
		ret.push_back(GameObjectReference(i, &m_sceneObjectsVector));
	}

	return ret;
}

