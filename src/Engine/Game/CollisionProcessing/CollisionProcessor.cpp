#include <Engine/Game/GameComponents/RigidBodyComponent.h>
#include <Engine/Game/GameComponents/ColliderComponent.h>
#include "CollisionProcessor.h"
#include "Engine/System/Console.h"

using namespace BLAengine;

CollisionProcessor::CollisionProcessor(Timer* time, float* timestepPtr) :
    m_flagInterpolateNormals(false),
    m_maxIterations(500),
    m_bounce(0.5f),
    m_friction(0.1f),
    m_epsilon(0.01f),
    debug_stop(false),
    m_iterationCount(0),
    m_solveCount(0)
{
    m_bodiesList = vector<RigidBodyComponent*>();
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
    srand((int) time(NULL));

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
    for (size_t i = 0; i < m_bodiesList.size(); i++)
    {
        for (size_t j = i; j < m_bodiesList.size(); j++)
        {
            RigidBodyComponent* body1 = m_bodiesList[i];
            RigidBodyComponent* body2 = m_bodiesList[j];

            if (!(body1 == body2) && body1->m_enableCollision && body2->m_enableCollision)
            {
                blaVec3* nxtPos1 = &(body1->m_nextState.m_nextPos);
                blaVec3* nxtPos2 = &(body2->m_nextState.m_nextPos);
                if (length(*nxtPos1 - *nxtPos2) < (body1->GetAssociatedCollider()->GetBoundingRadius() + body2->GetAssociatedCollider()->GetBoundingRadius()))
                    NarrowPhaseDetection(body1, body2);
            }
        }
    }
}

