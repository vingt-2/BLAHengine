#include "CollisionProcessor.h"

CollisionProcessor::CollisionProcessor():
	m_flagInterpolateNormals(false)
{
	m_bodiesList = vector<RigidBody*>();
}

CollisionProcessor::~CollisionProcessor()
{
	m_bodiesList.clear();
	m_currentContacts.clear();
	m_collisionsPoints.clear();
}


void CollisionProcessor::BroadPhaseDetection()
{
	for (int i = 0; i < m_bodiesList.size(); i++)
	{
		for (int j = i; j < m_bodiesList.size(); j++)
		{
			RigidBody* body1 = m_bodiesList[i];
			RigidBody* body2 = m_bodiesList[j];

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
		body1->m_collider->m_collisionMesh->getCollidingTriangles(collidingTriangles);
		
		for (int col = 0; col < collisionPoints.size(); col += 3)
		{
			int triIndexBody1 = collidingTriangles->at(col/3).first;
			int triIndexBody2 = collidingTriangles->at(col/3).second;

			vec3 collisionPoint(collisionPoints[col], collisionPoints[col + 1], collisionPoints[col + 2]);

			m_collisionsPoints.push_back(collisionPoint);
			
			vec3 body1ContactNormal(0);
			vec3 body2ContactNormal(0);

			vec3 body1ContactNormals[3] = { vec3(0), vec3(0), vec3(0) };
			vec3 body1ContactVertices[3] = { vec3(0), vec3(0), vec3(0) };

			vec3 body2ContactNormals[3] = { vec3(0), vec3(0), vec3(0) };
			vec3 body2ContactVertices[3] = { vec3(0), vec3(0), vec3(0) };

			for (int k = 0; k < 3; k++)
			{
				if (triIndexBody1 + 2 < body1->m_collider->m_triVertices->size())
				{
					body1ContactVertices[k] = body1->m_collider->m_triVertices->at(triIndexBody1 + k);
					body1ContactNormals[k] = body1->m_collider->m_triNormals->at(triIndexBody1 + k);
				}
				if (triIndexBody2 + 2 < body2->m_collider->m_triVertices->size())
				{
					body2ContactVertices[k] = body2->m_collider->m_triVertices->at(triIndexBody2 + k);
					body2ContactNormals[k] = body2->m_collider->m_triNormals->at(triIndexBody2 + k);
				}
			}

			vec3 contactInBody1 = body1->m_transform->WorldlPositionToLocal(collisionPoint);
			vec3 contactInBody2 = body2->m_transform->WorldlPositionToLocal(collisionPoint);

			if (m_flagInterpolateNormals)
			{

				/*
					Assuming that the reported contact location is "Close enough to the triangle",
					approximate interpolation parameters can be obtained by comparing distance
					to vertices without projecting on the triangle plane
					*/

				float sqrDistancesToBody1Vertices[3];
				float sqrDistancesToBody2Vertices[3];
				float totalSquareDistance1 = 0;
				float totalSquareDistance2 = 0;


				for (int i = 0; i < 3; i++)
				{
					sqrDistancesToBody1Vertices[i] = (body1ContactNormals[i].x - contactInBody1.x) +
						(body1ContactNormals[i].y - contactInBody1.y) +
						(body1ContactNormals[i].z - contactInBody1.z);

					totalSquareDistance1 += sqrDistancesToBody1Vertices[i];

					sqrDistancesToBody2Vertices[i] = (body2ContactNormals[i].x - contactInBody2.x) +
						(body2ContactNormals[i].y - contactInBody2.y) +
						(body2ContactNormals[i].z - contactInBody2.z);

					totalSquareDistance2 += sqrDistancesToBody2Vertices[i];
				}


				body1ContactNormal = (sqrDistancesToBody1Vertices[0] / totalSquareDistance1) * body1ContactNormals[0] +
					(sqrDistancesToBody1Vertices[1] / totalSquareDistance1) * body1ContactNormals[1] +
					(sqrDistancesToBody1Vertices[2] / totalSquareDistance1) * body1ContactNormals[2];

				body2ContactNormal = (sqrDistancesToBody2Vertices[0] / totalSquareDistance1) * body2ContactNormals[0] +
					(sqrDistancesToBody2Vertices[1] / totalSquareDistance1) * body2ContactNormals[1] +
					(sqrDistancesToBody2Vertices[2] / totalSquareDistance1) * body2ContactNormals[2];
			}
			else
			{
				body1ContactNormal = body1ContactNormals[0] + body1ContactNormals[1] + body1ContactNormals[2];
				body2ContactNormal = body2ContactNormals[0] + body2ContactNormals[1] + body2ContactNormals[2];
			}

			// Normals should point inwards
			body1ContactNormal *= (dot(contactInBody1, body1ContactNormal) > 0 ? -1.f : 1.f);
			body2ContactNormal *= (dot(contactInBody2, body1ContactNormal) > 0 ? -1.f : 1.f);

			vec3 normalBody1W = body1->m_transform->LocalDirectionToWorld(body1ContactNormal);
			vec3 normalBody2W = body2->m_transform->LocalDirectionToWorld(body2ContactNormal);

			Contact contact(body1, body2, collisionPoint, normalBody1W, normalBody2W);

			m_currentContacts.push_back(contact);
		}
	}
}

void CollisionProcessor::ProcessCollisions()
{
	m_currentContacts.clear();
	BroadPhaseDetection();
}

Contact::Contact(RigidBody* body1, RigidBody* body2, vec3 colPoint, vec3 normal1W, vec3 normal2W)
{
	m_body1 = body1;
	m_body1 = body1;
	m_contactPositionW = colPoint;
	m_contactNormalBody1W = normal1W;
	m_contactNormalBody2W = normal2W;
}
Contact::~Contact() {}

void Contact::ComputeJacobian()
{

}