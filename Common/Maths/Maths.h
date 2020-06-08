// Include GLM
#pragma once

#include "System.h"
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
#define RAD_TO_DEG (360.f / M_PI)
#define DEG_TO_RAD (M_PI / 360.f)
#define M_TWO_PI 6.28318530718f
#define P_GRAVITY blaVec3(0.f, -0.9807f, 0.f)

#define MAX_NORMAL_FLOAT 1e35f //3.4028234664e38
#define MIN_NORMAL_FLOAT -1e35f //-3.4028234664e38
//#define MAX_SUBNORMAL_FLOAT 0x00800000

#define BLA_EPSILON 1.192092896e-07F

typedef glm::quat blaQuat;

inline blaMat3 matrixCross(blaVec3 v)
{
    return blaMat3
    (
        blaVec3(0, -v[2], v[1]),
        blaVec3(v[2], 0, -v[0]),
        blaVec3(-v[1], v[0], 0)
    );
}

inline void QuatDerivative(blaQuat& outQuat, const blaVec3& angularVelocity, const blaQuat& inputQuat)
{
    outQuat = 0.5f * inputQuat * blaQuat(angularVelocity[0], angularVelocity[1], angularVelocity[2], 0.f);
}
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
    blaPosQuat(blaVec4 p, blaQuat q) :
        m_p(p), m_q(q)
    {}

    blaPosQuat(blaVec3 p, blaQuat q) :
        m_q(q),
        m_p(blaVec4(p[0], p[1], p[2], 1.f))
    {}

    blaPosQuat()
    {
        *this = GetIdentity();
    }

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

    blaVec3 GetTranslation3() const
    {
        return blaVec3(m_p[0], m_p[1], m_p[2]);
    }

    const blaVec4& GetTranslation() const
    {
        return m_p;
    }

    blaVec4& GetTranslation()
    {
        return m_p;
    }

    static blaPosQuat GetIdentity()
    {
        return blaPosQuat(blaVec4(0.f, 0.f, 0.f, 1.f), QuatIdentity());
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
        q.x = cyaw * cp * sr - syaw * sp * cr;
        q.y = syaw * cp * sr + cyaw * sp * cr;
        q.z = syaw * cp * cr - cyaw * sp * sr;
        q.w = cyaw * cp * cr + syaw * sp * sr;
        return q;
    }

    static blaVec3 QuatToEuler(blaQuat q)
    {
        blaVec3 angles;

        float sinp = 2 * (q.w * q.y - q.z * q.x);
        if (std::abs(sinp) >= 1)
            angles.x = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
        else
            angles.x = std::asin(sinp);

        float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
        float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
        angles.y = std::atan2(sinr_cosp, cosr_cosp);

        float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
        float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
        angles.z = std::atan2(siny_cosp, cosy_cosp);

        return angles;
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
            blaVec3(cosyaw*cosroll, -cosyaw * sinroll, sinyaw),
            blaVec3(cospitch*sinroll + sinpitch * sinyaw*cosroll, cospitch*cosroll - sinpitch * sinyaw*sinroll, -sinpitch * cosyaw),
            blaVec3(sinpitch*sinroll - cospitch * sinyaw*cosroll, sinpitch*cosroll + cospitch * sinyaw*sinroll, cospitch*cosyaw)
        );
    }

    static blaQuat QuatIdentity()
    {
        blaQuat q;
        q.x = 0.f;
        q.y = 0.f;
        q.z = 0.f;
        q.w = 1.f;

        return q;
    }

};

blaQuat inline QuatBetweenVectors(const blaVec3& u, const blaVec3& v)
{
    blaVec3 w = cross(u, v);
    blaQuat q = blaQuat(glm::dot(u, v), w.x, w.y, w.z);
    q.w += glm::length(q);
    return normalize(q);

}

inline blaVec3 SafeNormalize(const blaVec3& v)
{
    blaF32 vLength = glm::length(v);
    if (vLength < BLA_EPSILON)
    {
        return blaVec3(0.f);
    }
    return v / vLength;
}

inline blaVec4 SafeNormalize(const blaVec4& v)
{
    blaF32 vLength = glm::length(v);
    if (vLength < BLA_EPSILON)
    {
        return blaVec4(0.f);
    }
    return v / vLength;
}

struct blaScaledTransform
{
    blaScaledTransform() :
        m_scale(blaVec3(1.f)),
        m_posQuat(blaPosQuat::GetIdentity())
    {
    }

    blaScaledTransform(blaVec3 scale, blaPosQuat posQuat) :
        m_scale(scale),
        m_posQuat(posQuat)
    {
    }

    void QuatDerivative(blaQuat& outQuat, const blaVec3& angularVelocity, const blaQuat& inputQuat)
    {
        outQuat = 0.5f * inputQuat * blaQuat(angularVelocity[0], angularVelocity[1], angularVelocity[2], 0.f);
    }

    void GetScaledTransformMatrix(blaMat4& outputMatrix) const
    {
        outputMatrix = m_posQuat.ToMat4();

        outputMatrix[0][0] *= m_scale.x; outputMatrix[0][1] *= m_scale.x; outputMatrix[0][2] *= m_scale.x;
        outputMatrix[1][0] *= m_scale.y; outputMatrix[1][1] *= m_scale.y; outputMatrix[1][2] *= m_scale.y;
        outputMatrix[2][0] *= m_scale.z; outputMatrix[2][1] *= m_scale.z; outputMatrix[2][2] *= m_scale.z;
    }

    void SetRotation(blaQuat rotation)
    {
        m_posQuat.SetRotation(rotation);
    }

    blaQuat GetRotation() const
    {
        return m_posQuat.GetRotation();
    }

    void SetPosition(blaVec3 position)
    {
        m_posQuat.SetTranslation(blaVec4(position, 1.f));
    }

    blaVec3 GetPosition() const
    {
        return blaVec3(m_posQuat.GetTranslation());
    }

    void SetEulerAngles(float pitch, float yaw, float roll)
    {
        m_posQuat.SetRotation(blaPosQuat::EulerToQuat(pitch, yaw, roll));
    }

    void SetEulerAngles(blaVec3 eulerAngles)
    {
        m_posQuat.SetRotation(blaPosQuat::EulerToQuat(eulerAngles.x, eulerAngles.y, eulerAngles.z));
    }

    blaVec3 GetEulerAngles() const
    {
        const blaQuat& q = m_posQuat.GetRotation();

        return blaVec3(pitch(q), yaw(q), roll(q));
    }

    blaVec3 GetScale() const
    {
        return m_scale;
    }

    blaVec3 LocalDirectionToWorld(const blaVec3& direction) const
    {
        return m_posQuat.TransformVector(direction);
    }

    blaVec3 LocalPositionToWorld(const blaVec3& position) const
    {
        return m_posQuat.TransformPoint(m_scale * position);
    }

    blaVec3 WorldDirectionToLocal(const blaVec3& direction) const
    {
        blaPosQuat inverseTransform = m_posQuat.GetInverse();

        return inverseTransform.TransformVector(direction);
    }

    blaVec3 WorldPositionToLocal(const blaVec3& position) const
    {
        blaPosQuat inverseTransform = m_posQuat.GetInverse();

        return inverseTransform.TransformPoint(position) / m_scale;
    }

    blaPosQuat& GetPosQuat() { return m_posQuat; };
    const blaPosQuat& GetPosQuat() const { return m_posQuat; };

    blaVec3 m_scale;
    blaPosQuat m_posQuat;
};