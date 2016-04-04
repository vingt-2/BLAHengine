#pragma once
#include"GameComponents\RigidBody.h"

class Contact
{
public:
	Contact(RigidBody* body1, RigidBody* body2, vec3 colPoint, vec3 normal1W, vec3 normal2W);
	~Contact();

	int m_contactIndex;
	RigidBody* m_body1;
	RigidBody* m_body2;

	vec3 m_contactNormalBody1W;
	vec3 m_contactNormalBody2W;
	vec3 m_contactPositionW;

	float contactJacobian[12];

private:
	void ComputeJacobian();
};

class CollisionProcessor
{
public:
	CollisionProcessor();
	~CollisionProcessor();

	bool m_flagInterpolateNormals;

	vector<RigidBody*> m_bodiesList;
	vector<vec3> m_collisionsPoints;
	vector<Contact> m_currentContacts;

	void ProcessCollisions();
private:

	void BroadPhaseDetection();
	void NarrowPhaseDetection(RigidBody* body1, RigidBody* body2);
};