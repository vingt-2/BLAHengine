#include <fstream>
#include <iostream>
#include <string.h>
#include <stack>
#include "skeletal_animation.h"

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

void QuerySkeletalAnimationRecursive
(
/*Defines the recursion parameters */
SkeletonJoint* joint,
blaPosQuat& cumulativeTransform,
unordered_map<string, vector<blaPosQuat>>& jointTransforms,
float skeletonScale,
/*Defines the query inputs*/
int frameIndex,
int skeletonIndex,
/*Defines the query outputs*/
vector<blaVec3>* jointPositions,
unordered_map<string, blaVec3>* jointPositionsByName,
vector<pair<blaVec3, blaVec3>>* segmentPositions,
unordered_map<string, blaPosQuat>* cumulativeTransformsByName
)
{
	blaVec3 jointPositionL = joint->GetLocalOffset();
	blaVec3 jointPositionW = cumulativeTransform * blaVec4(jointPositionL[0], jointPositionL[1], jointPositionL[2], 1) * skeletonScale;

	if (cumulativeTransformsByName)
	{
		if (cumulativeTransformsByName->find(joint->GetName()) == cumulativeTransformsByName->end())
			cumulativeTransformsByName->emplace(joint->GetName(), cumulativeTransform);
	}

	if (jointPositions)
		jointPositions->push_back(jointPositionW);

	if (jointPositionsByName)
	{
		if (jointPositionsByName->find(joint->GetName()) == jointPositionsByName->end())
			jointPositionsByName->emplace(joint->GetName(), jointPositionW);
	}

	blaPosQuat nextCumulativeTransform;
	if (joint->GetDirectChildren().size()) // Leaf joints do not have transforms, let's not try looking for them
		nextCumulativeTransform = cumulativeTransform * jointTransforms[joint->GetName()][frameIndex];

	for (auto child : joint->GetDirectChildren())
	{
		if (segmentPositions)
		{
			blaVec3 childPositionL = child->GetLocalOffset();
			blaVec3 childPositionW = nextCumulativeTransform * blaVec4(childPositionL[0], childPositionL[1], childPositionL[2], 1) * skeletonScale;

			segmentPositions->push_back(pair<blaVec3, blaVec3>(nextCumulativeTransform.GetTranslation() * skeletonScale, childPositionW));
		}

		QuerySkeletalAnimationRecursive(
			child,
			nextCumulativeTransform,
			jointTransforms,
			skeletonScale,
			frameIndex,
			skeletonIndex,
			jointPositions,
			jointPositionsByName,
			segmentPositions,
			cumulativeTransformsByName);
		
	}
}

void SkeletalMotion::QuerySkeletalAnimation
(
/*Defines the query inputs*/
int frameIndex,
int skeletonIndex,
bool addRootOffset,
/*Defines the query outputs*/
vector<blaVec3>* jointPositions,
unordered_map<string, blaVec3>* jointPositionsByName,
vector<pair<blaVec3, blaVec3>>* segmentPositions,
unordered_map<string, blaPosQuat>* cumulativeTransformsByName
)
{
	if (!jointPositions && !jointPositionsByName && !segmentPositions && !cumulativeTransformsByName)
		return;

	SkeletonJoint* root = m_skeletonRoots[skeletonIndex];

	blaPosQuat rootTransform = blaPosQuat::GetIdentity();

	if (addRootOffset)
		rootTransform.SetTranslation3(m_rootTrajectories[frameIndex][skeletonIndex]);

	// Build world pose of the skeleton by traversing the skeleton recursively and fetching joint rotation at the right frame.
	// fills out provided containers while traversing

	QuerySkeletalAnimationRecursive
	(
		root, 
		rootTransform, 
		m_jointTransforms,
		m_skeletonScale,
		frameIndex, 
		skeletonIndex,
		jointPositions,
		jointPositionsByName, 
		segmentPositions,
		cumulativeTransformsByName
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
	if (dt > 0.1)
		dt = 0.1;
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

void SkeletalMotion::SetNormalizedScale()
{
	vector<blaVec3> jointPositions;
	QuerySkeletalAnimation(0, 0, false, &jointPositions, NULL, NULL, NULL);

	float maxLength = 0;
	for (blaVec3 position : jointPositions)
	{
		if (maxLength < position.length())
			maxLength = position.length();
	}

	m_skeletonScale = 1.0f / maxLength;
}

void SkeletalMotion::SetNormalizedScaleWithMultiplier(float scaleCoeff)
{
	vector<blaVec3> jointPositions;
	QuerySkeletalAnimation(0, 0, false, &jointPositions, NULL, NULL, NULL);

	float maxLength = 0;
	for (blaVec3 position : jointPositions)
	{
		if (maxLength < position.length())
			maxLength = position.length();
	}

	m_skeletonScale = scaleCoeff / maxLength;
}

SkeletalMotion::~SkeletalMotion()
{

}