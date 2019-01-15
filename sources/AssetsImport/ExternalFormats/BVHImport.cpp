#include <fstream>
#include <iostream>
#include <string.h>

#include "BVHImport.h"


#define _HAS_ITERATOR_DEBUGGING 0

#ifndef M_PI
#define M_PI       float(3.14159265358979323846)
#endif

using namespace std;
using namespace BLAengine;

//TODO: Extract asset data to the Asset folder out of Engine/Game
#include "../../Engine/Game/Animation/SkeletonAnimationData.h"

void InvalidBVH()
{
    cout << "There were invalid values encountered in your BVH file.\n";
}

#define INVALID_BVH_IN_SKEL_DEF \
    InvalidBVH(); \
    return nullptr;

#define INVALID_BVH_IN_DATA_DEF \
    InvalidBVH(); \
    return vector<SkeletonAnimationData*>();

// Helper that gets Which rotation matrix we should output for each axis
blaVec3 GetTranslationForChannel(int axis, float offset);
blaQuat GetRotationForChannel(int axis, float angle);
// Helper that turns a bunch of string parameters from a bvh to int values
int ChannelOrderToInt(string str);

// Turns a big string file into a big bunch of tokens (splits with special characters)
void tokenize(vector<string>& tokens, string str)
{
    string token = "";
    for (int i = 0; i < str.length(); i++)
    {
        if (i == str.length() - 1)
        {
            char a = str[i];
        }

        if (str[i] == '\0' || str[i] == '\n' || str[i] == ' ' || str[i] == '\r' || str[i] == '\t' || str[i] < 0)
        {
            if (token.length() > 0)
                tokens.push_back(token);

            token = "";
        }
        else
        {
            token += str[i];
        }
    }
}

// See BVHImport for explanation
SkeletonJoint* ParseJoint(vector<string> &tokens, vector<vector<int>>& jointChannelsOrderings, int& currentToken, int& jointIndex)
{
    string jointName = tokens[currentToken];

    if (tokens[++currentToken].compare("{"))
    {
        INVALID_BVH_IN_SKEL_DEF
    }

    if (tokens[++currentToken].compare("OFFSET"))
    {
        INVALID_BVH_IN_SKEL_DEF
    }

    blaVec3 jointLocalOffset;
    jointLocalOffset.x = (float) atof(tokens[++currentToken].c_str());
    jointLocalOffset.y = (float) atof(tokens[++currentToken].c_str());
    jointLocalOffset.z = (float) atof(tokens[++currentToken].c_str());


    vector<SkeletonJoint*> jointChildren;

    // Next token should say "Channels". If it don't, we got a problem ...
    if (tokens[++currentToken].compare("CHANNELS"))
    {
        INVALID_BVH_IN_SKEL_DEF
    }

    //Next token is the numbero of data channel for this joint
    int dataChannels = atoi(tokens[++currentToken].c_str());

    vector<int> channelOrderForJoint(dataChannels);
    for (int i = 0; i < dataChannels; ++i)
    {
        channelOrderForJoint[i] = ChannelOrderToInt(tokens[++currentToken]);
    }

    jointChannelsOrderings.push_back(channelOrderForJoint);
 
    // Now to read the child joints ...

    const blaU32 currentJointIndex = jointIndex;

    while (tokens[++currentToken].compare("}"))
    {
        if (!tokens[currentToken].compare("JOINT"))
        {
            SkeletonJoint* childJoint = ParseJoint(tokens, jointChannelsOrderings, ++currentToken, ++jointIndex);

            if (childJoint)
            {
                jointChildren.push_back(childJoint);
            }
        }
        else if (!tokens[currentToken].compare("End") && !tokens[currentToken + 1].compare("Site"))
        {
            if (!tokens[currentToken + 2].compare("{") && !tokens[currentToken + 3].compare("OFFSET") && !tokens[currentToken + 7].compare("}"))
            {
                string endJointName = jointName + "_end";

                blaVec3 endJointLocalOffset;
                endJointLocalOffset.x = (float)atof(tokens[currentToken + 4].c_str());
                endJointLocalOffset.y = (float)atof(tokens[currentToken + 5].c_str());
                endJointLocalOffset.z = (float)atof(tokens[currentToken + 6].c_str());

                currentToken += 7;

                SkeletonJoint* endJoint = new SkeletonJoint(endJointName, endJointLocalOffset, currentJointIndex);
                jointChildren.push_back(endJoint);
            }
            else
            {
                INVALID_BVH_IN_SKEL_DEF
            }
        }
        else
        {
            INVALID_BVH_IN_SKEL_DEF
        }
    }

    auto joint =  new SkeletonJoint(jointName, jointLocalOffset, currentJointIndex);

    for(auto c : jointChildren)
    {
        joint->AddChild(c);
    }

    return joint;
}

