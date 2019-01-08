#include "IKTools.h"
#include "SkeletonAnimationData.h"
#include <stack>
#include <utility>
#include "../../EngineInstance.h"
#include "../Debug.h"
#include <glm/common.hpp>
#include <vector>

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
    for (int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(0.f, sizeOfBones, 0.f);
        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId, sizeOfBones);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientation = blaPosQuat::blaQuatIdentity();
        constraint->m_coneAngleConstraint = 0.125 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

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

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientation = blaPosQuat::blaQuatIdentity();
        constraint->m_coneAngleConstraint = 0.125 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(sizeOfBones, 0.f, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId, sizeOfBones);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientation = blaPosQuat::blaQuatIdentity();
        constraint->m_coneAngleConstraint = 0.125 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() - blaVec3(sizeOfBones, 0.f, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId, sizeOfBones);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientation = blaPosQuat::blaQuatIdentity();
        constraint->m_coneAngleConstraint = 0.125 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    return root;
}

void IKChainJoint::GetEndEffectorRecursive(IKChainJoint* joint, std::vector<IKChainJoint*>& results)
{
    if (joint->GetNumberChildren() < 1)
    {
        results.emplace_back(joint);
    }

    auto child = joint->GetChild();
    while (child != nullptr)
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
    while (child != nullptr)
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

    while (child != nullptr)
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

void IKChainJoint::SolveIKChain(IKChainJoint* root, vector<blaVec3> endEffectorDesiredPositions, int iterationCount)
{
    for (int i = 0; i < iterationCount; ++i)
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
}

void IKChainJoint::SetupJointRotationAxes(IKChainJoint* root)
{

}

void IKChainJoint::IKSolveBackwardPhase(IKChainJoint* currentJoint)
{
    while (currentJoint->m_parent != nullptr)
    {
        auto parentJoint = currentJoint->m_parent;

        auto next = parentJoint->GetChild();

        int i = 0;
        while (next != nullptr)
        {
            if (next == currentJoint)
                break;
            i++;
            next = next->GetNext();
        }

        IKJointConstraint::BackwardClampToConstraint(parentJoint->m_jointConstraints[i], parentJoint, currentJoint);

        currentJoint = parentJoint;
    }
}


void IKChainJoint::IKSolveForwardPhase(IKChainJoint* root)
{
    IKChainJoint* currentChild = root->GetChild();

    int i = 0;
    while (currentChild != nullptr)
    {
        IKJointConstraint::ForwardClampToConstraint(root->m_jointConstraints[i], root, currentChild);

        i++;

        IKSolveForwardPhase(currentChild);

        currentChild = currentChild->GetNext();
    }
}

void IKJointConstraint::BackwardClampToConstraint(const IKJointConstraint* constraint, IKChainJoint* parentJoint, const IKChainJoint* childJoint)
{
    switch (constraint->m_constraintType)
    {
        case IKJointConstraintType::IKFixedLengthConstraint:
        {
            FixedChildPositionConstraint::BackwardClampToConstraint(static_cast<const FixedChildPositionConstraint*>(constraint), parentJoint, childJoint);
            break;
        }
        case IKJointConstraintType::IKConeTwistConstraint:
        {
            ConeTwistConstraint::BackwardClampToConstraint(static_cast<const ConeTwistConstraint*>(constraint), parentJoint, childJoint);
            break;
        }
    }
}

void IKJointConstraint::ForwardClampToConstraint(const IKJointConstraint* constraint, const IKChainJoint* parentJoint, IKChainJoint* childJoint)
{
    switch (constraint->m_constraintType)
    {
        case IKJointConstraintType::IKFixedLengthConstraint:
        {
            FixedChildPositionConstraint::ForwardClampToConstraint(static_cast<const FixedChildPositionConstraint*>(constraint), parentJoint, childJoint);
            break;
        }
        case IKJointConstraintType::IKConeTwistConstraint:
        {
            ConeTwistConstraint::ForwardClampToConstraint(static_cast<const ConeTwistConstraint*>(constraint), parentJoint, childJoint);
            break;
        }
    }
}

void FixedChildPositionConstraint::BackwardClampToConstraint(const FixedChildPositionConstraint* constraint, IKChainJoint* parentJoint, const IKChainJoint* childJoint)
{
    const blaVec4& currentJointPositionW = parentJoint->m_jointTransform.GetTranslation();
    const blaVec4& childPositionW = childJoint->m_jointTransform.GetTranslation();

    const blaVec3 currentToConstrainedChildPositionW = parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL);

    const blaVec3 currentToChildW = childPositionW - currentJointPositionW;

    const blaF32 constraintLength = glm::length(constraint->m_childConstrainedPositionL);

    if (glm::length2(currentToConstrainedChildPositionW - currentToChildW) > 0.00001f)
    {
        // Rotate towards solution to minimize displacement
        parentJoint->m_jointTransform.GetRotation() *= QuatBetweenVectors(currentToConstrainedChildPositionW, currentToChildW);

        // Snap Translation to proper length;
        parentJoint->m_jointTransform.SetTranslation(childPositionW - constraintLength * parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL));
    }
}

// The forward clamp of this constraint only acts on the child position.
void FixedChildPositionConstraint::ForwardClampToConstraint(const FixedChildPositionConstraint* constraint, const IKChainJoint* parentJoint, IKChainJoint* childJoint)
{
    childJoint->m_jointTransform.SetTranslation(parentJoint->m_jointTransform * constraint->m_childConstrainedPositionL);
}

void ConeTwistConstraint::BackwardClampToConstraint(const ConeTwistConstraint* constraint, IKChainJoint* parentJoint, const IKChainJoint* childJoint)
{
    const blaVec4& currentJointPositionW = parentJoint->m_jointTransform.GetTranslation();
    const blaVec4& childPositionW = childJoint->m_jointTransform.GetTranslation();

    const blaVec3 currentToConstrainedChildPositionW = parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL);

    const blaVec3 currentToChildW = childPositionW - currentJointPositionW;

    const blaF32 constraintLength = glm::length(constraint->m_childConstrainedPositionL);

    if (glm::length2(currentToConstrainedChildPositionW - currentToChildW) > 0.00001f)
    {
        // Rotate towards solution to minimize displacement
        parentJoint->m_jointTransform.GetRotation() *= QuatBetweenVectors(currentToConstrainedChildPositionW, currentToChildW);

        const blaVec3 restToChildW = constraint->m_restOrientation * constraint->m_childConstrainedPositionL;

        const blaVec3 newToChildW = parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL);

        blaF32 angleRestToNew = acosf(glm::dot(restToChildW, newToChildW) / glm::pow(glm::length(restToChildW),2.f));

        blaF32 clampedAngle = angleRestToNew > constraint->m_coneAngleConstraint ? constraint->m_coneAngleConstraint : angleRestToNew;

        if (clampedAngle > BLA_EPSILON)
        {
            blaQuat clampedOrientation = glm::angleAxis(clampedAngle, glm::normalize(glm::cross(restToChildW, newToChildW)));

            parentJoint->m_jointTransform.SetRotation(constraint->m_restOrientation * clampedOrientation);
        }

        // Snap Translation to proper length;
        parentJoint->m_jointTransform.SetTranslation(childPositionW - constraintLength * parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL));
    }
}

// The forward clamp of this constraint only acts on the child position.
void ConeTwistConstraint::ForwardClampToConstraint(const ConeTwistConstraint* constraint, const IKChainJoint* parentJoint, IKChainJoint* childJoint)
{
    childJoint->m_jointTransform.SetTranslation(parentJoint->m_jointTransform * constraint->m_childConstrainedPositionL);
}