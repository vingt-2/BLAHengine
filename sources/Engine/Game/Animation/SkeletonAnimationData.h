#pragma once
#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>
#include <Common/DataStructures/Tree.h>

/*
    Class SkeletonJoint:
    
    This is the tree node that holds information about a specific joint. A skeleton is a tree of these things.
*/

namespace BLAengine
{
    class BLACORE_API SkeletonJoint : public IntrusiveTree<SkeletonJoint>
    {
    public:

        SkeletonJoint(string name, const blaVec3 &localOffset, blaU32 jointIndex)
        {
            m_name = name;
            m_localOffset = localOffset;
            m_jointIndex = jointIndex;
        }

        ~SkeletonJoint() {};

        string		GetName() const		{ return m_name; }

        blaVec3		GetLocalOffset() const { return m_localOffset; }

        blaU32      GetJointIndex() const { return m_jointIndex; }

        void ApplyOffsetNormalization(float normalizer)	{ m_localOffset /= normalizer; }

        /*
        QuerySkeleton:
        Use to retrieve information from the Skeleton without having to do it recursively yourself.
        Careful there... this has to traverse the skeleton, and may therefore be costly!
        */
        void QuerySkeleton(unordered_map<string, SkeletonJoint*>* jointPointersByNames, vector<pair<string, string>>* bonesByJointNames);

        void PrintJoint();

        void DiscardJointsByName(string subname);

    private:
        string					m_name;
        blaVec3					m_localOffset;
        blaU32                  m_jointIndex;
    };

    class BLACORE_API SkeletonAnimationData
    {
    public:

        SkeletonAnimationData(
            string name,
            vector<vector<blaPosQuat>> jointTransforms,
            SkeletonJoint* skeletonDef,
            float samplingRate,
            int	  frameCount)
        {
            m_name = name;
            m_jointTransforms = jointTransforms;
            m_skeletonDef = skeletonDef;
            m_samplingRate = samplingRate;
            m_frameCount = frameCount;
        };

        ~SkeletonAnimationData();

        string GetName() const		{ return m_name; }

        /*
        Returns the sampling rate in secs as specified in the animation file
        */
        float GetSamplingRate() const { return m_samplingRate; }

        /*
        Returns the length of the animation, in number of frames. Length in time = framecout * sampling_rate
        */
        int GetFrameCount() const { return m_frameCount; }

        /*
        Returns the Root joint of the desired skeleton defined in the animation clip.
        */
        SkeletonJoint* GetSkeleton() const { return m_skeletonDef; }

        /*
        EvaluateAnimation:
        Use this function to retrieve information about the pose of an animation at a frameIndex, in the required formats.
        Careful there... this traverses the skeleton and assembles the full pose. Call only once a frame if possible <3.
        */
        void EvaluateAnimation
            (
            /*Defines the query inputs*/
            int frameIndex,
            /*Main query output*/
            vector<blaPosQuat>& worldJointTransform
            );

        /*
        Queries the local transform of a joint using its name, for a specific frame index.
        */
        blaPosQuat GetLocalTransformByName(std::string name, int frameIndex)
        {
            if (m_jointIndexByName.empty())
            {
                ComputeJointIndexByNameTable(m_skeletonDef);
            }

            return m_jointTransforms[frameIndex][m_jointIndexByName[name]];
        }

        static void GetBoneArrayFromEvalAnim(vector<pair<blaVec3, blaVec3>>& outputBones, const SkeletonJoint* skeleton, vector<blaPosQuat> evalAnim);

    private:

        void ComputeJointIndexByNameTable(SkeletonJoint* skeleton)
        {
            m_jointIndexByName[skeleton->GetName()] = skeleton->GetJointIndex();

            for (auto& child : *skeleton)
            {
                ComputeJointIndexByNameTable(&child);
            }
        }

        static void ForwardKinematicRecursive
            (
            /*Defines the query inputs*/
            int frameIndex,
            /*Defines the recursion parameters */
            SkeletonJoint* joint,
            const blaPosQuat& cumulativeTransform,
            vector<vector<blaPosQuat>>& jointTransforms,
            /*Main query output*/
            vector<blaPosQuat>& worldJointTransforms
            );

        string m_name;

        vector<vector<blaPosQuat>>      m_jointTransforms;

        unordered_map<string, blaU32>   m_jointIndexByName;

        //Todo: Make this a pointer to a valid skeleton Asset...
        SkeletonJoint*		            m_skeletonDef;

        float							m_samplingRate;
        int								m_frameCount;
    };
}
