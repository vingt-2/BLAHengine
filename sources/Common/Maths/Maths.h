// Include GLM
#pragma once

#include "../System.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

typedef glm::ivec2 blaIVec2;
typedef glm::vec2 blaVec2;
typedef glm::vec3 blaVec3;
typedef glm::vec4 blaVec4;
typedef glm::mat3 blaMat3;
typedef glm::mat4 blaMat4;

#define M_PI 3.14159265359f
#define P_GRAVITY blaVec3(0.f, -0.9807f, 0.f)

#define MAX_NORMAL_FLOAT 3.4028234664e38
#define MIN_NORMAL_FLOAT -3.4028234664e38
//#define MAX_SUBNORMAL_FLOAT 0x00800000

#define BLA_EPSILON 1.192092896e-07F

typedef glm::quat blaQuat;

//class blaQuat : public glm::quat
//{
//public:
//    blaQuat(blaF32 x, blaF32 y, blaF32 z, blaF32 w)
//    {
//        this->x = x;
//        this->y = y;
//        this->z = z;
//        this->w = w;
//    }
//
//    blaQuat operator* (const blaQuat &otherQuat) const
//    {
//        glm::quat q = static_cast<const glm::quat>(*this) * static_cast<const glm::quat>(otherQuat);
//        return blaQuat(q.x, q.y, q.z, q.w);
//    }
//
//    blaQuat operator* (const float scalar) const
//    {
//        glm::quat q = *this * scalar;
//        return blaQuat(q.x, q.y, q.z, q.w);
//    }
//
//    blaQuat &operator*= (const blaQuat &otherQuat)
//    {
//        return *this = *this * otherQuat;
//    }
//
//    blaQuat operator*= (const float scalar)
//    {
//        return *this = *this * scalar;
//    }
//
//    void Inverse()
//    {
//        const glm::quat q = glm::inverse(*static_cast<glm::quat*>(this));
//        this->x = q.x;
//        this->y = q.y;
//        this->z = q.z;
//        this->w = q.w;
//    }
//
//    void Normalize()
//    {
//        const glm::quat q = glm::normalize(*static_cast<glm::quat*>(this));
//        this->x = q.x;
//        this->y = q.y;
//        this->z = q.z;
//        this->w = q.w;
//    }
//
//    blaQuat GetInverse() const
//    {
//        const glm::quat q = glm::inverse(*static_cast<const glm::quat*>(this));
//        return blaQuat(q.x,q.y,q.z,q.w);
//    }
//
//    blaQuat GetNormalized() const
//    {
//        const glm::quat q = glm::normalize(*static_cast<const glm::quat*>(this));
//        return blaQuat(q.x, q.y, q.z, q.w);
//    }
//
//    static blaQuat GetIdentity()
//    {
//        return blaQuat(0.f,0.f,0.f,1.f); 
//    }
//
//    static blaQuat EulerToQuat(float roll, float yaw, float pitch)
//    {
//        float cyaw = cos(roll * 0.5f);
//        float syaw = sin(roll * 0.5f);
//        float cp = cos(-yaw * 0.5f);
//        float sp = sin(-yaw * 0.5f);
//        float cr = cos(pitch * 0.5f);
//        float sr = sin(pitch * 0.5f);
//        blaQuat q = GetIdentity();
//        q.x = cyaw * cp * cr + syaw * sp * sr;
//        q.y = cyaw * cp * sr - syaw * sp * cr;
//        q.z = syaw * cp * sr + cyaw * sp * cr;
//        q.w = syaw * cp * cr - cyaw * sp * sr;
//
//        return q;
//    }
//};
//
//inline blaQuat operator* (const float scalar, const blaQuat & q)
//{
//    return q * scalar;
//}

class blaPosQuat
{
private:

    blaVec4 m_p;
    blaQuat m_q;

public:    
    blaPosQuat(blaVec4 p, blaQuat q):
        m_p(p), m_q(q)
    {}

    blaPosQuat(blaVec3 p, blaQuat q):
        m_q(q),
        m_p(blaVec4(p[0], p[1], p[2], 1.f))
    {}

    blaPosQuat operator* (const blaPosQuat &otherPosQuat) const
    {
        const blaVec4 newP = glm::rotate(m_q, otherPosQuat.m_p) + m_p;
        return blaPosQuat(newP, m_q*otherPosQuat.m_q);
    }

    blaPosQuat &operator*= (const blaPosQuat &otherPosQuat)
    {
        return (*this = *this * otherPosQuat);
    }

