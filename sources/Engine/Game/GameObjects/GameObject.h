#pragma once
#include "../GameComponents/MeshRenderer.h"
#include "../GameComponents/RigidBody.h"
#include "..\..\..\Common\StdInclude.h"

class BLACORE_API GameObject
{
public:
	Transform* m_transform;
	string m_objectName;

	virtual void Update() = 0;

	void UpdateTransform();


	GameObject(void);
	~GameObject(void);
};