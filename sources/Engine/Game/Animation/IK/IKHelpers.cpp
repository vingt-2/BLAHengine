#include "IKTools.h"
#include <utility>
#include <vector>

using namespace BLAengine;

#define BLA_IK_CONVERGENCE_EPSILON 0.001f

IKChainJoint* IKChainJoint::CreateTestIKChain2EndsFixedConstraint(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin)
{
    blaPosQuat originId = blaPosQuat::GetIdentity();
    originId.SetTranslation3(origin);
    auto root = new IKChainJoint(originId);
    root->m_jointTransform.SetTranslation3(origin);

    auto joint = root;
    for (int i = 0; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(0.f, i == 0 ? 0.f : sizeOfBones, 0.f);
        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        FixedChildPositionConstraint* constraint = new FixedChildPositionConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 0; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() - blaVec3(0.f, i == 0 ? 0.f : sizeOfBones, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        FixedChildPositionConstraint* constraint = new FixedChildPositionConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 0; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(i == 0 ? 0.f : sizeOfBones, 0.f, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        FixedChildPositionConstraint* constraint = new FixedChildPositionConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() - blaVec3(i == 0 ? 0.f : sizeOfBones, 0.f, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        FixedChildPositionConstraint* constraint = new FixedChildPositionConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    return root;
}

IKChainJoint* IKChainJoint::CreateTestIKChain2EndsConeTwist(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin)
{
    blaPosQuat originId = blaPosQuat::GetIdentity();
    originId.SetTranslation3(origin);
    auto root = new IKChainJoint(originId);
    root->m_jointTransform.SetTranslation3(origin);

    auto joint = root;
    for (int i = 0; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(0.f, i == 0 ? 0.f : sizeOfBones, 0.f);
        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientationLocalToParent = blaPosQuat::QuatIdentity();
        constraint->m_coneAngleConstraint = 0.5 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 0; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() - blaVec3(0.f, i == 0 ? 0.f : sizeOfBones, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientationLocalToParent = blaPosQuat::QuatIdentity();
        constraint->m_coneAngleConstraint = 0.5 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 0; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() + blaVec3(i == 0 ? 0.f : sizeOfBones, 0.f, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientationLocalToParent = blaPosQuat::QuatIdentity();
        constraint->m_coneAngleConstraint = 0.5 * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    joint = root;
    for (int i = 1; i < numberOfJoints; i++)
    {
        blaVec3 newPos = joint->m_jointTransform.GetTranslation3() - blaVec3(i == 0 ? 0.f : sizeOfBones, 0.f, 0.f);

        blaPosQuat newPosId = blaPosQuat::GetIdentity();
        newPosId.SetTranslation3(newPos);
        auto newChild = new IKChainJoint(newPosId);

        ConeTwistConstraint* constraint = new ConeTwistConstraint();

        constraint->m_childConstrainedPositionL = joint->m_jointTransform.GetInverse() * blaVec4(newPos, 1.f);
        constraint->m_restOrientationLocalToParent = blaPosQuat::QuatIdentity();
        constraint->m_coneAngleConstraint = 0.5f * M_PI;

        joint->m_jointConstraints.push_back(constraint);

        joint->AddChild(newChild);

        joint = newChild;
    }

    return root;
}