#include "CollisionProcessor.h"
using namespace BLAengine;

CollisionProcessor::CollisionProcessor(Time* time, float* timestepPtr) :
	m_flagInterpolateNormals(false),
	m_maxIterations(500),
	m_bounce(0.5),
	m_friction(0.1),
	m_epsilon(0.001),
	debug_stop(false),
	m_iterationCount(0),
	m_solveCount(0)
{
	m_bodiesList = vector<RigidBody*>();
	m_timeStepPointer = timestepPtr;
	m_time = time;

	m_processingTime = 0;
	m_detectionTime = 0;
}

CollisionProcessor::~CollisionProcessor()
{
	m_bodiesList.clear();
	m_currentContacts.clear();
}

void HelperShuffleArray(int* array,int size)
{
	srand(time(NULL));

	for (int i = size - 1; i > 0; i--)
	{
		int index = rand() % (i+1);
		int a = array[index];
		array[index] = array[i];
		array[i] = a;
	}
}

void CollisionProcessor::BroadPhaseDetection()
{
	for (int i = 0; i < m_bodiesList.size(); i++)
	{
		for (int j = i; j < m_bodiesList.size(); j++)
		{
			RigidBody* body1 = m_bodiesList[i];
			RigidBody* body2 = m_bodiesList[j];

			if (!(body1 == body2) && body1->m_enableCollision && body2->m_enableCollision)
			{
				vec3* nxtPos1 = &(body1->m_nextState->m_nextPos);
				vec3* nxtPos2 = &(body2->m_nextState->m_nextPos);
				if (length(*nxtPos1 - *nxtPos2) < (body1->m_collider->m_boundingRadius + body2->m_collider->m_boundingRadius))
					NarrowPhaseDetection(body1, body2);
			}
		}
	}
}

void CollisionProcessor::NarrowPhaseDetection(RigidBody* body1, RigidBody* body2)
{
	mat4 t1 = body1->GetObjectTransform().m_transformMatrix;
	body1->m_collider->m_collisionMesh->setTransform(&t1[0][0]);

	mat4 t2 = body2->GetObjectTransform().m_transformMatrix;
	body2->m_collider->m_collisionMesh->setTransform(&t2[0][0]);

	vector<pair<int, int>> collidingTriangles;
	vector<float> collisionPoints;
	vector<int> collidingFaces;
	vector<int> colSizes;
	bool collision = body1->m_collider->m_collisionMesh->collision(body2->m_collider->m_collisionMesh);

	if (collision)
	{
		body1->m_collider->m_collisionMesh->getCollisionPoints(&collisionPoints);
		body1->m_collider->m_collisionMesh->getCollidingTriangles(&collidingTriangles);
		body1->m_collider->m_collisionMesh->getPointsFromTri(&collidingFaces);

		for (int col = 0; col < collidingTriangles.size(); col++)
		{
			int triIndexBody1 = collidingTriangles.at(col).first;
			int triIndexBody2 = collidingTriangles.at(col).second;

			vec3 collisionPoint(collisionPoints[3 * col], collisionPoints[3 * col + 1], collisionPoints[3 * col + 2]);

			int collidingFace = collidingFaces[col];

			//Degenerate collision cases may be detected
			if (length(collisionPoint) < 0.01)
				continue;

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
				uint32_t vertIndexK1 = body1->m_collider->m_vertIndices->at(3 * triIndexBody1 + k);

				body1ContactVertices[k] = body1->m_collider->m_triVertices->at((int)vertIndexK1);
				if (body1->m_collider->m_triNormals->size() != 0) body1ContactNormals[k] = body1->m_collider->m_triNormals->at((int)vertIndexK1);

				uint32_t vertIndexK2 = body2->m_collider->m_vertIndices->at(3 * triIndexBody2 + k);

				body2ContactVertices[k] = body2->m_collider->m_triVertices->at((int)vertIndexK2);
				if (body2->m_collider->m_triNormals->size() != 0) body2ContactNormals[k] = body2->m_collider->m_triNormals->at((int)vertIndexK2);
			}

			vec3 contactInBody1 = body1->GetObjectTransform().WorldPositionToLocal(collisionPoint);
			vec3 contactInBody2 = body2->GetObjectTransform().WorldPositionToLocal(collisionPoint);

			vec3 tangent1Body1 = body1ContactVertices[1] - body1ContactVertices[0];
			vec3 tangent2Body1 = body1ContactVertices[2] - body1ContactVertices[0];

			vec3 tangent1Body2 = body2ContactVertices[1] - body2ContactVertices[0];
			vec3 tangent2Body2 = body2ContactVertices[2] - body2ContactVertices[0];

			body1ContactNormal = normalize(cross(tangent1Body1, tangent2Body1));
			body2ContactNormal = normalize(cross(tangent1Body2, tangent2Body2));

			body1ContactTangent = normalize(tangent1Body1);
			body2ContactTangent = normalize(tangent1Body2);

			// Normals should point outwards
			body1ContactNormal *= (dot(contactInBody1, body1ContactNormal) < 0 ? -1.f : 1.f);
			body2ContactNormal *= (dot(contactInBody2, body2ContactNormal) < 0 ? -1.f : 1.f);

			vec3 normalBody1W = body1->GetObjectTransform().LocalDirectionToWorld(body1ContactNormal);
			vec3 normalBody2W = body2->GetObjectTransform().LocalDirectionToWorld(body2ContactNormal);


			vec3 tangentBody1W = body1->GetObjectTransform().LocalDirectionToWorld(body1ContactTangent);
			vec3 tangentBody2W = body2->GetObjectTransform().LocalDirectionToWorld(body2ContactTangent);

			vec3 normal, tangent;

			if (collidingFace == 1)
			{
				normal = normalBody1W;

				vec3 body1to2 = body2->GetObjectTransform().m_position - collisionPoint;
				if (dot(normal, body1to2) < 0)
					normal = -normal;

				tangent = tangentBody1W;
			}
			else if (collidingFace == 2)
			{
				normal = normalBody2W;

				vec3 body2to1 = body1->GetObjectTransform().m_position - collisionPoint;
				if (dot(normal, body2to1) < 0)
					normal = -normal;

				tangent = tangentBody2W;
			}

			if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z))
				continue;

			Contact contact
			(
				body1, body2, 
				collisionPoint, 
				normal,
				tangent,
				collidingFace
			);

			contact.ComputeJacobian();

			m_currentContacts.push_back(contact);
		}
	}
}

