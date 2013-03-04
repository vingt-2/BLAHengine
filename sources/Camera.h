#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:

	mat4 projection;
	mat4 view;
	
	void SetProjection(mat4 projection);
	void SetView(mat4 view);

	void UpdateView();

	Camera();
	~Camera(void);
};

