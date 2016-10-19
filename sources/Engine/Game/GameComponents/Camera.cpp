#include "Camera.h"
#include "../GameObject.h"
using namespace BLAengine;

Camera::Camera():
	m_fovSize(vec2(1,1)),
	GameComponent()
{
	UpdateView();

	m_isControlEnabled = false;
}

Camera::~Camera(void)
{

}


void Camera::UpdateView()
{
	if (!m_parentObject)
		return;

	m_viewTransform.m_position = -1.f * m_parentObject->GetTransform().m_position;
	m_viewTransform.m_rotation = 1.f *  m_parentObject->GetTransform().m_rotation;
	m_viewTransform.UpdateTransform();
	m_viewTransform.m_transformMatrix = inverse(m_viewTransform.m_transformMatrix);
}

void Camera::Update()
{
	// Regular updates
	UpdateView(); // View Transform update
}
