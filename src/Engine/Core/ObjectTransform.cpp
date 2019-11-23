#include "ObjectTransform.h"

using namespace BLAengine;

ObjectTransform::ObjectTransform(void) :
    m_scale(blaVec3(1.f)),
    m_posQuat(blaPosQuat::GetIdentity())
{};


ObjectTransform::~ObjectTransform(void)
{}

void ObjectTransform::GetScaledTransformMatrix(blaMat4 &outputMatrix) const
{
    outputMatrix = m_posQuat.ToMat4();

    outputMatrix[0][0] *= m_scale.x; outputMatrix[0][1] *= m_scale.x; outputMatrix[0][2] *= m_scale.x;
    outputMatrix[1][0] *= m_scale.y; outputMatrix[1][1] *= m_scale.y; outputMatrix[1][2] *= m_scale.y;
    outputMatrix[2][0] *= m_scale.z; outputMatrix[2][1] *= m_scale.z; outputMatrix[2][2] *= m_scale.z;
}

void ObjectTransform::SetRotation(blaQuat rotation)
{
    m_posQuat.SetRotation(rotation);
}

blaQuat ObjectTransform::GetRotation() const
{
    return m_posQuat.GetRotation();
}

void ObjectTransform::SetPosition(blaVec3 position)
{
    m_posQuat.SetTranslation(blaVec4(position, 1.f));
}

blaVec3 ObjectTransform::GetPosition() const
{
    return blaVec3(m_posQuat.GetTranslation());
}

void ObjectTransform::SetEulerAngles(float pitch, float yaw, float roll)
{
    m_posQuat.SetRotation(blaPosQuat::EulerToQuat(pitch, yaw, roll));
}

void ObjectTransform::SetEulerAngles(blaVec3 eulerAngles)
{
    m_posQuat.SetRotation(blaPosQuat::EulerToQuat(eulerAngles.x, eulerAngles.y, eulerAngles.z));
}

blaVec3 ObjectTransform::GetEulerAngles() const
{
    const blaQuat& q = m_posQuat.GetRotation();

    return blaVec3(pitch(q), yaw(q), roll(q));
}

blaVec3 ObjectTransform::LocalDirectionToWorld(const blaVec3& direction) const
{
    return m_posQuat.TransformVector(direction);
}

blaVec3 ObjectTransform::LocalPositionToWorld(const blaVec3& position) const
{
    return m_posQuat.TransformPoint(m_scale * position);
}

blaVec3 ObjectTransform::WorldDirectionToLocal(const blaVec3& direction) const
{
    blaPosQuat inverseTransform = m_posQuat.GetInverse();

    return inverseTransform.TransformVector(direction);
}

blaVec3 ObjectTransform::WorldPositionToLocal(const blaVec3& position) const
{
    blaPosQuat inverseTransform = m_posQuat.GetInverse();

    return inverseTransform.TransformPoint(position) / m_scale;
}