void CollisionProcessor::NarrowPhaseDetection(RigidBodyComponent* body1, RigidBodyComponent* body2)
{
    //blaMat4 t1 = body1->GetObjectTransform().m_posQuat;
    //body1->m_collider->m_collisionMesh->setTransform(&t1[0][0]);

    //blaMat4 t2 = body2->GetObjectTransform().m_posQuat;
    //body2->m_collider->m_collisionMesh->setTransform(&t2[0][0]);

    //vector<pair<int, int>> collidingTriangles;
    //vector<float> collisionPoints;
    //vector<int> collidingFaces;
    //vector<int> colSizes;
    //bool collision = body1->m_collider->m_collisionMesh->collision(body2->m_collider->m_collisionMesh);

    //if (collision)
    //{
    //    body1->m_collider->m_collisionMesh->getCollisionPoints(&collisionPoints);
    //    body1->m_collider->m_collisionMesh->getCollidingTriangles(&collidingTriangles);
    //    body1->m_collider->m_collisionMesh->getPointsFromTri(&collidingFaces);

    //    for (int col = 0; col < collidingTriangles.size(); col++)
    //    {
    //        int triIndexBody1 = collidingTriangles.at(col).first;
    //        int triIndexBody2 = collidingTriangles.at(col).second;

    //        blaVec3 collisionPoint(collisionPoints[3 * col], collisionPoints[3 * col + 1], collisionPoints[3 * col + 2]);

    //        int collidingFace = collidingFaces[col];

    //        //Degenerate collision cases may be detected
    //        if (length(collisionPoint) < 0.0f1)
    //            continue;

    //        blaVec3 body1ContactNormal(0);
    //        blaVec3 body2ContactNormal(0);
    //        blaVec3 body1ContactTangent(0);
    //        blaVec3 body2ContactTangent(0);

    //        blaVec3 body1ContactNormals[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };
    //        blaVec3 body1ContactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

    //        blaVec3 body2ContactNormals[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };
    //        blaVec3 body2ContactVertices[3] = { blaVec3(0), blaVec3(0), blaVec3(0) };

    //        for (int k = 0; k < 3; k++)
    //        {
    //            blaU32 vertIndexK1 = body1->m_collider->m_vertIndices->at(3 * triIndexBody1 + k);

    //            body1ContactVertices[k] = body1->m_collider->m_triVertices->at((int)vertIndexK1);
    //            if (body1->m_collider->m_triNormals->size() != 0) body1ContactNormals[k] = body1->m_collider->m_triNormals->at((int)vertIndexK1);

    //            blaU32 vertIndexK2 = body2->m_collider->m_vertIndices->at(3 * triIndexBody2 + k);

    //            body2ContactVertices[k] = body2->m_collider->m_triVertices->at((int)vertIndexK2);
    //            if (body2->m_collider->m_triNormals->size() != 0) body2ContactNormals[k] = body2->m_collider->m_triNormals->at((int)vertIndexK2);
    //        }

    //        blaVec3 contactInBody1 = body1->GetObjectTransform().WorldPositionToLocal(collisionPoint);
    //        blaVec3 contactInBody2 = body2->GetObjectTransform().WorldPositionToLocal(collisionPoint);

    //        blaVec3 tangent1Body1 = body1ContactVertices[1] - body1ContactVertices[0];
    //        blaVec3 tangent2Body1 = body1ContactVertices[2] - body1ContactVertices[0];

    //        blaVec3 tangent1Body2 = body2ContactVertices[1] - body2ContactVertices[0];
    //        blaVec3 tangent2Body2 = body2ContactVertices[2] - body2ContactVertices[0];

    //        body1ContactNormal = normalize(cross(tangent1Body1, tangent2Body1));
    //        body2ContactNormal = normalize(cross(tangent1Body2, tangent2Body2));

    //        body1ContactTangent = normalize(tangent1Body1);
    //        body2ContactTangent = normalize(tangent1Body2);

    //        // Normals should point outwards
    //        body1ContactNormal *= (glm::dot(contactInBody1, body1ContactNormal) < 0 ? -1.f : 1.f);
    //        body2ContactNormal *= (glm::dot(contactInBody2, body2ContactNormal) < 0 ? -1.f : 1.f);

    //        blaVec3 normalBody1W = body1->GetObjectTransform().LocalDirectionToWorld(body1ContactNormal);
    //        blaVec3 normalBody2W = body2->GetObjectTransform().LocalDirectionToWorld(body2ContactNormal);


    //        blaVec3 tangentBody1W = body1->GetObjectTransform().LocalDirectionToWorld(body1ContactTangent);
    //        blaVec3 tangentBody2W = body2->GetObjectTransform().LocalDirectionToWorld(body2ContactTangent);

    //        blaVec3 normal, tangent;

    //        if (collidingFace == 1)
    //        {
    //            normal = normalBody1W;

    //            blaVec3 body1to2 = body2->GetObjectTransform().m_position - collisionPoint;
    //            if (glm::dot(normal, body1to2) < 0)
    //                normal = -normal;

    //            tangent = tangentBody1W;
    //        }
    //        else if (collidingFace == 2)
    //        {
    //            normal = normalBody2W;

    //            blaVec3 body2to1 = body1->GetObjectTransform().m_position - collisionPoint;
    //            if (glm::dot(normal, body2to1) < 0)
    //                normal = -normal;

    //            tangent = tangentBody2W;
    //        }

    //        if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z))
    //            continue;

    //        Contact contact
    //        (
    //            body1, body2, 
    //            collisionPoint, 
    //            normal,
    //            tangent,
    //            collidingFace
    //        );

    //        contact.ComputeJacobian();

    //        m_currentContacts.push_back(contact);
    //    }
    //}
}

