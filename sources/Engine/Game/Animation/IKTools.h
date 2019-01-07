#pragma once
#include "../../../Common/DataStructures/Tree.h"
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths/Maths.h"

namespace BLAengine
{
    class IKChainJoint;
    class SkeletonJoint;

    struct IKJointConstraint
    {
    protected:
        enum class IKJointConstraintType
        {
            IKFixedLengthConstraint
        } m_constraintType;
    public:
        IKJointConstraint(IKJointConstraintType type) :
            m_constraintType(type)
        {};

    public:
        static void ClampToConstraint(IKChainJoint* currentJoint);
    };

    struct FixedChildPositionConstraint : IKJointConstraint
    {
    public:
        // Contraint parameterization...
        // This constraint is that the child IKChainJoint has got to be at this place in local Space
        blaVec4 m_childConstrainedPositionL;

    public:
        FixedChildPositionConstraint():
            IKJointConstraint(IKJointConstraintType::IKFixedLengthConstraint){};

        void ClampToConstraint(IKChainJoint* currentJoint) const;
    };

    class BLACORE_API IKChainJoint : public IntrusiveTree<IKChainJoint>
    {
    public:
        const SkeletonJoint* m_joint;
        IKChainJoint* m_parent;

        blaPosQuat m_jointTransform;

        blaF32 m_length;

        IKJointConstraint* m_jointConstraint;

    public:

        IKChainJoint() :
            m_jointTransform(blaPosQuat::GetIdentity()),
            m_length(0.f),
            m_parent(nullptr),
            m_joint(nullptr),
            m_jointConstraint(new FixedChildPositionConstraint())
        {};

        IKChainJoint(blaPosQuat jointTransform, blaF32 length) :
            m_jointTransform(jointTransform),
            m_length(length),
            m_parent(nullptr),
            m_joint(nullptr),
            m_jointConstraint(new FixedChildPositionConstraint())
        {};

        ~IKChainJoint() {};

        std::vector<IKChainJoint*> GetEndEffectors();

        void AddChild(IKChainJoint* next);


    // Static members
        static IKChainJoint* BuildFromSkeleton(const SkeletonJoint* skeletonRoot, const vector<blaPosQuat>& jointTransformsW);

        static IKChainJoint* CreateTestIKChain2Ends(int numberOfJoints, blaF32 sizeOfBones, blaVec3 origin);

        static void GetBoneArray(vector<pair<blaVec3, blaVec3>>& outputBones, const IKChainJoint& skeleton);

        static void GetJointTransforms(vector<blaPosQuat>& jointTransforms, const IKChainJoint& skeleton);

        static void SolveIKChain(IKChainJoint* root, vector<blaVec3> endEffectorDesiredPositions);

        static void SetupJointRotationAxes(IKChainJoint* root);

        static void ComputeForwardKinematic(IKChainJoint) {};

    private:

        static void GetEndEffectorRecursive(IKChainJoint* joint, std::vector<IKChainJoint*>& results);
        
        static void IKSolveForwardPhase(IKChainJoint* currentJoint);
        
        static void IKSolveBackwardPhase(IKChainJoint* currentJoint);
    };
}