// See BVHImport for explanation
void ReadFrameRecursive(vector<string>& tokens,
                        vector<blaPosQuat>& jointTransforms,
                        SkeletonJoint* joint,
                        int &currentToken,
                        vector<vector<int>> &jointsChannelOrderings)
{
    if (joint->GetChild() == nullptr)
        return;

    blaPosQuat localJointTransform(joint->GetLocalOffset(), blaPosQuat::QuatIdentity());

    // Read the data for that joint !
    const blaU32 currentJointIdx = joint->GetJointIndex();
    for (int c = 0; c < jointsChannelOrderings[currentJointIdx].size(); ++c)
    {
        const int channel = jointsChannelOrderings[currentJointIdx][c];

        if (channel < 3)
        {
            localJointTransform.SetTranslation3(localJointTransform.GetTranslation3() +  
                GetTranslationForChannel(channel, atof(tokens[++currentToken].c_str())));
        }
        else
        {
            localJointTransform.GetRotation() *= GetRotationForChannel(channel, atof(tokens[++currentToken].c_str()));
        }
    }

    jointTransforms.push_back(localJointTransform);

    for (auto& child : *joint)
    {
        ReadFrameRecursive(tokens, jointTransforms, &child, currentToken, jointsChannelOrderings);
    }
}

/*
    BVH Imports:

    1) Opens the file.
    2) Calls a recursive joint parser to parse the tree structure
    3) for each frame, calls a recursive frame data reader that recusrively populates 
        the tree's joints data and advances in the frames data block.
    4) Profit. Returns a null pointer if there were any issue parsing the data.
*/
vector<SkeletonAnimationData*> BVHImport::ImportAnimation(string bvhFilePath)
{
    ifstream bvhFile(bvhFilePath, std::ifstream::binary);

    vector<string> tokens;
    if (bvhFile.is_open())
    {
        // get length of file:
        bvhFile.seekg(0, bvhFile.end);
        int length = bvhFile.tellg();
        bvhFile.seekg(0, bvhFile.beg);

        char * buffer = new char[length];

        // read data as a block:
        bvhFile.read(buffer, length);

        if (!bvhFile)
        {
            bvhFile.close();

            std::cout << "Could not open " << bvhFilePath << "\n";
        }
        bvhFile.close();

        tokenize(tokens, string(buffer));

        delete[] buffer;
    }


    if (!tokens.size())
    {
        INVALID_BVH_IN_DATA_DEF
    }

    int currentToken = -1;
    
    vector<SkeletonJoint*>		skeletalRoots;
    vector<vector<int>>			jointChannelsOrderings;
    int jointIndex = 0;
    while (tokens[++currentToken] != "MOTION")
    {

        if (tokens[currentToken].compare("HIERARCHY"))
        {
            INVALID_BVH_IN_DATA_DEF
        }

        if (!tokens[++currentToken].compare("ROOT"))
        {
            vector<int> rootOrdering;
            SkeletonJoint* rootJoint = ParseJoint(tokens, jointChannelsOrderings, ++currentToken, jointIndex);

            if (rootJoint)
                skeletalRoots.push_back(rootJoint);
        }
    }

    // Print the Skeleton to the console
    std::cout << "Loaded the following Skeleton(s): \n\n";
    for (auto roots : skeletalRoots)
    {
        roots->PrintJoint();
    }

    if (tokens[++currentToken].compare("Frames:"))
    {
        INVALID_BVH_IN_DATA_DEF
    }

    int frameCount = atoi(tokens[++currentToken].c_str());

    if(tokens[++currentToken].compare("Frame") || tokens[++currentToken].compare("Time:"))
    {
        INVALID_BVH_IN_DATA_DEF
    }

    float frameTime = atof(tokens[++currentToken].c_str());

    vector<vector<vector<blaPosQuat>>>	jointTransformsPerFramePerAnimationInBVH(skeletalRoots.size());

    // Frame and multiple skeleton are not presented how we want in the file so We'll just build our result vector later
    for (int frame = 0; frame < frameCount; frame++)
    {
        for (int rootIndex = 0; rootIndex < skeletalRoots.size(); rootIndex++)
        {
            // Add a new data frame in the result...
            jointTransformsPerFramePerAnimationInBVH[rootIndex].push_back(vector<blaPosQuat>());

            SkeletonJoint* root = skeletalRoots[rootIndex];

            // Call Read Frame Recursive which will fill up the newly added dataframe!
            ReadFrameRecursive(tokens, jointTransformsPerFramePerAnimationInBVH[rootIndex][frame], root, currentToken, jointChannelsOrderings);
        }
    }
    ++currentToken;
    if (currentToken != tokens.size())
    {
        INVALID_BVH_IN_DATA_DEF
    }

    //Todo: Really don't like that the importer in in charge of allocating the data here ...
    vector<SkeletonAnimationData*> results;
    for (int rootIndex = 0; rootIndex < skeletalRoots.size(); ++rootIndex)
    {
        SkeletonAnimationData* skeletonAnimation = new SkeletonAnimationData(bvhFilePath, jointTransformsPerFramePerAnimationInBVH[rootIndex], skeletalRoots[rootIndex], 1.0f / frameTime, frameCount);
        results.push_back(skeletonAnimation);
    }

    return results;
}

