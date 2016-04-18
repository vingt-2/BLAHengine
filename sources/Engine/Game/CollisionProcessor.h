#pragma once
#include"GameComponents\RigidBody.h"

class Contact
{
public:
	Contact(RigidBody* body1, RigidBody* body2, vec3 colPoint, vec3 normalW, vec3 tangentW, int face);
	~Contact();

	int m_faceFrom;
	RigidBody* m_body1;
	RigidBody* m_body2;

	vec3 m_contactNormalW;
	vec3 m_contactTangent1W;
	vec3 m_contactTangent2W;

	vec3 m_contactPositionW;

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