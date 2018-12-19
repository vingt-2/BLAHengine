#include <fstream>
#include <iostream>
#include <string.h>
#include <stack>
#include "SkeletonAnimationData.h"

void PrintJointRecursive(SkeletonJoint* joint, int depth)
{
    string out = "";
    for (int i = 0; i < depth; i++)
        out += "_";
    cout << out + joint->GetName() + "\n";

    for (auto child : joint->GetDirectChildren())
        PrintJointRecursive(child, depth + 1);
}

void SkeletonJoint::PrintJoint()
{
    PrintJointRecursive(this, 0);
}

void SkeletonAnimationData::GetBoneArrayFromEvalAnim(vector<pair<blaVec3, blaVec3>>& outputBones,
    const SkeletonJoint* skeleton, vector<blaPosQuat> evalAnim)
{

    blaPosQuat transform = evalAnim[skeleton->GetJointIndex()];

    for (auto child : skeleton->GetDirectChildren())
    {
        outputBones.emplace_back(pair<blaVec3, blaVec3>(transform.GetTranslation3(), evalAnim[child->GetJointIndex()].GetTranslation3()));
        GetBoneArrayFromEvalAnim(outputBones, child, evalAnim);
    }
}

void SkeletonAnimationData::ForwardKinematicRecursive
(
    /*Defines the query inputs*/
    int frameIndex,
    /*Defines the recursion parameters */
    SkeletonJoint* joint,
    const blaPosQuat& parentWorldTransform,
    vector<vector<blaPosQuat>>& localJointTransforms,
    /*Main query output*/
    vector<blaPosQuat>& worldJointTransforms
)
{
    const blaPosQuat worldJointTransform = parentWorldTransform * localJointTransforms[frameIndex][joint->GetJointIndex()];

    worldJointTransforms[joint->GetJointIndex()] = worldJointTransform;

    for (auto child : joint->GetDirectChildren())
    {
        ForwardKinematicRecursive(
            frameIndex,
            child,
            worldJointTransform,
            localJointTransforms,
            worldJointTransforms);
        
    }
}

void SkeletonAnimationData::EvaluateAnimation
(
/*Defines the query inputs*/
int frameIndex,
/*Defines the main query output*/
vector<blaPosQuat>& worldJointTransforms
)
{
    SkeletonJoint* root = m_skeletonDef;

    blaPosQuat cumulativeTransform = blaPosQuat::GetIdentity();

    worldJointTransforms.resize(m_jointTransforms[0].size());

    ForwardKinematicRecursive
    (
        frameIndex,
        m_skeletonDef,
        blaPosQuat::GetIdentity(),
        m_jointTransforms,
        worldJointTransforms
    );
}

void SkeletonJoint::QuerySkeleton(unordered_map<string, SkeletonJoint*>* jointPointersByNames, vector<pair<string, string>>* bonesByJointNames)
{
    // Traverse skeleton recursively and fill out provided containers.

    if (jointPointersByNames)
    {
        if (jointPointersByNames->find(m_name) == jointPointersByNames->end())
            jointPointersByNames->emplace(m_name, this);
    }

    for (auto child : m_childJoints)
    {
        if (bonesByJointNames)
        {
            bonesByJointNames->push_back(pair<string, string>(m_name, child->GetName()));
        }

        child->QuerySkeleton(jointPointersByNames, bonesByJointNames);
    }
}

void SkeletalAnimationPlayer::UpdatePlayer()
{
    float time = 0.f;//m_clock.getTimeSeconds();
    float dt = time - m_lastTime;
    if (dt > 0.1f)
        dt = 0.1f;
    m_lastTime = time;
    if(m_playerState == IS_PLAYING)
    {
        m_currentAnimationFrame += dt * m_motionSpeed * m_skeletalMotion->GetSamplingRate();
        if (m_currentAnimationFrame >= m_skeletalMotion->GetFrameCount())
        {
            m_currentAnimationFrame = m_skeletalMotion->GetFrameCount();
            std::cout << "Animation Ended. Press Enter to start over.\n";
            m_playerState = IS_WAITING_START;
        }
    }
    else if (m_playerState == SHOULD_START)
    {
        m_playerState = IS_PLAYING;
    }
}

void SkeletalAnimationPlayer::UpdatePlayer(bool &bIsAnimationStarting)
{
    bIsAnimationStarting = false;
    if (m_playerState == SHOULD_START)
    {
        UpdatePlayer();
        if (m_playerState == IS_PLAYING)
            bIsAnimationStarting = true;
    }
    else
    {
        UpdatePlayer();
    }
}

SkeletonAnimationData::~SkeletonAnimationData()
{

}