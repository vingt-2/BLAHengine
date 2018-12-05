#include "ObjectTransform.h"

using namespace BLAengine;

ObjectTransform::ObjectTransform(void) :
m_scale(blaVec3(1.f)),
m_transform(blaPosQuat::GetIdentity())
{};


ObjectTransform::~ObjectTransform(void)
{}

blaMat4 ObjectTransform::GetScaledTransformMatrix() const
{
    blaMat4 sizeMatrix = blaMat4
        (
        blaVec4(m_scale.x, 0, 0, 0),
        blaVec4(0, m_scale.y, 0, 0),
        blaVec4(0, 0, m_scale.z, 0),
        blaVec4(0, 0, 0, 1)
        );

    return m_transform.ToMat4() * sizeMatrix;
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
    const blaMat3 rotationMatrix = blaMat3
    (
        blaVec3(cos(yaw)*cos(roll), -cos(yaw)*sin(roll), sin(yaw)),
        blaVec3(cos(pitch)*sin(roll) + sin(pitch)*sin(yaw)*cos(roll), cos(pitch)*cos(roll) - sin(pitch)*sin(yaw)*sin(roll), -sin(pitch)*cos(yaw)),
        blaVec3(sin(pitch)*sin(roll) - cos(pitch)*sin(yaw)*cos(roll), sin(pitch)*cos(roll) + cos(pitch)*sin(yaw)*sin(roll), cos(pitch)*cos(yaw))
    );

    m_transform.SetRotation(blaQuat(rotationMatrix));
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