    blaVec4 operator* (const blaVec4 &v) const
    {
        return glm::rotate(m_q, v) + m_p;
    }

    blaPosQuat &operator= (const blaPosQuat &otherPosQuat)
    {
        m_q = otherPosQuat.m_q;
        m_p = otherPosQuat.m_p;
        return *this;
    }

    blaPosQuat GetInverse() const
    {
        blaPosQuat pq = *this;
        pq.Inverse();
        
        return pq;
    }

    blaVec3 TransformVector(const blaVec3& v) const
    {
        return glm::rotate(m_q, v);
    }

    blaVec3 TransformPoint(const blaVec3& v) const
    {
        return glm::rotate(m_q, v) + blaVec3(m_p);
    }

    blaVec4 TransformVector(const blaVec4& v) const
    {
        return glm::rotate(m_q, v);
    }

    blaVec4 TransformPoint(const blaVec4& v) const
    {
        return *this * v;
    }

    blaMat4 ToMat4() const
    {
        blaMat4 mat = toMat4(m_q);
        mat[3][0] = m_p[0];
        mat[3][1] = m_p[1];
        mat[3][2] = m_p[2];
        mat[3][3] = 1.f;

        return mat;
    }

    // TODO: Verify it is correct to invert q then use it...
    void Inverse()
    {
        m_q = glm::inverse(m_q);
        m_p = glm::rotate(m_q, -m_p);
    }

    void NormalizeRotation()
    {
        m_q = glm::normalize(m_q);
    }

    void SetRotation(blaQuat q)
    {
        m_q = q;
    }

    const blaQuat& GetRotation() const
    {
        return m_q;
    }

    blaQuat& GetRotation()
    {
        return m_q;
    }

    void SetTranslation3(const blaVec3& p)
    {
        m_p[0] = p[0];
        m_p[1] = p[1];
        m_p[2] = p[2];
        m_p[3] = 1.0f;
    }

    void SetTranslation(const blaVec4& p)
    {
        m_p = p;
    }

    const blaVec3& GetTranslation3() const
    {
        return blaVec3(m_p[0], m_p[1], m_p[2]);
    }

    const blaVec4& GetTranslation() const
    {
        return m_p;
    }

    blaVec3& GetTranslation3()
    {
        return blaVec3(m_p[0], m_p[1], m_p[2]);
    }

    blaVec4& GetTranslation() 
    {
        return m_p;
    }

    static blaPosQuat GetIdentity()
    {
        return blaPosQuat(blaVec4(0.f, 0.f, 0.f, 1.f), blaQuatIdentity());
    };

    static blaQuat EulerToQuat(float pitch, float yaw, float roll)
    {
        float cyaw = cos(roll * 0.5f);
        float syaw = sin(roll * 0.5f);
        float cp = cos(yaw * 0.5f);
        float sp = sin(yaw * 0.5f);
        float cr = cos(pitch * 0.5f);
        float sr = sin(pitch * 0.5f);

        blaQuat q;
        q.w = cyaw * cp * cr + syaw * sp * sr;
        q.x = cyaw * cp * sr - syaw * sp * cr;
        q.y = syaw * cp * sr + cyaw * sp * cr;
        q.z = syaw * cp * cr - cyaw * sp * sr;
        return q;
    }
    
    //TODO: The order of inputs here is probably wrong
    static blaMat4 EulerToMat3(float pitch, float yaw, float roll)
    {
        float cospitch = cos(pitch);
        float cosyaw = cos(yaw);
        float cosroll = cos(roll);

        float sinpitch = sin(pitch);
        float sinyaw = sin(yaw);
        float sinroll = sin(roll);

        return blaMat3
        (
            blaVec3(cosyaw*cosroll, -cosyaw*sinroll, sinyaw),
            blaVec3(cospitch*sinroll + sinpitch*sinyaw*cosroll, cospitch*cosroll - sinpitch*sinyaw*sinroll, -sinpitch*cosyaw),
            blaVec3(sinpitch*sinroll - cospitch*sinyaw*cosroll, sinpitch*cosroll + cospitch*sinyaw*sinroll, cospitch*cosyaw)
        );
    }

    static blaQuat blaQuatIdentity()
    {
        blaQuat q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = 0.f;
        q.w = 1.f;

        return q;
    }

};

void ComputeQuatDerivative(blaQuat& outQuat, const blaVec3& angularVelocity, const blaQuat& inputQuat);
blaMat3 matrixCross(blaVec3 vector);
void printVector(blaVec3 vec);