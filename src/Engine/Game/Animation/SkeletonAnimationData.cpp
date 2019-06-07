#include <fstream>
#include <iostream>
#include <string.h>
#include <stack>
#include "SkeletonAnimationData.h"

using namespace BLAengine;

void PrintJointRecursive(SkeletonJoint* joint, int depth)
{
    string out = "";
    for (int i = 0; i < depth; i++)
        out += " ";
    cout << out + joint->GetName() + "\n";

    for (auto& child : *joint)
        PrintJointRecursive(&child, depth + 1);
}

void SkeletonJoint::PrintJoint()
{
    PrintJointRecursive(this, 0);
}

void SkeletonJoint::DiscardJointsByName(string subname)
{
    auto child = GetChild();

    if(child != nullptr)
    {
        if(child->GetName().find(subname) != string::npos)
        {
            m_child = child->GetNext();
            delete child;
        }
        
    }
    SkeletonJoint* prevChild = nullptr;

    while(child != nullptr)
    {
        auto nextChild = child->GetNext();
        if(child->GetName().find(subname) != string::npos)
        {
            prevChild->m_next = nextChild;
            delete child;
        }
        else
        {
            child->DiscardJointsByName(subname);
            prevChild = child;
        }

        child = nextChild;
    }
}

void SkeletonAnimationData::GetBoneArrayFromEvalAnim(vector<pair<blaVec3, blaVec3>>& outputBones,
    const SkeletonJoint* skeleton, vector<blaPosQuat> evalAnim)
{
    blaPosQuat transform = evalAnim[skeleton->GetJointIndex()];

    for (auto& child : *skeleton)
    {
        outputBones.emplace_back(pair<blaVec3, blaVec3>(transform.GetTranslation3(), evalAnim[child.GetJointIndex()].GetTranslation3()));
        GetBoneArrayFromEvalAnim(outputBones, &child, evalAnim);
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

    for (auto &child : *joint)
    {
        ForwardKinematicRecursive(
            frameIndex,
            &child,
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

    for (auto child : *this)
    {
        if (bonesByJointNames)
        {
            bonesByJointNames->push_back(pair<string, string>(m_name, child.GetName()));
        }

        child.QuerySkeleton(jointPointersByNames, bonesByJointNames);
    }
}

SkeletonAnimationData::~SkeletonAnimationData()
{

}