#pragma once

#include <Common/StdInclude.h>
#include <Common/BLASingleton.h>

//TODO: Create a ''system'' abstraction and the scene will enumarate and update all systems
//TODO: Systems should register themselves if they are compiled and only the non empty systems are updated
//TODO: This way I don't have to include non Core stuff in core ... (it's bad)
#include <Engine/Physics/RigidBodySystem.h>
#include <Engine/Core/CameraComponent.h>
#include <Engine/Core/RenderingManager.h>
#include <Engine/Physics/CollisionProcessing/CollisionProcessor.h>
#include <Engine/Physics/ColliderComponent.h>
#include <Common/Maths/Ray.h>
#include "GameObject.h"

class SceneSerializer;
namespace BLAengine
{
    class BLACORE_API Scene
    {
    public:
        friend class ::SceneSerializer;

        Scene();

        void Initialize(RenderingManager* renderingManager); // TODO: IMPLEMENT! Find camera object and so on
        void Update();

        GameObjectReference CreateObject(std::string name, const GameObjectReference &parentObject = GameObjectReference::InvalidReference());
        bool DeleteObject(std::string name);
		GameObjectReference FindObjectByName(std::string name);
        vector<GameObjectReference> FindObjectsMatchingName(std::string name);

        void AddComponent(GameObjectReference object, GameComponent* component);
        CameraComponent* GetMainCamera();
        vector<Contact>* GetContacts() const;

        bool m_enableSimulation;

        void EnableGravity() const { m_rigidBodySystem->m_enableGravity = true; }
        void DisableGravity() const { m_rigidBodySystem->m_enableGravity = false; }
        bool GetGravity() const { return m_rigidBodySystem->m_enableGravity; }
        RigidBodySystem* m_rigidBodySystem;

        void SetTimeObject(Timer* time) const;

        GameObjectReference PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact);

        GameObjectReference GetSceneRoot() { return { 0, &m_sceneObjectsVector }; }

        void SetGameObjectParent(const GameObjectReference& parent, const GameObjectReference& child);

    private:

        CameraComponent* m_camera; //TODO: Find in object scene !
        RenderingManager* m_renderingManager;
        vector<GameObject>  m_sceneObjectsVector;
    };

}