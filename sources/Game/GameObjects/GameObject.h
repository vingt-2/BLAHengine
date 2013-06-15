#pragma once
#include "../GameComponents/MeshRenderer.h"
#include "../GameComponents/RigidBody.h"
class GameObject
{
public:
Transform* transform;
std::string objectName;

virtual void Update() =0;

void UpdateTransform();


GameObject(void);
~GameObject(void);
};