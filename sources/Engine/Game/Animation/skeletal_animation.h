#pragma once
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths/Maths.h"


/*
	Class SkeletonJoint:
	
	This is the tree node that holds information about a specific joint. A skeleton is a tree of these things.
*/
class SkeletonJoint
{
public:

	SkeletonJoint(string name, vector<SkeletonJoint*> childJoints, blaVec3 &localOffset)
	{
		m_name = name;
		m_childJoints = childJoints;
		m_localOffset = localOffset;
	}

	~SkeletonJoint() {};

	string		GetName()			{ return m_name; }
	
	blaVec3		GetLocalOffset()	{ return m_localOffset; }

	void		ApplyOffsetNormalization(float normalizer)	{ m_localOffset /= normalizer; }

	/*
		Returns a vector of pointers to all the direct descendance of this joint.
		An end joint will return a empty vector.
	*/
	vector<SkeletonJoint*> GetDirectChildren()	{ return m_childJoints; }

	/*
		QuerySkeleton:
		Use to retrieve information from the Skeleton without having to do it recursively yourself.
		Careful there... this has to traverse the skeleton, and may therefore be costly!
	*/
	void QuerySkeleton(unordered_map<string, SkeletonJoint*>* jointPointersByNames, vector<pair<string, string>>* bonesByJointNames);

	void PrintJoint();

private:
	string					m_name;
	blaVec3					m_localOffset;
	vector<SkeletonJoint*>	m_childJoints;
};

class SkeletalMotion
{
public:

	SkeletalMotion(
		string name,
		vector<vector<blaVec3>> rootTrajectories,
		unordered_map<string, vector<blaPosQuat>> jointTransforms,
		vector<SkeletonJoint*> skeletonRoots,
		float samplingRate,
		int	  frameCount)
	{
		m_name = name;
		m_rootTrajectories = rootTrajectories;
		m_jointTransforms = jointTransforms;
		m_skeletonRoots = skeletonRoots;
		m_samplingRate = samplingRate;
		m_frameCount = frameCount;
		m_skeletonScale = 1.0f;
	};

	~SkeletalMotion();

	string GetName()		{ return m_name; }

	/*
		Returns the sampling rate in secs as specified in the animation file 
	*/

	float GetSamplingRate() { return m_samplingRate; }
	
	/*
		Returns the length of the animation, in number of frames. Length in time = framecout * sampling_rate
	*/
	int GetFrameCount()		{ return m_frameCount; }

	/*
		Returns the Root joint of the desired skeleton defined in the animation clip.
	*/
	SkeletonJoint* GetRoot(int index){ return m_skeletonRoots[index]; }

	/*
		QuerySkeletalAnimation:
		Use this function to retrieve information about the pose of an animation at a frameIndex, in the required formats.
		Careful there... this traverses the skeleton and assembles the full pose. Call only once a frame if possible <3.
	*/
	void QuerySkeletalAnimation
	(
		/*Defines the query inputs*/
		int frameIndex,
		int skeletonIndex,
		bool addRootOffset,
		/*Defines the query outputs*/
		vector<blaVec3>* jointPositions = NULL,
		unordered_map<string, blaVec3>* jointPositionsByName = NULL,
		vector<pair<blaVec3, blaVec3>>* segmentPositions = NULL,
		unordered_map<string, blaPosQuat>* cumulativeTransformsByName = NULL
	);

	/*
		Compute and set a normalizing scale so that differnt skeleton definition appear at the same scale in the application
	*/
	void SetNormalizedScale();

	/*
		Compute and set a normalizing scale, with a multiplier applied (for visualization)
	*/
	void SetNormalizedScaleWithMultiplier(float scaleCoeff);

	/*
		Sets the scale of positions information retrieved.
	*/
	void SetScale(float scale) { m_skeletonScale = scale; }

	/*
		Queries the local transform of a joint using its name, for a specific frame index.
	*/
	blaPosQuat GetLocalTransformByName(std::string name, int frameIndex) 
	{
		blaPosQuat tr = m_jointTransforms[name][frameIndex];
		return tr;
	}

private:
	string m_name;
	vector<vector<blaVec3>>							m_rootTrajectories;
	unordered_map <string, vector<blaPosQuat>>		m_jointTransforms;

