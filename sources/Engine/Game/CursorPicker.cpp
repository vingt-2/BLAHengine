#include "CursorPicker.h"

CursorPicker::CursorPicker(const GameSingleton* singleton)
{
	this->m_gameSingleton = singleton;
}

Ray CursorPicker::ScreenToRay(float length)
{
	vec2 renderSize = m_gameSingleton->renderer->GetRenderSize();
	
	double x,y ;
	
	glfwGetCursorPos(m_gameSingleton->renderer->GetWindow(), &x, &y);
	
	
	vec3 rayDirection = vec3(1.f);
	rayDirection.x = ( ( ( 2.0f * (renderSize.x - x)) / renderSize.x ) -1 ) / m_gameSingleton->renderer->m_mainCamera->m_projection[0][0];
	
	rayDirection.y = - ( ( ( 2.0f * (renderSize.y - (y))) / renderSize.y ) -1 ) / m_gameSingleton->renderer->m_mainCamera->m_projection[1][1];

	mat4 inverseView = inverse(m_gameSingleton->renderer->m_mainCamera->m_viewTransform.transformMatrix);
	
	vec4 direction = (inverseView * vec4(rayDirection,0));
	rayDirection = vec3(direction.x,direction.y,direction.z);
	
	vec3 rayOrigin = vec3(inverseView[3][0], inverseView[3][1], inverseView[3][2]);
	
	return Ray(rayOrigin, -1.f * rayDirection, length);
}

void PrintVec3(vec3 print)
{
	cout << print.x << ", " << print.y << ", " << print.z << "\n";
}

GameChar* CursorPicker::PickGameCharInScene(Scene &pickedScene, Ray &ray, vec3 &hitInWorld)
{
	vector<GameObject*> objects = pickedScene.GetObjects();
	float minDistance = INFINITY;
	GameChar* pickedObject = NULL;
	for (auto obj : objects)
	{
		GameChar* gameChar = dynamic_cast<GameChar*>(obj);
		if (gameChar != NULL)
		{
			gameChar->m_rigidBody->m_collider->m_collisionMesh->setTransform(&(gameChar->m_transform->transformMatrix[0][0]));

			bool closestPoint = &hitInWorld != NULL;

			bool collision = gameChar->m_rigidBody->m_collider->m_collisionMesh->rayCollision(&(ray.m_origin.x), &(ray.m_direction.x), closestPoint,0,ray.m_length);
			
			if (!collision) continue;

			vec3 collisionPoint;
			gameChar->m_rigidBody->m_collider->m_collisionMesh->getCollisionPoint(&(collisionPoint.x),false);

			float distance = dot(collisionPoint - ray.m_origin, ray.m_direction);

			if (distance > 0 && distance < minDistance) 
			{
				minDistance = distance;
				pickedObject = gameChar;
			}
			if (&hitInWorld != NULL)
			{
				hitInWorld = collisionPoint;
			}
		}
	}
	
	return pickedObject;
}