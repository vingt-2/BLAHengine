#include "Maths.h"
#include <string>

blaMat3 matrixCross(blaVec3 v)
{
    return blaMat3
    (
        blaVec3(0, -v[2], v[1]),
        blaVec3(v[2], 0, -v[0]),
        blaVec3(-v[1], v[0], 0)
    );
}

std::string VecToString(blaVec3 vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}

std::string VecToString(blaVec2 vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}

std::string VecToString(blaIVec2 vec)
{
	return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}

void ComputeQuatDerivative(blaQuat& outQuat, const blaVec3& angularVelocity, const blaQuat& inputQuat)
{
    outQuat = 0.5f * inputQuat * blaQuat(angularVelocity[0], angularVelocity[1], angularVelocity[2], 0.f);
}