#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:

	mat4 projection;
	Transform viewTransform;
	
	void SetProjection(mat4 projection);
	void UpdateView();

	void Update();

	Camera();
	~Camera(void);
};