void CollisionProcessor::SolveContacts()
{
    vector<vector<blaVec3>> T(3 * m_currentContacts.size());
    ComputeT(T);

    vector<float> D(3 * m_currentContacts.size());
    GetDiagonalElements(T, D);

    vector<blaVec3> B;
    for (size_t i = 0; i<m_currentContacts.size(); i++)
    {
        Contact contact = m_currentContacts[i];

        RigidBodyComponent* body1 = contact.m_body1;
        blaVec3 vLUpdate1 = (blaVec3) body1->m_nextState.m_velocity;
        blaVec3 vAUpdate1 = (blaVec3) body1->GetObjectTransform().LocalDirectionToWorld(body1->m_nextState.m_angularVelocity);
        RigidBodyComponent* body2 = contact.m_body2;
        blaVec3 vLUpdate2 = (blaVec3) body2->m_nextState.m_velocity;
        blaVec3 vAUpdate2 = (blaVec3) body2->GetObjectTransform().LocalDirectionToWorld(body2->m_nextState.m_angularVelocity);

        vector<blaVec3> nJac = contact.m_normalJacobian;
        vector<blaVec3> t1Jac = contact.m_tangentJacobian1;
        vector<blaVec3> t2Jac = contact.m_tangentJacobian2;

        float b_normal = glm::dot(nJac[0], vLUpdate1) + glm::dot(nJac[2], vLUpdate2) + glm::dot(nJac[1], vAUpdate1) + glm::dot(nJac[3], vAUpdate2);
        b_normal += m_bounce * (glm::dot(nJac[0], vLUpdate1) + glm::dot(nJac[2], vLUpdate2) + glm::dot(nJac[1], vAUpdate1) + glm::dot(nJac[3], vAUpdate2));

        float b_tangent1 = glm::dot(t1Jac[0], vLUpdate1) + glm::dot(t1Jac[1], vAUpdate1) + glm::dot(t1Jac[2], vLUpdate2) + glm::dot(t1Jac[3], vAUpdate2);

        float b_tangent2 = glm::dot(t2Jac[0], vLUpdate1) + glm::dot(t2Jac[1], vAUpdate1) + glm::dot(t2Jac[2], vLUpdate2) + glm::dot(t2Jac[3], vAUpdate2);

        B.push_back(blaVec3(b_normal/D[3*i + 0], b_tangent1 / D[3 * i + 1], b_tangent2 / D[3 * i + 2]));
    }

    // Gauss-Seidel Solver:
    int iteration = 0;

    vector<blaVec3> lambdas(m_currentContacts.size());
    for (size_t i = 0; i < m_currentContacts.size(); i++)
    {
        lambdas[i] = blaVec3(0);
    }

    // Randomize contact order for faster convergence
    vector<int> contactIndices(m_currentContacts.size());
    for (size_t i = 0; i < m_currentContacts.size(); i++)
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
                RigidBodyComponent* body1 = contact.m_body1;
                RigidBodyComponent* body2 = contact.m_body2;

                vector<blaVec3>* jacobianEntry;
                if (i == 0)
                    jacobianEntry = &(contact.m_normalJacobian);
                else if (i == 1)
                    jacobianEntry = &(contact.m_tangentJacobian1);
                else if (i == 2)
                    jacobianEntry = &(contact.m_tangentJacobian2);

                float a = glm::dot((*jacobianEntry)[0], body1->m_nextState.m_correctionLinearVelocity);
                float b = glm::dot((*jacobianEntry)[2], body2->m_nextState.m_correctionLinearVelocity);
                float c = glm::dot((*jacobianEntry)[1], body1->m_nextState.m_correctionAngularVelocity);
                float d = glm::dot((*jacobianEntry)[3], body2->m_nextState.m_correctionAngularVelocity);

                float correction = a+b+c+d;

                float Bd = (B[index][i]);
                float Cd = (correction / D[3 * index + i]);
                float newLambda = lambdas[index][i] - Bd - Cd;

                if (i == 0) // Normal direction 
                {
                    newLambda = newLambda > 0 ? newLambda : 0;
                }
                else if (i == 1 || i == 2)  // Tangential direction 1 
                {
                    float a = newLambda < (-m_friction * newLambda) ? (-m_friction * newLambda) : newLambda;
                    newLambda = a > m_friction * newLambda ? m_friction * newLambda : a;
                }

                float deltaLambda = newLambda - lambdas[index][i];

                body1->m_nextState.m_correctionLinearVelocity  += deltaLambda * T[3 * index + i][0];
                body1->m_nextState.m_correctionAngularVelocity += deltaLambda * T[3 * index + i][1];
                body2->m_nextState.m_correctionLinearVelocity  += deltaLambda * T[3 * index + i][2];
                body2->m_nextState.m_correctionAngularVelocity += deltaLambda * T[3 * index + i][3];

                lambdas[index][i] = newLambda;

                averageDistance += fabs(deltaLambda);
            }
        }
        if (isnan(averageDistance) || isnan(-averageDistance) || fabs(averageDistance) > 1000000)
        {
			Console::LogMessage("Lambdas blew up: " + std::to_string(iteration) + " iterations");
			Console::LogMessage("Average Distance: " + std::to_string(averageDistance));

            debug_stop = true;
        }
        averageDistance /= 3 * lambdas.size();
        iteration++;
    }
    if (iteration == m_maxIterations)
    {
		Console::LogMessage("LCP Solver did not converge after " + std::to_string(m_maxIterations) + " iterations");
    }
    m_iterationCount = iteration;
    m_solveCount++;
}