void CollisionProcessor::SolveContacts()
{
	vector<vector<dvec3>> T(3 * m_currentContacts.size());
	ComputeT(T);

	vector<double> D(3 * m_currentContacts.size());
	GetDiagonalElements(T, D);

	vector<vec3> B;
	for (int i = 0; i<m_currentContacts.size(); i++)
	{
		Contact contact = m_currentContacts[i];

		RigidBody* body1 = contact.m_body1;
		dvec3 vLUpdate1 = (dvec3) body1->m_nextState->m_velocity;
		dvec3 vAUpdate1 = (dvec3) body1->GetObjectTransform().LocalDirectionToWorld(body1->m_nextState->m_angularVelocity);
		RigidBody* body2 = contact.m_body2;
		dvec3 vLUpdate2 = (dvec3) body2->m_nextState->m_velocity;
		dvec3 vAUpdate2 = (dvec3) body2->GetObjectTransform().LocalDirectionToWorld(body2->m_nextState->m_angularVelocity);

		vector<dvec3> nJac = contact.m_normalJacobian;
		vector<dvec3> t1Jac = contact.m_tangentJacobian1;
		vector<dvec3> t2Jac = contact.m_tangentJacobian2;

		float b_normal = dot(nJac[0], vLUpdate1) + dot(nJac[2], vLUpdate2) + dot(nJac[1], vAUpdate1) + dot(nJac[3], vAUpdate2);
		b_normal += m_bounce * (dot(nJac[0], vLUpdate1) + dot(nJac[2], vLUpdate2) + dot(nJac[1], vAUpdate1) + dot(nJac[3], vAUpdate2));

		float b_tangent1 = dot(t1Jac[0], vLUpdate1) + dot(t1Jac[1], vAUpdate1) + dot(t1Jac[2], vLUpdate2) + dot(t1Jac[3], vAUpdate2);

		float b_tangent2 = dot(t2Jac[0], vLUpdate1) + dot(t2Jac[1], vAUpdate1) + dot(t2Jac[2], vLUpdate2) + dot(t2Jac[3], vAUpdate2);

		B.push_back(vec3(b_normal/D[3*i + 0], b_tangent1 / D[3 * i + 1], b_tangent2 / D[3 * i + 2]));
	}

	// Gauss-Seidel Solver:
	int iteration = 0;

	vector<dvec3> lambdas(m_currentContacts.size());
	for (int i = 0; i < m_currentContacts.size(); i++)
	{
		lambdas[i] = dvec3(0);
	}

	// Randomize contact order for faster convergence
	vector<int> contactIndices(m_currentContacts.size());
	for (int i = 0; i < m_currentContacts.size(); i++)
	{
		contactIndices[i] = i;
	}
	HelperShuffleArray(contactIndices.data(), contactIndices.size());

	float averageDistance = 1;

	while (iteration < m_maxIterations && averageDistance > m_epsilon)
	{
		averageDistance = 0;
		for(auto index : contactIndices)
		{
			for (int i = 0; i < 3; i++)
			{
				Contact contact = m_currentContacts.at(index);
				RigidBody* body1 = contact.m_body1;
				RigidBody* body2 = contact.m_body2;

				vector<dvec3>* jacobianEntry;
				if (i == 0)
					jacobianEntry = &(contact.m_normalJacobian);
				else if (i == 1)
					jacobianEntry = &(contact.m_tangentJacobian1);
				else if (i == 2)
					jacobianEntry = &(contact.m_tangentJacobian2);

				double a = dot((*jacobianEntry)[0], body1->m_nextState->m_correctionLinearVelocity);
				double b = dot((*jacobianEntry)[2], body2->m_nextState->m_correctionLinearVelocity);
				double c = dot((*jacobianEntry)[1], body1->m_nextState->m_correctionAngularVelocity);
				double d = dot((*jacobianEntry)[3], body2->m_nextState->m_correctionAngularVelocity);

				double correction = a+b+c+d;

				double Bd = (B[index][i]);
				double Cd = (correction / D[3 * index + i]);
				double newLambda = lambdas[index][i] - Bd - Cd;

				if (i == 0) // Normal direction 
				{
					newLambda = newLambda > 0 ? newLambda : 0;
				}
				else if (i == 1 || i == 2)  // Tangential direction 1 
				{
					float a = newLambda < (-m_friction * newLambda) ? (-m_friction * newLambda) : newLambda;
					newLambda = a > m_friction * newLambda ? m_friction * newLambda : a;
				}

				double deltaLambda = newLambda - lambdas[index][i];

				body1->m_nextState->m_correctionLinearVelocity  += deltaLambda * T[3 * index + i][0];
				body1->m_nextState->m_correctionAngularVelocity += deltaLambda * T[3 * index + i][1];
				body2->m_nextState->m_correctionLinearVelocity  += deltaLambda * T[3 * index + i][2];
				body2->m_nextState->m_correctionAngularVelocity += deltaLambda * T[3 * index + i][3];

				lambdas[index][i] = newLambda;

				averageDistance += fabs(deltaLambda);
			}
		}
		if (isnan(averageDistance) || isnan(-averageDistance) || fabs(averageDistance) > 1000000)
		{
			cout << "Lambdas blew up: " << iteration << " iterations\n";
			cout << "Average Distance: " << averageDistance << "\n";
			debug_stop = true;
		}
		averageDistance /= 3 * lambdas.size();
		iteration++;
	}
	if (iteration == m_maxIterations)
	{
		cout << "LCP Solver did not converge after " << m_maxIterations << " iterations\n";
	}
	m_iterationCount = iteration;
	m_solveCount++;
}

