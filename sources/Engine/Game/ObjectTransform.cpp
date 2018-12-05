#include "ObjectTransform.h"

using namespace BLAengine;

ObjectTransform::ObjectTransform(void) :
m_scale(blaVec3(1.f)),
m_transform(blaPosQuat::GetIdentity())
{};


ObjectTransform::~ObjectTransform(void)
{}

void ObjectTransform::GetScaledTransformMatrix(blaMat4 &outputMatrix) const
{
    outputMatrix = m_transform.ToMat4();

    outputMatrix[0][0] *= m_scale.x;
    outputMatrix[1][1] *= m_scale.y;
    outputMatrix[2][2] *= m_scale.z;
}

void ObjectTransform::SetRotation(blaQuat rotation)
{
    m_transform.SetRotation(rotation);
}

blaQuat ObjectTransform::GetRotation() const
{
    return m_transform.GetRotation();
}

void ObjectTransform::SetPosition(blaVec3 position)
{
    m_transform.SetTranslation(blaVec4(position,1.f));
}

blaVec3 ObjectTransform::GetPosition() const
{
    return blaVec3(m_transform.GetTranslation());
}


void ObjectTransform::SetEulerAngles(float pitch, float yaw, float roll)
{
    m_transform.SetRotation(blaPosQuat::EulerToQuat(pitch, yaw, roll));
}

blaVec3 ObjectTransform::GetEulerAngles() const
{
    blaQuat q = m_transform.GetRotation();

    return blaVec3(pitch(q), yaw(q), roll(q));
}

blaVec3 ObjectTransform::LocalDirectionToWorld(const blaVec3& direction) const
{
    return m_transform.TransformVector(direction);
}

blaVec3 ObjectTransform::LocalPositionToWorld(const blaVec3& position) const
{
    return m_transform.TransformPoint(m_scale * position);
}

blaVec3 ObjectTransform::WorldDirectionToLocal(const blaVec3& direction) const
{
    blaPosQuat inverseTransform = m_transform.GetInverse();

    return inverseTransform.TransformVector(direction);
}

blaVec3 ObjectTransform::WorldPositionToLocal(const blaVec3& position) const
{
    blaPosQuat inverseTransform = m_transform.GetInverse();

    return inverseTransform.TransformPoint(position) / m_scale;
}