// Gets Which rotation matrix we should output for each axis
blaQuat GetRotationForChannel(int axis, float angle)
{
    angle *= M_PI / 180.0f;

    if (axis == 3)
    {
        return blaPosQuat::EulerToQuat(angle, 0.f, 0.f);
    }
    else if (axis == 4)
    {
        return blaPosQuat::EulerToQuat(0.f, angle, 0.f);
    }
    else if (axis == 5)
    {
        return blaPosQuat::EulerToQuat(0.f, 0.f, angle);
    }
    else
    {
        //std::cout << "Warning: Invalid rotation axis provided in bvh file...\n";
        return blaQuat(0.f,0.f,0.f,1.f);
    }
}

// Gets Which rotation matrix we should output for each axis
blaVec3 GetTranslationForChannel(int axis, float offset)
{
    blaVec3 vec(0.f);
    if (axis < 0 || axis > 2)
    {
        return vec;
    }

    vec[axis] = offset;

    return vec;
}

// Helper that turns a bunch of string parameters from a bvh to int values
int ChannelOrderToInt(string str)
{
    if (!str.compare("Xposition"))
    {
        return 0;
    }
    if (!str.compare("Yposition"))
    {
        return 1;
    }
    if (!str.compare("Zposition"))
    {
        return 2;
    }
    if (!str.compare("Xrotation"))
    {
        return 3;
    }
    if (!str.compare("Yrotation"))
    {
        return 4;
    }
    if (!str.compare("Zrotation"))
    {
        return 5;
    }

    return -1;
}