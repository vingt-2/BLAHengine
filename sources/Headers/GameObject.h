#pragma once
#include "MeshRenderer.h"
#include "RigidBody.h"
class GameObject
{
public:
Transform* transform;
std::string objectName;

void Update();


GameObject(void);
~GameObject(void);
};