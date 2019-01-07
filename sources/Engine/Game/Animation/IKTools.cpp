#include "IKTools.h"
#include "SkeletonAnimationData.h"
#include <stack>
#include <utility>
#include "../../EngineInstance.h"
#include "../Debug.h"

using namespace std;
using namespace BLAengine;

#pragma optimize("", off)

IKChainJoint* IKChainJoint::CreateTestIKChain2Ends(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin)
{
    blaPosQuat originId = blaPosQuat::GetIdentity();
    originId.SetTranslation3(origin);
    auto root = new IKChainJoint(originId, sizeOfBones);
    root->m_jointTransform.SetTranslation3(origin);

    auto joint = root;
    for(int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(0.f, sizeOfBones, 0.f);
        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId, sizeOfBones);

        static_cast<FixedChildPositionConstraint*>(joint->m_jointConstraint)->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos,1.f);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() - blaVec3(0.f, sizeOfBones, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId, sizeOfBones);

        static_cast<FixedChildPositionConstraint*>(joint->m_jointConstraint)->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);

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
    IKChainJoint* ikJoint = new IKChainJoint(jointTransformsW[skeletonRoot->GetJointIndex()], 0.f);

    ikJoint->m_joint = skeletonRoot;

    auto child = skeletonRoot->GetChild();
    while(child != nullptr)
    {
        auto childIkJoint = BuildFromSkeleton(child, jointTransformsW);
        ikJoint->m_length = glm::length(ikJoint->m_jointTransform.GetTranslation3() - childIkJoint->m_jointTransform.GetTranslation3());
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
        outputBones.emplace_back(std::pair<blaVec3, blaVec3>(skeleton.m_jointTransform.GetTranslation3(), child->m_jointTransform.GetTranslation3()));
        GetBoneArray(outputBones, *child);
        child = child->GetNext();
    }
}

void IKChainJoint::GetJointTransforms(vector<blaPosQuat>& jointTransforms, const IKChainJoint& skeleton)
{
    jointTransforms.emplace_back(skeleton.m_jointTransform);
    auto child = skeleton.GetChild();

    while (child != nullptr)
    {
        GetJointTransforms(jointTransforms, *child);
        child = child->GetNext();
    }
}

void IKChainJoint::SolveIKChain(IKChainJoint* root, vector<blaVec3> endEffectorDesiredPositions)
{
    vector<IKChainJoint*> endEffectors = root->GetEndEffectors();

    for (int i = 0; i < endEffectors.size(); ++i)
    {
        auto endEffector = endEffectors[i];

        endEffector->m_jointTransform.SetTranslation3(endEffectorDesiredPositions[i]);

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

        IKJointConstraint::ClampToConstraint(parent);

        currentJoint = parent;
    }
}

void IKChainJoint::IKSolveForwardPhase(IKChainJoint* root)
{
    auto currentJoint = root->GetChild();

    //while (currentJoint != nullptr)
    //{
    //    
    //}
   
}

void IKJointConstraint::ClampToConstraint(IKChainJoint* currentJoint)
{
    // Implement FixedChildPositionConstraint clamping
    switch (currentJoint->m_jointConstraint->m_constraintType)
    {
    case IKJointConstraint::IKJointConstraintType::IKFixedLengthConstraint:
        // Safe downcasting don't harass me.
        FixedChildPositionConstraint* constraint = static_cast<FixedChildPositionConstraint*>(currentJoint->m_jointConstraint);
        constraint->ClampToConstraint(currentJoint);
    }
}

void FixedChildPositionConstraint::ClampToConstraint(IKChainJoint* currentJoint) const
{
    const IKChainJoint* childJoint = currentJoint->GetChild();

    if (childJoint != nullptr)
    {
        const blaVec4& currentJointPositionW = currentJoint->m_jointTransform.GetTranslation();
        const blaVec4& childPositionW = childJoint->m_jointTransform.GetTranslation();

        const blaVec3 currentToConstrainedChildPositionW = currentJoint->m_jointTransform.TransformVector(m_childConstrainedPositionL);

        const blaVec3 currentToChildW = childPositionW - currentJointPositionW;

        const blaF32 constraintLength = glm::length(m_childConstrainedPositionL);

        if (glm::length2(currentToConstrainedChildPositionW - currentToChildW) > 0.001f)
        {
            // Rotate towards solution to minimize displacement
            currentJoint->m_jointTransform.GetRotation() *= QuatBetweenVectors(currentToConstrainedChildPositionW, currentToChildW);

            // Snap Translation to proper length;
            currentJoint->m_jointTransform.SetTranslation(childPositionW - constraintLength * currentJoint->m_jointTransform.TransformVector(m_childConstrainedPositionL));
        }
    }
}