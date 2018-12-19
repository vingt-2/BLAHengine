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

void SkeletonAnimationData:: ForwardKinematicQueryRecursive
(
    /*Defines the recursion parameters */
    SkeletonJoint* joint,
    const blaPosQuat& parentWorldTransform,
    vector<vector<blaPosQuat>>& localJointTransforms,
    float skeletonScale,
    /*Defines the query inputs*/
    int frameIndex,
    /*Main query output*/
    vector<blaPosQuat>* worldJointTransforms,
    /*Optional query output*/
    vector<blaVec3>* jointPositions,
    unordered_map<string, blaVec3>* jointPositionsByName,
    vector<pair<blaVec3, blaVec3>>* segmentPositions
)
{
    blaPosQuat worldJointTransform = parentWorldTransform * localJointTransforms[frameIndex][joint->GetJointIndex()];

    if (worldJointTransforms)
    {
        (*worldJointTransforms)[joint->GetJointIndex()] = worldJointTransform;
    }

    if (jointPositions)
    {
        jointPositions->push_back(worldJointTransform.GetTranslation3());
    }

    if (jointPositionsByName)
    {
        if (jointPositionsByName->find(joint->GetName()) == jointPositionsByName->end())
            jointPositionsByName->emplace(joint->GetName(), worldJointTransform.GetTranslation3());
    }

    for (auto child : joint->GetDirectChildren())
    {
        if (segmentPositions)
        {
            blaPosQuat nextJointTransform = blaPosQuat::GetIdentity();
            nextJointTransform = worldJointTransform * localJointTransforms[frameIndex][child->GetJointIndex()];

            segmentPositions->push_back(pair<blaVec3, blaVec3>(worldJointTransform.GetTranslation3(), nextJointTransform.GetTranslation3())); //* skeletonScale, childPositionW));
        }

        ForwardKinematicQueryRecursive(
            child,
            worldJointTransform,
            localJointTransforms,
            skeletonScale,
            frameIndex,
            worldJointTransforms,
            jointPositions,
            jointPositionsByName,
            segmentPositions);
        
    }
}

void SkeletonAnimationData::ForwardKinematicQuery
(
/*Defines the query inputs*/
int frameIndex,
/*Defines the main query output*/
vector<blaPosQuat>* worldJointTransforms,
/*Defines the query outputs*/
vector<blaVec3>* jointPositions,
unordered_map<string, blaVec3>* jointPositionsByName,
vector<pair<blaVec3, blaVec3>>* segmentPositions
)
{
    if (!jointPositions && !jointPositionsByName && !segmentPositions && !worldJointTransforms)
        return;

    SkeletonJoint* root = m_skeletonDef;

    blaPosQuat cumulativeTransform = blaPosQuat::GetIdentity();

    // Build world pose of the skeleton by traversing the skeleton recursively and fetching joint rotation at the right frame.
    // fills out provided containers while traversing

    if (worldJointTransforms)
    {
        blaPosQuat a = blaPosQuat::GetIdentity();
        worldJointTransforms->resize(m_jointTransforms[0].size());
    }
    
    ForwardKinematicQueryRecursive
    (
        root, 
        cumulativeTransform,
        m_jointTransforms,
        m_skeletonScale,
        frameIndex, 
        worldJointTransforms,
        jointPositions,
        jointPositionsByName, 
        segmentPositions
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

void SkeletonAnimationData::SetNormalizedScale()
{
    vector<blaVec3> jointPositions;
    ForwardKinematicQuery(0, NULL, &jointPositions, NULL, NULL);

    float maxLength = 0;
    for (blaVec3 position : jointPositions)
    {
        if (maxLength < glm::length(position))
            maxLength = glm::length(position);
    }

    m_skeletonScale = 1.0f / maxLength;
}

void SkeletonAnimationData::SetNormalizedScaleWithMultiplier(float scaleCoeff)
{
    vector<blaVec3> jointPositions;
    ForwardKinematicQuery(0, NULL, &jointPositions, NULL, NULL);

    float maxLength = 0;
    for (blaVec3 position : jointPositions)
    {
        if (maxLength < glm::length(position))
            maxLength = glm::length(position);
    }

    m_skeletonScale = scaleCoeff / maxLength;
}

SkeletonAnimationData::~SkeletonAnimationData()
{

}