void CollisionProcessor::ComputeT(vector<vector<dvec3>>& T)
{
	for (int i = 0; i < 3*m_currentContacts.size(); i++)
	{
		Contact contact = m_currentContacts[(int)(i/3)];

		RigidBody* body1 = contact.m_body1;
		RigidBody* body2 = contact.m_body2;

		vector<dvec3> Ti(4);

		vector<dvec3> jacobianEntry;
		if (i % 3 == 0)
			jacobianEntry = contact.m_normalJacobian;
		else if (i % 3 == 1)
			jacobianEntry = contact.m_tangentJacobian1;
		else if (i % 3 == 2)
			jacobianEntry = contact.m_tangentJacobian2;
		
		Ti[0] = body1->m_invMassTensor * jacobianEntry[0];
		Ti[1] = body1->m_invInertiaTensor * jacobianEntry[1];
		Ti[2] = body2->m_invMassTensor * jacobianEntry[2];
		Ti[3] = body2->m_invInertiaTensor * jacobianEntry[3];

		T[i] = Ti;
	}
}

void CollisionProcessor::GetDiagonalElements(vector<vector<dvec3>> T, vector<double>& D)
{
	for (int i = 0; i < 3 * m_currentContacts.size(); i++) 
	{
		Contact contact = m_currentContacts[(int)(i / 3)];

		RigidBody* body1 = contact.m_body1;
		RigidBody* body2 = contact.m_body2;

		vector<dvec3> jacobianEntry;
		if (i % 3 == 0)
			jacobianEntry = contact.m_normalJacobian;
		else if (i % 3 == 1)
			jacobianEntry = contact.m_tangentJacobian1;
		else if (i % 3 == 2)
			jacobianEntry = contact.m_tangentJacobian2;

		D[i] =  dot(jacobianEntry[0], T[i][0]) + 
				dot(jacobianEntry[1], T[i][1]) + 
				dot(jacobianEntry[2], T[i][2]) + 
				dot(jacobianEntry[3], T[i][3]);
	}
}

