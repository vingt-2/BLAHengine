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
	mat4 t1 = body1->m_transform->m_transformMatrix;
	body1->m_collider->m_collisionMesh->setTransform(&t1[0][0]);

	mat4 t2 = body2->m_transform->m_transformMatrix;
	body2->m_collider->m_collisionMesh->setTransform(&t2[0][0]);

	vector<pair<int, int>>* collidingTriangles = new vector<pair<int, int>>();
	vector<float> collisionPoints;
	bool collision = body1->m_collider->m_collisionMesh->collision(body2->m_collider->m_collisionMesh);

	if (collision)
	{
		body1->m_collider->m_collisionMesh->getCollisionPoints(&collisionPoints);
		body1->m_collider->m_collisionMesh->getCollidingTriangles(collidingTriangles);
		
		for (int col = 1; col < collidingTriangles->size(); col ++)
		{
			int triIndexBody1 = collidingTriangles->at(col).first;
			int triIndexBody2 = collidingTriangles->at(col).second;

			vec3 collisionPoint(collisionPoints[3 * col], collisionPoints[3 * col + 1], collisionPoints[3 * col + 2]);
			
			vec3 body1ContactNormal(0);
			vec3 body2ContactNormal(0);
			vec3 body1ContactTangent(0);
			vec3 body2ContactTangent(0);

			vec3 body1ContactNormals[3] = { vec3(0), vec3(0), vec3(0) };
			vec3 body1ContactVertices[3] = { vec3(0), vec3(0), vec3(0) };

			vec3 body2ContactNormals[3] = { vec3(0), vec3(0), vec3(0) };
			vec3 body2ContactVertices[3] = { vec3(0), vec3(0), vec3(0) };

			for (int k = 0; k < 3; k++)
			{
				uint32_t vertIndexK1 = body1->m_collider->m_vertIndices->at(3*triIndexBody1 + k);

				body1ContactVertices[k] = body1->m_collider->m_triVertices->at((int)vertIndexK1);
				body1ContactNormals[k] = body1->m_collider->m_triNormals->at((int)vertIndexK1);

				uint32_t vertIndexK2 = body2->m_collider->m_vertIndices->at(3*triIndexBody2 + k);

				body2ContactVertices[k] = body2->m_collider->m_triVertices->at((int)vertIndexK2);
				body2ContactNormals[k] = body2->m_collider->m_triNormals->at((int)vertIndexK2);
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
					sqrDistancesToBody1Vertices[i] = (body1ContactVertices[i].x - contactInBody1.x) +
						(body1ContactVertices[i].y - contactInBody1.y) +
						(body1ContactVertices[i].z - contactInBody1.z);

					totalSquareDistance1 += sqrDistancesToBody1Vertices[i];

					sqrDistancesToBody2Vertices[i] = (body2ContactVertices[i].x - contactInBody2.x) +
						(body2ContactVertices[i].y - contactInBody2.y) +
						(body2ContactVertices[i].z - contactInBody2.z);

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
				vec3 tangent1Body1 = body1ContactVertices[1] - body1ContactVertices[0];
				vec3 tangent2Body1 = body1ContactVertices[2] - body1ContactVertices[0];

				vec3 tangent1Body2 = body2ContactVertices[1] - body2ContactVertices[0];
				vec3 tangent2Body2 = body2ContactVertices[2] - body2ContactVertices[0];

				body1ContactNormal = normalize(cross(tangent1Body1, tangent2Body1));
				body2ContactNormal = normalize(cross(tangent1Body2, tangent2Body2));

				body1ContactTangent = normalize(tangent1Body1);
				body2ContactTangent = normalize(tangent1Body2);

			}

			m_collisionsPoints.push_back(collisionPoint);

			// Normals should point outwards
			body1ContactNormal *= (dot(contactInBody1, body1ContactNormal) < 0 ? -1.f : 1.f);
			body2ContactNormal *= (dot(contactInBody2, body2ContactNormal) < 0 ? -1.f : 1.f);

			vec3 normalBody1W = body1->m_transform->LocalDirectionToWorld(body1ContactNormal);
			vec3 normalBody2W = body2->m_transform->LocalDirectionToWorld(body2ContactNormal);

			vec3 tangentBody1W = body1->m_transform->LocalDirectionToWorld(body1ContactTangent);
			vec3 tangentBody2W = body2->m_transform->LocalDirectionToWorld(body2ContactTangent);

			Contact contact(body1, body2, 
								collisionPoint, 
								body1ContactNormal,
								body2ContactNormal,
								body1ContactTangent,
								body2ContactTangent);

			contact.ComputeJacobian();

			m_currentContacts.push_back(contact);
		}
	}
}

