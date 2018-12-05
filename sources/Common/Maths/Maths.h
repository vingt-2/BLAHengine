// Include GLM
#pragma once

#include "../System.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

typedef glm::vec3 blaVec3;
typedef glm::vec4 blaVec4;
typedef glm::mat3 blaMat3;
typedef glm::mat4 blaMat4;
typedef glm::quat blaQuat;

#define M_PI 3.14159265359f
#define P_GRAVITY blaVec3(0.f, -0.9807f, 0.f)

#define MAX_NORMAL_FLOAT 3.4028234664e38
#define MIN_NORMAL_FLOAT -3.4028234664e38
//#define MAX_SUBNORMAL_FLOAT 0x00800000

class blaPosQuat
{
private:

    blaVec4 m_p;
    blaQuat m_q;

public:    
    blaPosQuat(blaVec4 p, blaQuat q)
    {
        m_p = p;
        m_q = q;
    }

    blaPosQuat(blaVec3 p, blaQuat q)
    {
        m_p = blaVec4(p[0], p[1], p[2], 1.f);
        m_q = q;
    }

    blaPosQuat operator* (const blaPosQuat &otherPosQuat) const
    {
        blaVec4 newP = glm::rotate(m_q, otherPosQuat.m_p) + m_p;
        return blaPosQuat(newP, m_q*otherPosQuat.m_q);
    }

    blaVec4 operator* (const blaVec4 &v) const
    {
        return glm::rotate(m_q, v) + m_p;
    }

    blaPosQuat &operator*= (const blaPosQuat &otherPosQuat)
    {
        return (*this = *this * otherPosQuat);
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

    blaQuat GetRotation() const
    {
        return m_q;
    }


    void SetTranslation(blaVec4 p)
    {
        m_p = p;
    }

    blaVec4 GetTranslation() const
    {
        return m_p;
    }

    static blaPosQuat GetIdentity()
    {
        blaQuat q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = 0.f;
        q.w = 1.f;
        return blaPosQuat(blaVec4(0.f, 0.f, 0.f, 1.f), q);
    };
};

blaMat3 matrixCross(blaVec3 vector);
void printVector(blaVec3 vec);