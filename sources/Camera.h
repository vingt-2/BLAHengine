#pragma once
#include "GameObject.h"
class Camera : public GameObject
{
public:
	RigidBody* rigidBody;

	mat4 projection;
	Transform viewTransform;
	
	void SetProjection(mat4 projection);
	void UpdateView();

	void Update();

	vec3 UndoProjection(vec3 v)
	{
		vec4 vector(v,1);
		vec4 newV = inverse(projection) * vector;
		vec3 result(newV.x,newV.y,newV.z);
		return result;
	}

	Camera();
	~Camera(void);
};

