#include "IKTools.h"
#include "SkeletonAnimationData.h"
#include <stack>
#include <utility>

using namespace std;

#pragma optimize("", off)

IKChainJoint* IKChainJoint::CreateTestIKChain2Ends(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin)
{
	auto root = new IKChainJoint(origin, sizeOfBones);
	root->m_jointPosition = origin;

	auto joint = root;
	for(int i = 1; i < numberOfJoints; i++)
	{
		blaVec3 newPos = joint->m_jointPosition + blaVec3(0.f, sizeOfBones, 0.f);
		auto newChild = new IKChainJoint(newPos, sizeOfBones);
		joint->AddChild(newChild);

		joint = newChild;
	}

	joint = root;
	for (int i = 1; i < numberOfJoints; i++)
	{
		blaVec3 newPos = joint->m_jointPosition - blaVec3(0.f, sizeOfBones, 0.f);		
		auto newChild = new IKChainJoint(newPos, sizeOfBones);
		joint->AddChild(newChild);

		joint = newChild;
	}

	return root;
}

void IKChainJoint::GetEndEffectorRecursive(IKChainJoint* joint, std::vector<IKChainJoint*>& results)
{
	if(joint->GetNumberChildren() < 1)
	{
		results.emplace_back(joint);
	}
	
	auto child = joint->GetChild();
	while(child != nullptr)
	{
		GetEndEffectorRecursive(child, results);
		child = child->GetNext();
	}
}

std::vector<IKChainJoint*> IKChainJoint::GetEndEffectors()
{
	std::vector<IKChainJoint*> result;
	
	if (GetNumberChildren() > 0)
	{
		GetEndEffectorRecursive(this, result);
	}

	return result;
}

IKChainJoint * IKChainJoint::BuildFromSkeleton(const SkeletonJoint* skeletonRoot, const vector<blaPosQuat>& jointTransformsW)
{
	IKChainJoint* ikJoint = new IKChainJoint(jointTransformsW[skeletonRoot->GetJointIndex()].GetTranslation3(), 0.f);

	ikJoint->m_joint = skeletonRoot;

	auto child = skeletonRoot->GetChild();
	while(child != nullptr)
	{
		auto childIkJoint = BuildFromSkeleton(child, jointTransformsW);
		ikJoint->m_length = glm::length(ikJoint->m_jointPosition - childIkJoint->m_jointPosition);
		ikJoint->AddChild(childIkJoint);
		child = child->GetNext();
	}

	return ikJoint;
}

void IKChainJoint::AddChild(IKChainJoint* next)
{
	IntrusiveTree<IKChainJoint>::AddChild(next);
	next->m_parent = this;
}

void IKChainJoint::GetBoneArray(vector<pair<blaVec3, blaVec3>>& outputBones, const IKChainJoint& skeleton)
{
	auto child = skeleton.GetChild();

	while(child != nullptr)
	{
		outputBones.emplace_back(std::pair<blaVec3, blaVec3>(skeleton.m_jointPosition, child->m_jointPosition));
		GetBoneArray(outputBones, *child);
		child = child->GetNext();
	}
}

void IKChainJoint::SolveIKChain(IKChainJoint* root, vector<blaVec3> endEffectorDesiredPositions)
{
	vector<IKChainJoint*> endEffectors = root->GetEndEffectors();

	for (int i = 0; i < endEffectors.size(); ++i)
	{
		auto endEffector = endEffectors[i];

		endEffector->m_jointPosition = endEffectorDesiredPositions[i];

		IKSolveBackwardPhase(endEffector);
		IKSolveForwardPhase(root);
	}
}

void IKChainJoint::SetupJointRotationAxes(IKChainJoint* root)
{

}

void IKChainJoint::IKSolveBackwardPhase(IKChainJoint* currentJoint)
{
	while (currentJoint->m_parent != nullptr)
	{
		auto parent = currentJoint->m_parent;

		//if (parent->m_parent != nullptr)
		{
			const blaVec3 toParent = parent->m_jointPosition - currentJoint->m_jointPosition;
			const blaF32 toParentLength = glm::length(toParent);
			if (toParentLength > BLA_EPSILON)
			{
				parent->m_jointPosition = currentJoint->m_jointPosition + parent->m_length * toParent / toParentLength;
			}
		}

		currentJoint = parent;
	}
}

void IKChainJoint::IKSolveForwardPhase(IKChainJoint* root)
{
	auto currentJoint= root->GetChild();
	while (currentJoint != nullptr)
	{
		blaVec3 toChild = currentJoint->m_jointPosition - root->m_jointPosition;
		blaF32 toChildLength = glm::length(toChild);
		if (toChildLength > BLA_EPSILON)
		{
			currentJoint->m_jointPosition = root->m_jointPosition + root->m_length * toChild / toChildLength;

			//TODO: It actually works quite alright... Now write it better ?
			//Let's try to write a cone-twist constraint geometrically
			// (u.v) / ||u||.||v|| < cos(a) (test a = Pi/4)

			blaVec3 toChild = currentJoint->m_jointPosition - root->m_jointPosition;
			blaF32 toChildLength = glm::length(toChild);

			//...
			if(root->m_parent != nullptr)
			{
				const blaVec3 toCurrent = root->m_jointPosition - root->m_parent->m_jointPosition;
				const blaF32 toCurrentLength = glm::length(toCurrent);

				if(toCurrentLength > BLA_EPSILON)
				{
					blaF32 toChildDotToParent = glm::dot(toCurrent, toChild) / (toCurrentLength * toChildLength);
					if (toChildDotToParent < 1.f - glm::cos(0.25f * M_PI))
					{
						blaVec3 toChildProjectedOnToParent = (toChild / toChildLength) - toCurrent * toChildDotToParent;

						//Normalize is safe here since we already know the two bones aren't colinear...
						currentJoint->m_jointPosition = glm::cos(0.25f * M_PI) * glm::normalize(toChildProjectedOnToParent) + toChild + root->m_jointPosition;
					}
				}
			}
		}

		IKSolveForwardPhase(currentJoint);

		currentJoint = currentJoint->GetNext();
	}
}