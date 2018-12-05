#pragma once
#include "./RigidBodySystem.h"
#include "../../Common/StdInclude.h"
#include "./GameComponents/RigidBodyComponent.h"
#include "./GameComponents/CameraComponent.h"
#include "./RenderingManager.h"
#include "GameObject.h"

namespace BLAengine
{
    class BLACORE_API Scene
    {
    public:
        Scene();
        ~Scene(); // TODO: IMPLEMENT!

        void Initialize(RenderingManager* renderingManager); // TODO: IMPLEMENT! Find camera object and so on
        void Update();

        GameObject* CreateObject(std::string name);
        bool DeleteObject(std::string name);
        GameObject* FindNameInScene(std::string name);

        void AddComponent(GameObject* object, GameComponent* component);
        vector<GameObject*> GetObjects() { return m_sceneObjectsVector; }
        CameraComponent* GetMainCamera();
        vector<Contact>* GetContacts();

        bool m_enableSimulation;

        void EnableGravity() { m_rigidBodySystem->m_enableGravity = true; }
        void DisableGravity() { m_rigidBodySystem->m_enableGravity = false; }
        bool GetGravity() { return m_rigidBodySystem->m_enableGravity; }
        RigidBodySystem* m_rigidBodySystem;

        void SetTimeObject(Time* time);

        std::pair<GameObject*, ColliderComponent::RayCollision> PickGameObjectInScene(Ray ray);

    private:

        CameraComponent* m_camera; //TODO: Find in object scene !
        RenderingManager* m_renderingManager;
        vector<GameObject*>  m_sceneObjectsVector;
    };

}