void CollisionProcessor::ProcessCollisions()
{
	m_currentContacts.clear();
	BroadPhaseDetection();


}

Contact::Contact(RigidBody* body1, RigidBody* body2, vec3 colPoint, vec3 normal1, vec3 normal2, vec3 tangent11, vec3 tangent21)
{
	m_body1 = body1;
	m_body2 = body2;
	m_contactPositionW = colPoint;

	vec3 tangent12 = cross(normal1, tangent11);
	vec3 tangent22 = cross(normal2, tangent21);

	m_contactFrame1L = mat3(normal1, tangent11, tangent12);
	m_contactFrame2L = mat3(normal1, tangent21, tangent22);

	m_contactNormalBody1W = m_body1->m_transform->LocalDirectionToWorld(normal1);
	m_contactNormalBody2W = m_body2->m_transform->LocalDirectionToWorld(normal2);
	m_contactTangent1Body1W = m_body1->m_transform->LocalDirectionToWorld(tangent11);
	m_contactTangent1Body2W = m_body2->m_transform->LocalDirectionToWorld(tangent21);
}
Contact::~Contact() {}

void Contact::ComputeJacobian()
{

	m_contactTangent2Body1W = m_body1->m_transform->LocalDirectionToWorld(m_contactFrame1L[2]);
	m_contactTangent2Body2W = m_body2->m_transform->LocalDirectionToWorld(m_contactFrame2L[2]);

	m_contactTangent2Body2W = cross(m_contactNormalBody2W, m_contactTangent1Body2W);

	vec3 radialBody1 = m_contactPositionW - m_body1->m_transform->m_position;
	vec3 radialBody2 = m_contactPositionW - m_body2->m_transform->m_position;

	vec3 crossNormal1 = cross(radialBody1, m_contactNormalBody1W);
	vec3 crossNormal2 = cross(radialBody2, m_contactNormalBody2W);

	float nJacobian[12] = { -m_contactNormalBody1W.x, -m_contactNormalBody1W.y, -m_contactNormalBody1W.z,
		crossNormal1.x, crossNormal1.y, crossNormal1.z,
		-m_contactNormalBody2W.x, -m_contactNormalBody2W.y, -m_contactNormalBody2W.z,
		crossNormal2.x, crossNormal2.y, crossNormal2.z, };
	memcpy(m_normalJacobian, nJacobian, 12 * sizeof(float));

	vec3 crossTangent11 = cross(radialBody1, m_contactTangent1Body1W);
	vec3 crossTangent12 = cross(radialBody2, m_contactTangent1Body2W);

	float tJacobian1[12] = { -m_contactTangent1Body1W.x, -m_contactTangent1Body1W.y, -m_contactTangent1Body1W.z,
		crossTangent11.x, crossTangent11.y, crossTangent11.z,
		-m_contactTangent1Body2W.x, -m_contactTangent1Body2W.y, -m_contactTangent1Body2W.z,
		crossTangent12.x, crossTangent12.y, crossTangent12.z, };
	memcpy(m_tangentJacobian1, tJacobian1, 12 * sizeof(float));

	vec3 crossTangent21 = cross(radialBody1, m_contactTangent2Body1W);
	vec3 crossTangent22 = cross(radialBody2, m_contactTangent2Body2W);

	float tJacobian2[12] = { -m_contactTangent2Body1W.x, -m_contactTangent2Body1W.y, -m_contactTangent2Body1W.z,
		crossTangent21.x, crossTangent21.y, crossTangent21.z,
		-m_contactTangent2Body2W.x, -m_contactTangent2Body2W.y, -m_contactTangent2Body2W.z,
		crossTangent22.x, crossTangent22.y, crossTangent22.z, };
	memcpy(m_tangentJacobian2, tJacobian2, 12 * sizeof(float));
}