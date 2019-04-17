#include "IKTools.h"
#include <Engine/Game/Animation/SkeletonAnimationData.h>
#include <stack>
#include <utility>
#include <Engine/EngineInstance.h>
#include <Engine/Debug/Debug.h>
#include <vector>

using namespace BLAengine;

#define BLA_IK_CONVERGENCE_EPSILON 0.001f

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

void BLAengine::IKChainJoint::ResetIKChainRecursive(IKChainJoint* joint)
{
	joint->m_jointTransform = joint->m_bindPoseTransform;

	auto child = joint->GetChild();
	while (child != nullptr)
	{
		ResetIKChainRecursive(child);
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
    if (skeletonRoot->GetJointIndex() > jointTransformsW.size())
    {
        std::cout << "There is a problem here \n";
    }

    IKChainJoint* ikJoint = new IKChainJoint(jointTransformsW[skeletonRoot->GetJointIndex()]);

    ikJoint->m_joint = skeletonRoot;

    auto child = skeletonRoot->GetChild();

    if (skeletonRoot->GetNumberChildren() > 1)
    {
        while (child != nullptr)
        {
            IKChainJoint* middleIKJoint = new IKChainJoint(jointTransformsW[skeletonRoot->GetJointIndex()]);

            FixedChildPositionConstraint* middleConstraint = new FixedChildPositionConstraint();

            middleConstraint->m_childConstrainedPositionL = blaVec4(0.f, 0.f, 0.f, 1.f);

            ikJoint->m_jointConstraints.push_back(middleConstraint);

            ikJoint->AddChild(middleIKJoint);

            auto childIkJoint = BuildFromSkeleton(child, jointTransformsW);

            FixedChildPositionConstraint* constraint = new FixedChildPositionConstraint();

            constraint->m_childConstrainedPositionL = middleIKJoint->m_jointTransform.GetTranslation() - childIkJoint->m_jointTransform.GetTranslation();

            middleIKJoint->m_jointConstraints.push_back(constraint);

            middleIKJoint->AddChild(childIkJoint);

            child = child->GetNext();
        }
    }
    else if (skeletonRoot->GetNumberChildren() > 0)
    {
        auto childIkJoint = BuildFromSkeleton(child, jointTransformsW);

        /*ConeTwistConstraint* constraint = new ConeTwistConstraint();
        constraint->m_restOrientationLocalToParent = blaPosQuat::QuatIdentity();
        constraint->m_childConstrainedPositionL = ikJoint->m_jointTransform.GetTranslation() - childIkJoint->m_jointTransform.GetTranslation();*/

        FixedChildPositionConstraint* constraint = new FixedChildPositionConstraint();

        constraint->m_childConstrainedPositionL = ikJoint->m_jointTransform.GetTranslation() - childIkJoint->m_jointTransform.GetTranslation();

        ikJoint->m_jointConstraints.push_back(constraint);

        ikJoint->AddChild(childIkJoint);
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
	ResetIKChainRecursive(root);

    for (int i = 0; i < iterationCount; ++i)
    {
        blaF32 distanceToEndEffectors = 0.f;

        vector<IKChainJoint*> endEffectors = root->GetEndEffectors();

        size_t nbEffectors = endEffectors.size();

        for (int j = 0; j < nbEffectors; ++j)
        {
            auto endEffector = endEffectors[j];

            distanceToEndEffectors += glm::length2(endEffector->m_jointTransform.GetTranslation3() - endEffectorDesiredPositions[j]);
        }

        if (distanceToEndEffectors < BLA_IK_CONVERGENCE_EPSILON)
        {
            break;
        }

        for (size_t j = 0; j < nbEffectors; ++j)
        {
            auto endEffector = endEffectors[j];

            endEffector->m_jointTransform.SetTranslation3(endEffectorDesiredPositions[j]);

            IKSolveBackwardPhase(endEffector);
            IKSolveForwardPhase(root);
        }
    }
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

    if (glm::length2(currentToConstrainedChildPositionW - currentToChildW) > BLA_IK_CONVERGENCE_EPSILON)
    {
        // Rotate towards solution to minimize displacement
        parentJoint->m_jointTransform.GetRotation() *= QuatBetweenVectors(currentToConstrainedChildPositionW, currentToChildW);

        // Snap Translation to proper length;
        parentJoint->m_jointTransform.SetTranslation(childPositionW - constraintLength * SafeNormalize(parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL)));
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

    if (glm::length2(currentToConstrainedChildPositionW - currentToChildW) > BLA_IK_CONVERGENCE_EPSILON)
    {
        // Rotate towards solution to minimize displacement
        parentJoint->m_jointTransform.GetRotation() *= QuatBetweenVectors(currentToConstrainedChildPositionW, currentToChildW);

        blaQuat restOrientationW;

        if (parentJoint->m_parent != nullptr)
        {
            restOrientationW = parentJoint->m_parent->m_jointTransform.GetRotation() * constraint->m_restOrientationLocalToParent;
        }
        else
        {
            restOrientationW = constraint->m_restOrientationLocalToParent;
        }

        const blaVec3 restToChildW = restOrientationW * constraint->m_childConstrainedPositionL;

        const blaVec3 newToChildW = parentJoint->m_jointTransform.TransformVector(constraint->m_childConstrainedPositionL);

        blaF32 angleRestToNew = acosf(glm::dot(restToChildW, newToChildW) / glm::pow(glm::length(restToChildW),2.f));

        blaF32 clampedAngle = angleRestToNew > constraint->m_coneAngleConstraint ? constraint->m_coneAngleConstraint : angleRestToNew;

        if (clampedAngle > BLA_EPSILON)
        {
            blaQuat clampedOrientation = glm::angleAxis(clampedAngle, glm::normalize(glm::cross(restToChildW, newToChildW)));

            parentJoint->m_jointTransform.SetRotation(restOrientationW * clampedOrientation);
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