#pragma once
#include"GameComponents\RigidBody.h"

class Contact
{
	int m_contactIndex;
	RigidBody* m_body1;
	RigidBody* m_body2;

	vec3 m_contactNormalW;
	vec3 m_contactPositionW;

};

class CollisionProcessor
{
public:
	CollisionProcessor();
	~CollisionProcessor();

	vector<RigidBody*> m_bodiesList;
	vector<vec3> m_collisionsPoints;

	void ProcessCollisions();
private:

	vector<Contact> m_currentContacts;
	void BroadPhaseDetection();
	void NarrowPhaseDetection(RigidBody* body1, RigidBody* body2);
};