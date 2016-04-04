#include "CollisionProcessor.h"

CollisionProcessor::CollisionProcessor()
{
	m_bodiesList = vector<RigidBody*>();
}

CollisionProcessor::~CollisionProcessor()
{

}


void CollisionProcessor::BroadPhaseDetection()
{
	for (auto body1 : m_bodiesList)
	{
		for (auto body2 : m_bodiesList)
		{
			if (!(body1 == body2))
			{
				NarrowPhaseDetection(body1, body2);
			}
		}
	}
}

void CollisionProcessor::NarrowPhaseDetection(RigidBody* body1, RigidBody* body2)
{
	m_collisionsPoints = vector<vec3>();
	mat4 t1 = body1->m_transform->transformMatrix;
	body1->m_collider->m_collisionMesh->setTransform(&t1[0][0]);

	mat4 t2 = body2->m_transform->transformMatrix;
	body2->m_collider->m_collisionMesh->setTransform(&t2[0][0]);

	vector<pair<int, int>>* collidingTriangles = new vector<pair<int, int>>();
	vector<float> collisionPoints;
	bool collision = body1->m_collider->m_collisionMesh->collision(body2->m_collider->m_collisionMesh);

	if (collision)
	{
		body1->m_collider->m_collisionMesh->getCollisionPoints(&collisionPoints);
		
		for (int i = 0; i < collisionPoints.size(); i += 3)
		{
			m_collisionsPoints.push_back(vec3(collisionPoints[i], collisionPoints[i + 1], collisionPoints[i + 2]));
		}

		cout << "Collision: " << m_collisionsPoints.size() << " contact points.\n";
	}
}

void CollisionProcessor::ProcessCollisions()
{
	BroadPhaseDetection();
}