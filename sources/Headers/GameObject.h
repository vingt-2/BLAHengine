#pragma once
#include "MeshRenderer.h"
#include "RigidBody.h"
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