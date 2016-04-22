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

	dvec3 m_contactNormalW;
	dvec3 m_contactTangent1W;
	dvec3 m_contactTangent2W;

	dvec3 m_contactPositionW;

	dvec3 debug_radialBody1;
	dvec3 debug_radialBody2;

	dvec3 debug_position1;
	dvec3 debug_position2;

	vector<dvec3> m_normalJacobian;
	vector<dvec3> m_tangentJacobian1;
	vector<dvec3> m_tangentJacobian2;

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
	double m_epsilon;

	bool debug_stop;

	vector<RigidBody*> m_bodiesList;
	vector<Contact> m_currentContacts;

	void ProcessCollisions();
private:

	void ComputeT(vector<vector<dvec3>>& T);
	void GetDiagonalElements(vector<vector<dvec3>> T, vector<double>& D);
	void BroadPhaseDetection();
	void NarrowPhaseDetection(RigidBody* body1, RigidBody* body2);
	void SolveContacts();
};