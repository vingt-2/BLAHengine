#include "../Game/GameObjects/Camera.h"

class RenderCamera
{
public:
	// Hold the View Projection matrix (updated each frame)
	mat4 m_ViewProjection;

	virtual void AttachCamera(Camera* camera);
	virtual void Update() = 0;

	RenderCamera();

	// Points to the camera object
	Camera* m_attachedCamera;

};

class PerspectiveCamera : public RenderCamera
{
public:
	mat4 m_perspectiveProjection;

	void SetPerspective(vec2 renderSize);
	void Update();
};

class OrthographicCamera: public RenderCamera
{
public:
	mat4 m_orthographicProjection;

	void SetOrthography();
	void Update();
};