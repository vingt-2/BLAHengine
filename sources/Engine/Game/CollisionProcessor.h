#pragma once
#include"GameComponents\RigidBody.h"

class Contact
{
public:
	Contact(RigidBody* body1, RigidBody* body2, vec3 colPoint, vec3 normal1W, vec3 normal2W, vec3 tangent1W, vec3 tangent2W);
	~Contact();

	int m_contactIndex;
	RigidBody* m_body1;
	RigidBody* m_body2;

	vec3 m_contactNormalBody1W;
	vec3 m_contactNormalBody2W;
	vec3 m_contactTangent1Body1W;
	vec3 m_contactTangent1Body2W;
	vec3 m_contactTangent2Body1W;
	vec3 m_contactTangent2Body2W;
	vec3 m_contactPositionW;

	mat3 m_contactFrame1L;
	mat3 m_contactFrame2L;

	vector<vec3> m_normalJacobian;
	vector<vec3> m_tangentJacobian1;
	vector<vec3> m_tangentJacobian2;

	void ComputeJacobian();
private:
};

class CollisionProcessor
{
public:
	CollisionProcessor();
	~CollisionProcessor();

	int m_maxIterations;
	bool m_flagInterpolateNormals;
	float m_friction;
	float m_bounce;

	vector<RigidBody*> m_bodiesList;
	vector<Contact> m_currentContacts;

	void ProcessCollisions();
private:
	void ComputeT(vector<vector<vec3>>& T);
	void GetDiagonalElements(vector<vector<vec3>> T, vector<float>& D);
	void BroadPhaseDetection();
	void NarrowPhaseDetection(RigidBody* body1, RigidBody* body2);
	void SolveContacts();
};