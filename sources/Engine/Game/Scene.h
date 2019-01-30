#pragma once

#include <Common/StdInclude.h>

#include <Engine/Game/RigidBodySystem.h>
#include <Engine/Game/GameComponents/RigidBodyComponent.h>
#include <Engine/Game/GameComponents/CameraComponent.h>
#include <Engine/Game/RenderingManager.h>

#include "GameObject.h"

namespace BLAengine
{
    class BLACORE_API Scene
    {
    public:
        Scene();

        void Initialize(RenderingManager* renderingManager); // TODO: IMPLEMENT! Find camera object and so on
        void Update();

        GameObject* CreateObject(std::string name);
        bool DeleteObject(std::string name);
        GameObject* FindObjectByName(std::string name);
        vector<GameObject*> FindObjectsMatchingName(std::string name);

        void AddComponent(GameObject* object, GameComponent* component);
        const vector<GameObject>& GetObjects() const { return m_sceneObjectsVector; }
        vector<GameObject>& GetObjects() { return m_sceneObjectsVector; }
        CameraComponent* GetMainCamera();
        vector<Contact>* GetContacts() const;

        bool m_enableSimulation;

        void EnableGravity() const { m_rigidBodySystem->m_enableGravity = true; }
        void DisableGravity() const { m_rigidBodySystem->m_enableGravity = false; }
        bool GetGravity() const { return m_rigidBodySystem->m_enableGravity; }
        RigidBodySystem* m_rigidBodySystem;

        void SetTimeObject(Timer* time);

        std::pair<GameObject*, ColliderComponent::RayCollision> PickGameObjectInScene(Ray ray);

    private:

        CameraComponent* m_camera; //TODO: Find in object scene !
        RenderingManager* m_renderingManager;
        vector<GameObject>  m_sceneObjectsVector;
    };

}