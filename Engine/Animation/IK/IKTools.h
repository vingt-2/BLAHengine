// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "IntrusiveTree.h"
#include "StdInclude.h"
#include "Maths/Maths.h"

namespace BLA
{
    class IKChainJoint;
    class SkeletonJoint;

    struct IKJointConstraint
    {
    protected:
        enum class IKJointConstraintType
        {
            IKFixedLengthConstraint,
            IKConeTwistConstraint
        } m_constraintType;

    public:
        IKJointConstraint(IKJointConstraintType type) :
            m_constraintType(type)
        {};

        static void BackwardClampToConstraint(const IKJointConstraint* constraint, IKChainJoint* currentJoint, const IKChainJoint* childJoint);
        static void ForwardClampToConstraint(const IKJointConstraint* constraint, const IKChainJoint* currentJoint, IKChainJoint* childJoint);
    };

    struct FixedChildPositionConstraint : IKJointConstraint
    {
        // Contraint parameterization...
        // This constraint is that the child IKChainJoint has got to be at this place in local Space
        blaVec4 m_childConstrainedPositionL;

        FixedChildPositionConstraint() :
            IKJointConstraint(IKJointConstraintType::IKFixedLengthConstraint) {};

        static void BackwardClampToConstraint(const FixedChildPositionConstraint* constraint, IKChainJoint* currentJoint, const IKChainJoint* childJoint);
        static void ForwardClampToConstraint(const FixedChildPositionConstraint* constraint, const IKChainJoint* currentJoint, IKChainJoint* childJoint);
    };

    struct ConeTwistConstraint : IKJointConstraint
    {
        // Contraint parameterization...
        // This constraint is that the child IKChainJoint has got to be at this place in local Space
        blaVec4 m_childConstrainedPositionL;
        blaQuat m_restOrientationLocalToParent;
        blaF32 m_coneAngleConstraint = 0.f;

        ConeTwistConstraint() :
            IKJointConstraint(IKJointConstraintType::IKConeTwistConstraint) {};

        static void BackwardClampToConstraint(const ConeTwistConstraint* constraint, IKChainJoint* currentJoint, const IKChainJoint* childJoint);
        static void ForwardClampToConstraint(const ConeTwistConstraint* constraint, const IKChainJoint* currentJoint, IKChainJoint* childJoint);
    };

    class IKChainJoint : public IntrusiveTree<IKChainJoint>
    {
    public:
        const SkeletonJoint* m_joint;
        IKChainJoint* m_parent;

        blaPosQuat m_jointTransform;
        blaPosQuat m_bindPoseTransform;

        blaVector<IKJointConstraint*> m_jointConstraints;

    public:

        IKChainJoint() :
            m_jointTransform(blaPosQuat::GetIdentity()),
            m_bindPoseTransform(m_jointTransform),
            m_parent(nullptr),
            m_joint(nullptr)
        {};

        IKChainJoint(blaPosQuat jointTransform) :
            m_jointTransform(jointTransform),
            m_bindPoseTransform(m_jointTransform),
            m_parent(nullptr),
            m_joint(nullptr)
        {};

        ~IKChainJoint() {};

        blaVector<IKChainJoint*> GetEndEffectors();

        void AddChild(IKChainJoint* next);


        // Static members
        static IKChainJoint* BuildFromSkeleton(const SkeletonJoint* skeletonRoot, const blaVector<blaPosQuat>& jointTransformsW);

        static IKChainJoint* CreateTestIKChain2EndsFixedConstraint(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin);

        static IKChainJoint* CreateTestIKChain2EndsConeTwist(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin);

        static void GetBoneArray(blaVector<blaPair<blaVec3, blaVec3>>& outputBones, const IKChainJoint& skeleton);

        static void GetJointTransforms(blaVector<blaPosQuat>& jointTransforms, const IKChainJoint& skeleton);

        static void SolveIKChain(IKChainJoint* root, blaVector<blaPosQuat> endEffectorDesiredTransform, int iterationCount);

        static void ComputeForwardKinematic(IKChainJoint) {};

    private:

        static void GetEndEffectorRecursive(IKChainJoint* joint, blaVector<IKChainJoint*>& results);

        static void ResetIKChainRecursive(IKChainJoint* joint);

        static void IKSolveForwardPhase(IKChainJoint* currentJoint);

        static void IKSolveBackwardPhase(IKChainJoint* currentJoint);
    };
}