void CollisionProcessor::ComputeT(vector<vector<blaVec3>>& T)
{
    for (size_t i = 0; i < 3 * m_currentContacts.size(); i++)
    {
        Contact contact = m_currentContacts[(int)(i/3)];

        RigidBodyComponent* body1 = contact.m_body1;
        RigidBodyComponent* body2 = contact.m_body2;

        vector<blaVec3> Ti(4);

        vector<blaVec3> jacobianEntry;
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

void CollisionProcessor::GetDiagonalElements(vector<vector<blaVec3>> T, vector<float>& D)
{
    for (size_t i = 0; i < 3 * m_currentContacts.size(); i++)
    {
        Contact contact = m_currentContacts[(int)(i / 3)];

        RigidBodyComponent* body1 = contact.m_body1;
        RigidBodyComponent* body2 = contact.m_body2;

        vector<blaVec3> jacobianEntry;
        if (i % 3 == 0)
            jacobianEntry = contact.m_normalJacobian;
        else if (i % 3 == 1)
            jacobianEntry = contact.m_tangentJacobian1;
        else if (i % 3 == 2)
            jacobianEntry = contact.m_tangentJacobian2;

        D[i] =  glm::dot(jacobianEntry[0], T[i][0]) + 
                glm::dot(jacobianEntry[1], T[i][1]) + 
                glm::dot(jacobianEntry[2], T[i][2]) + 
                glm::dot(jacobianEntry[3], T[i][3]);
    }
}

void CollisionProcessor::ProcessCollisions()
{
    m_currentContacts.clear();

    float startTime = m_time->GetTime();
    BroadPhaseDetection();
    m_detectionTime = m_time->GetTime() - startTime;

    startTime = m_time->GetTime();
    if(m_currentContacts.size() != 0)
        SolveContacts();
    m_processingTime = m_time->GetTime() - startTime;
}

Contact::Contact(RigidBodyComponent* body1, RigidBodyComponent* body2, blaVec3 colPoint, blaVec3 normalW, blaVec3 tangentW, int face):
    m_normalJacobian(vector<blaVec3>(4)),
    m_tangentJacobian1(vector<blaVec3>(4)),
    m_tangentJacobian2(vector<blaVec3>(4)),
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
    float sign = m_faceFrom == 1.f ? 1.f : -1.f;

    blaVec3 radialBody1 = m_contactPositionW - m_body1->GetObjectTransform().GetPosition();
    blaVec3 radialBody2 = m_contactPositionW - m_body2->GetObjectTransform().GetPosition();

    blaVec3 crossNormal1 = cross(radialBody1, m_contactNormalW);
    blaVec3 crossNormal2 = cross(radialBody2, m_contactNormalW);

    m_normalJacobian[0] = -sign * m_contactNormalW;
    m_normalJacobian[1] = -sign * crossNormal1;
    m_normalJacobian[2] = sign * m_contactNormalW;
    m_normalJacobian[3] = sign * crossNormal2;

    blaVec3 crossTangent11 = cross(radialBody1, m_contactTangent1W);
    blaVec3 crossTangent12 = cross(radialBody2, m_contactTangent1W);
    
    m_tangentJacobian1[0] = -sign * m_contactTangent1W;
    m_tangentJacobian1[1] = -sign * crossTangent11;
    m_tangentJacobian1[2] = sign * m_contactTangent1W;
    m_tangentJacobian1[3] = sign * crossTangent12;

    blaVec3 crossTangent21 = cross(radialBody1, m_contactTangent2W);
    blaVec3 crossTangent22 = cross(radialBody2, m_contactTangent2W);

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