void CollisionProcessor::ProcessCollisions()
{
	m_currentContacts.clear();

	double startTime = m_time->GetTime();
	BroadPhaseDetection();
	m_detectionTime = m_time->GetTime() - startTime;

	startTime = m_time->GetTime();
	if(m_currentContacts.size() != 0)
		SolveContacts();
	m_processingTime = m_time->GetTime() - startTime;
}

Contact::Contact(RigidBody* body1, RigidBody* body2, vec3 colPoint, vec3 normalW, vec3 tangentW, int face):
	m_normalJacobian(vector<dvec3>(4)),
	m_tangentJacobian1(vector<dvec3>(4)),
	m_tangentJacobian2(vector<dvec3>(4)),
	m_faceFrom(face)
{
	m_body1 = body1;
	m_body2 = body2;
	m_contactPositionW = colPoint;

	m_contactNormalW = normalW;
	m_contactTangent1W = tangentW;

	m_contactTangent2W = cross(normalW, tangentW);
}
Contact::~Contact() {}

void Contact::ComputeJacobian()
{
	double sign = m_faceFrom == 1 ? 1 : -1;

	dvec3 radialBody1 = m_contactPositionW - (dvec3) m_body1->GetObjectTransform().m_position;
	dvec3 radialBody2 = m_contactPositionW - (dvec3) m_body2->GetObjectTransform().m_position;

	dvec3 crossNormal1 = cross(radialBody1, m_contactNormalW);
	dvec3 crossNormal2 = cross(radialBody2, m_contactNormalW);

	m_normalJacobian[0] = -sign * m_contactNormalW;
	m_normalJacobian[1] = -sign * crossNormal1;
	m_normalJacobian[2] = sign * m_contactNormalW;
	m_normalJacobian[3] = sign * crossNormal2;

	dvec3 crossTangent11 = cross(radialBody1, m_contactTangent1W);
	dvec3 crossTangent12 = cross(radialBody2, m_contactTangent1W);
	
	m_tangentJacobian1[0] = -sign * m_contactTangent1W;
	m_tangentJacobian1[1] = -sign * crossTangent11;
	m_tangentJacobian1[2] = sign * m_contactTangent1W;
	m_tangentJacobian1[3] = sign * crossTangent12;

	dvec3 crossTangent21 = cross(radialBody1, m_contactTangent2W);
	dvec3 crossTangent22 = cross(radialBody2, m_contactTangent2W);

	m_tangentJacobian2[0] = -sign * m_contactTangent2W;
	m_tangentJacobian2[1] = -sign * crossTangent21;
	m_tangentJacobian2[2] = sign * m_contactTangent2W;
	m_tangentJacobian2[3] = sign * crossTangent22;
}

SpatialCoherence::SpatialCoherence(float granularity)
{
}

SpatialCoherence::~SpatialCoherence()
{
}

void SpatialCoherence::CreateSpatialTable()
{
}