	vector<SkeletonJoint*>			m_skeletonRoots;
	float							m_samplingRate;
	int								m_frameCount;
	float							m_skeletonScale;
public:

	/*
		BVHImport: 
		Allocates a SkeletalMotion object from a bvh file
	*/
	static SkeletalMotion* BVHImport(string bvhFilePath);
};

class SkeletalAnimationPlayer
{
public:

	/*
		Returns the current frame state of the Skeletal Animation player.
		This is essentially what the player outputs, just an index telling you what frame you should setup your skeleton in.
	*/
	int GetCurrentAnimationFrame()
	{
		float result = m_currentAnimationFrame;
		if (m_currentAnimationFrame < 0)
			result = 0;

		if (m_currentAnimationFrame > m_skeletalMotion->GetFrameCount() - 1)
			result = m_skeletalMotion->GetFrameCount() - 1;

		return (int)result;
	}

	void InputKeyPressed(int keyPressed)
	{
		//if (m_clock.getTimeSeconds() - m_lastInputTime < 0.2)
		{
			return;
		}

		if (m_playerState == IS_WAITING_START)
		{
			if (keyPressed == 0)
			{
				m_currentAnimationFrame = 0;
				m_playerState = SHOULD_START;
			}
			else if (keyPressed == 1)
			{
				std::cout << "Frame --\n";
				m_currentAnimationFrame -= 10;
			}
			else if (keyPressed == 2)
			{
				std::cout << "Frame ++\n";
				m_currentAnimationFrame += 10;
			}
		}
		else if (m_playerState == IS_PLAYING)
		{
			if (keyPressed == 0)
			{
				std::cout << "Stoping Player\n";
				m_playerState = IS_STOPED;
			}
			else if (keyPressed == 1)
			{
				std::cout << "Speed / 2\n";
				m_motionSpeed /= 2.0;
			}
			else if (keyPressed == 2)
			{
				std::cout << "Speed * 2\n";
				m_motionSpeed *= 2.0;
			}
		}
		else if (m_playerState == IS_STOPED)
		{
			if (keyPressed == 0)
			{
				std::cout << "Starting Player\n";
				m_playerState = IS_PLAYING;
			}
			else if (keyPressed == 1)
			{
				std::cout << "Frame --\n";
				m_currentAnimationFrame--;
			}
			else if (keyPressed == 2)
			{
				std::cout << "Frame ++\n";
				m_currentAnimationFrame++;
			}
		}
		//m_lastInputTime = m_clock.getTimeSeconds();
	}

	/*
		Call every frame if you want the player state to progress !
	*/
	void UpdatePlayer();
	void UpdatePlayer(bool &bIsAnimationStarting);

	/*
		Set which Skeletal Motion animation we are playing. We need to know about it for things like sampling rate and frame count
	*/
	void SetSkeletalMotion(SkeletalMotion* motion)
	{
		m_currentAnimationFrame = 0;
		m_skeletalMotion = motion;
	}

	SkeletalAnimationPlayer()
	{
		m_playerState = IS_WAITING_START;
		m_motionSpeed = 1;
		//m_lastTime = m_clock.getTimeSeconds();
		//m_lastInputTime = m_clock.getTimeSeconds();
		m_currentAnimationFrame = 0;
		bIsCycleAnimation = true;
	}

	bool bIsCycleAnimation;

private:

	enum AnimationPlayerState
	{
		IS_PLAYING,
		IS_STOPED,
		IS_WAITING_START,
		IS_WAITING_END,
		SHOULD_START
	};

	//btClock m_clock;
	float	m_lastInputTime;
	float	m_currentAnimationFrame;
	float	m_lastTime;
	AnimationPlayerState m_playerState;
	SkeletalMotion*		 m_skeletalMotion;
	float				 m_timeElapsedInAnimation;

	float	m_motionSpeed;
};