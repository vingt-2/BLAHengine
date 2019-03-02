#pragma once

#include <Common/StdInclude.h>

#include <Engine/Game/RigidBodySystem.h>
#include <Engine/Game/GameComponents/RigidBodyComponent.h>
#include <Engine/Game/GameComponents/CameraComponent.h>
#include <Engine/Game/RenderingManager.h>
#include <Engine/Game/CollisionProcessing/CollisionProcessor.h>
#include <Engine/Game/GameComponents/ColliderComponent.h>
#include <Engine/Game/GameAlgebra/Ray.h>
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

        GameObjectReference CreateObject(std::string name);
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

        void SetTimeObject(Timer* time);

        GameObjectReference PickGameObjectInScene(const Ray& inRay, ColliderComponent::CollisionContact& outContact);

    private:

        CameraComponent* m_camera; //TODO: Find in object scene !
        RenderingManager* m_renderingManager;
        vector<GameObject>  m_sceneObjectsVector;
    };

}