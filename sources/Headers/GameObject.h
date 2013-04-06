#pragma once
#include "MeshRenderer.h"
#include "RigidBody.h"
class GameObject
{
public:
Transform* transform;

void Update();


GameObject(void);
~GameObject(void);
};