#pragma once

#include "StdInclude.h"
#include "System.h"

#define BLA_DECLARE_REFERENCE(REFERENCE_NAME, REFERENCE_TYPE)               \
    public:                                                                 \
            REFERENCE_NAME() : BLAReference<REFERENCE_TYPE>() {}            \
            static REFERENCE_NAME InvalidReference()                        \
                { return REFERENCE_NAME(); }                                \
    private:                                                                \
        REFERENCE_NAME(blaIndex idx, std::vector<REFERENCE_TYPE>* pVector) :\
            BLAReference<REFERENCE_TYPE>(idx, pVector) {}                   \
        
template<class Object>
class BLAReference
{
public:
    BLAReference() : m_objectIndex(BLA_INVALID_INDEX), m_pGameObjectVector(nullptr)
    {}

    Object& GetObject() const { return m_pGameObjectVector->at(m_objectIndex); }
    blaBool IsValid() const { return m_objectIndex != BLA_INVALID_INDEX; }

    Object* operator->() const { return &(m_pGameObjectVector->at(m_objectIndex)); }

    blaBool operator==(const BLAReference& other) const
    {
        return (m_objectIndex == other.m_objectIndex) && (m_pGameObjectVector == other.m_pGameObjectVector);
    }

    blaBool operator!=(const BLAReference& other) const
    {
        return !(*this == other);
    }

    static BLAReference InvalidReference() { return BLAReference(); };
protected:
    BLAReference(blaIndex idx, std::vector<Object>* pVector) : m_objectIndex(idx), m_pGameObjectVector(pVector)
    {}

    blaIndex m_objectIndex;
    std::vector<Object>* m_pGameObjectVector;
};