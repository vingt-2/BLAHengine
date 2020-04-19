#include "Scene.h"

#include "Core/TransformComponent.h"
#include "Core/ComponentSystems.h"

#pragma optimize("", off)

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(Scene)

Scene::Scene() : m_sceneTimer(Timer(1))
{
    this->m_camera = nullptr;
    this->m_renderingManager = nullptr;
}

GameObject Scene::CreateObject(GameObjectID objectId, const GameObject& parentObject)
{
    m_sceneFlags |= ESceneFlags::DIRTY_SCENE_STRUCTURE;
    GameObject ref = GameObject(objectId);
    m_validObjects.push_back(ref);

    AddComponent(ref, BlaStringId("TransformComponent"));

    return ref;
}

GameObject Scene::FindObjectByName(blaString name)
{
    for(auto o : m_validObjects) 
    {
        if(blaString(o) == name) 
        {
            return o;
        }
    }
    return GameObject();
}

GameComponent* Scene::AddComponent(GameObject object, GameComponentID componentId)
{
    // TODO: Obviously make this a bit better
    auto oit = std::find(m_validObjects.begin(), m_validObjects.end(), object);
    if(oit == m_validObjects.end()) 
    {
        CreateObject(object);
    }

    GameComponent* gc = m_components.AddComponent(object, componentId);

    BLA_ASSERT(gc);

    if(gc)
        m_toInitialize.push_back(gc);

    return gc;
}

void Scene::UpdateSceneTimer(blaF32 time)
{
    m_sceneTimer.Update(time);
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

    for(GameComponent* gc : m_toInitialize) 
    {
        gc->Init();
    }
    m_toInitialize.clear();

    const ComponentSystemsRegistry* r = ComponentSystemsRegistry::GetSingletonInstanceRead();

    for(auto cs : r->m_componentSystems)
    {
        const ComponentSystem* system = cs.second;

        //// Validate component availability for system:
        //blaVector<blaStringId> iSystem = system->GetInputComponents();
        //blaVector<blaStringId> oSystem = system->GetOutputComponents();

        //bool validCall = true;

        //for(blaStringId c : iSystem) 
        //{
        //    if()
        //}

        system->ExecuteSystem(m_validObjects, m_components.GetComponentSystemIOInterface());
    }
}

void Scene::Clear()
{
    m_components.Clear();

    m_validObjects.clear();

    m_camera = nullptr;

    m_sceneFlags |= DIRTY_SCENE_STRUCTURE;
}

CameraComponent* Scene::GetMainCamera()
{
    if (m_camera == nullptr)
    {
        for(GameObject o : m_validObjects) 
        {
            if(CameraComponent* camera = GetComponentPerObject<CameraComponent>(o)) 
            {
                if (!camera->m_isShadowMapCamera)
                {
                    m_camera = camera;
                    break;
                }
            }
        }
    }
    BLA_ASSERT(m_camera);
    return m_camera;
}

//GameObject Scene::PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact)
//{
//    float minDistance = MAX_NORMAL_FLOAT;
//    GameObject pickedObject;
//    ColliderComponent::CollisionContact contactPoint;
//
//    for (blaU32 i = 0; i < m_sceneObjectsVector.size(); ++i)
//    {
//        GameObject& obj = m_sceneObjectsVector[i]     ;
//
//        ColliderComponent* collider = obj.GetComponent<ColliderComponent>();
//
//        if (collider == nullptr)
//            continue;
//
//        const blaBool collision = collider->CollideWithRay(inRay, contactPoint);
//
//        if (!collision)
//            continue;
//
//        const float distance = glm::dot(contactPoint.m_colPositionW - inRay.m_origin, inRay.m_direction);
//
//        if (distance > 0 && distance < minDistance)
//        {
//            minDistance = distance;
//            pickedObject = GameObject(i, &m_sceneObjectsVector);
//            outContact = contactPoint;
//        }
//    }
//
//    return GameObject();
//}

void Scene::SetGameObjectParent(const GameObject& parent, const GameObject& child)
{
    m_gameObjectHierarchy[parent] = child;
}

GameObject Scene::GetGameObjectParent(const GameObject& object)
{
    GameObjectHierarchyMap::iterator it = m_gameObjectHierarchy.find(object);
    if(it != m_gameObjectHierarchy.end()) 
    {
        return it->second;
    }
    return GameObject();
}

blaVector<GameObject> Scene::GetObjects()
{
    blaVector<GameObject> ret;
    for (int i = 0; i < m_validObjects.size(); ++i)
    {
        ret.push_back(GameObject(m_validObjects[i]));
    }

    return ret;
}

const blaVector<GameObjectID>& Scene::GetObjectsID() const
{
    return m_validObjects;
}

GameComponent* Scene::GetComponentPerObject(GameComponentID componentId, GameObject obj)
{
    return m_components.GetComponentPerObject(componentId, obj);
}

blaVector<GameComponent*> Scene::GetComponentsPerObject(GameObject obj)
{
    return m_components.GetComponentsPerObject(obj);
}
