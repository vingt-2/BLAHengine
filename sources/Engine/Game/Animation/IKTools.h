#pragma once
#include "../../../Common/DataStructures/Tree.h"
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths/Maths.h"

class SkeletonJoint;

//First implementation of IK Chain as a chain of 3D points in space.
class IKChainJoint : public IntrusiveTree<IKChainJoint>
{ 
public:
	const SkeletonJoint* m_joint;
	IKChainJoint* m_parent;
	blaVec3 m_jointPosition;

	blaQuat m_jointOrientation;

	blaF32 m_length;

	IKChainJoint() :
		m_jointPosition(blaVec3(0.f)),
		m_length(0.f),
		m_parent(nullptr),
		m_joint(nullptr)
	{};
	
	IKChainJoint(blaVec3 position, blaF32 length) :
		m_jointPosition(position),
		m_length(length),
		m_parent(nullptr), 
		m_joint(nullptr)
	{};

	~IKChainJoint() {};

	std::vector<IKChainJoint*> GetEndEffectors();

	static IKChainJoint* BuildFromSkeleton(const SkeletonJoint* skeletonRoot, const vector<blaPosQuat>& jointTransformsW);

	static IKChainJoint* CreateTestIKChain2Ends(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin);

	void AddChild(IKChainJoint* next);

	static void GetBoneArray(vector<pair<blaVec3, blaVec3>>& outputBones, const IKChainJoint& skeleton);

	static void SolveIKChain(IKChainJoint* root, vector<blaVec3> endEffectorDesiredPositions);

	static void SetupJointRotationAxes(IKChainJoint* root);

	static void ComputeForwardKinematic(IKChainJoint);
private:
	static void GetEndEffectorRecursive(IKChainJoint* joint, std::vector<IKChainJoint*>& results);
	static void IKSolveForwardPhase(IKChainJoint* currentJoint);
	static void IKSolveBackwardPhase(IKChainJoint* currentJoint);
};