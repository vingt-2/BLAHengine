#include "TransformComponent.h"

using namespace BLAengine;

#define SAFE_SIGNED_INC(i) (i < 0x7FFFFFFF) ? i++ : 0;

BeginComponentDescription(TransformComponent)
    Expose(m_localTransform)
EndComponentDescription()

// TODO: Make a binding component that is updated individually as a system to allow const GetTransform

blaScaledTransform TransformComponent::GetTransform()
{
    // Todo: If I don't rewrite this whole mess soon. at least cache the parent ?
    GameObject parent = GetOwnerObject().GetParent(); 
    if (parent.IsValid())
    {
        TransformComponent* parentTransformComponent = parent.GetComponent<TransformComponent>();
        if (parentTransformComponent != nullptr)
        {
            blaS32 parentCounter = parentTransformComponent->m_setCounter;
            if (m_cachedParentSetCounter != parentCounter || m_dirtyCachedTransform)
            {
                const blaScaledTransform parentTransform = parentTransformComponent->GetTransform();
                m_cachedWorldTransform.m_posQuat = parentTransform.m_posQuat * m_localTransform.m_posQuat;
                m_cachedWorldTransform.m_scale = parentTransform.m_scale * m_localTransform.m_scale;
                SAFE_SIGNED_INC(m_setCounter);
                m_cachedParentSetCounter = parentCounter;
                m_dirtyCachedTransform = false;
            }
            return m_cachedWorldTransform;
        }
    }
    return m_localTransform;
}

blaScaledTransform TransformComponent::GetLocalTransform() const
{
    return m_localTransform;
}

void TransformComponent::SetTransform(const blaScaledTransform& transform)
{
    // Todo: If I don't rewrite this whole mess soon. at least cache the parent ?
    GameObject parent = GetOwnerObject().GetParent();
    if (parent.IsValid())
    {
        TransformComponent* parentTransformComponent = parent.GetComponent<TransformComponent>();
        if (parentTransformComponent != nullptr)
        {
            blaScaledTransform& parentT = parentTransformComponent->GetTransform();

            m_localTransform.m_posQuat = parentT.m_posQuat.GetInverse() * transform.m_posQuat;
            m_localTransform.m_scale = transform.m_scale / parentT.m_scale; //TODO: Make this safe.
            m_cachedWorldTransform = transform;
            m_dirtyCachedTransform = false;
            SAFE_SIGNED_INC(m_setCounter);
            return;
        }
    }
    m_localTransform = transform;
    SAFE_SIGNED_INC(m_setCounter)
}

void TransformComponent::SetLocalTransform(const blaScaledTransform& transform)
{
    SAFE_SIGNED_INC(m_setCounter)

    m_localTransform = transform;
    m_dirtyCachedTransform = true;
}