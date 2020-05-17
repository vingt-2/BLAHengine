#include "InspectableVariables.h"
#include "Core/GameObject.h"
#include "Maths/Maths.h"
#include "System.h"
#include "System/Console.h"

// We're going to do some serialization work here for now
// Todo: Move serialization out of here.
#include "External/rapidjson/prettywriter.h"

using namespace BLA;
using namespace BLAInspectableVariables;


class BLASerializeWriter : public rapidjson::PrettyWriter<rapidjson::StringBuffer>
{};

struct BoolDescriptor : ExposedVarTypeDescriptor
{
    BoolDescriptor() : ExposedVarTypeDescriptor{ BlaStringId("bool"), sizeof(bool) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->Bool(*(bool*)obj);
    }

    struct TypedDeserializer: Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* Bool(bool b) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<bool*>(m_obj) = b;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 1;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<bool>()
{
    static BoolDescriptor typeDesc;
    return &typeDesc;
}

struct F32Descriptor : ExposedVarTypeDescriptor
{
    F32Descriptor() : ExposedVarTypeDescriptor{ BlaStringId("F32"), sizeof(blaF32) }
    {
    }

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->Double(*(blaF32*)obj);
    }

    struct TypedDeserializer: Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* Null() override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaF32*>(m_obj) = 0.f;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* Double(double d) override
        {
            float f = (float)d;
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaF32*>(m_obj) = f;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 1;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaF32>()
{
    static F32Descriptor typeDesc;
    return &typeDesc;
}


struct S32Descriptor : ExposedVarTypeDescriptor
{
    S32Descriptor() : ExposedVarTypeDescriptor{ BlaStringId("S32"), sizeof(blaS32) }
    {
    }

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->Int(*(blaS32*)obj);
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* Int(int i) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaS32*>(m_obj) = i;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* Uint(unsigned i) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaS32*>(m_obj) = i;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 1;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaS32>()
{
    static S32Descriptor typeDesc;
    return &typeDesc;
}

struct StringDescriptor : ExposedVarTypeDescriptor
{
    StringDescriptor() : ExposedVarTypeDescriptor{ BlaStringId("String"), sizeof(blaString) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->String(static_cast<blaString*>(obj)->c_str());
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* String(const char* str, size_t length, bool copy) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaString*>(m_obj) = blaString(str, length);
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 1;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaString>()
{
    static StringDescriptor typeDesc;
    return &typeDesc;
}

struct GameObjectReferenceDescriptor : ExposedVarTypeDescriptor
{
    GameObjectReferenceDescriptor() : ExposedVarTypeDescriptor{ BlaStringId("GameObject"), sizeof(GameObject) }
    {
    }

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->String(static_cast<GameObject*>(obj)->GetId());
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* String(const char* str, size_t length, bool copy) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<GameObject*>(m_obj) = GameObject(GenerateBlaStringId(blaString(str, length)));
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 1;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<GameObject>()
{
    static GameObjectReferenceDescriptor typeDesc;
    return &typeDesc;
}

struct blaVec2Descriptor : ExposedVarTypeDescriptor
{
    blaVec2Descriptor() : ExposedVarTypeDescriptor{ BlaStringId("Vector2"), sizeof(blaVec2) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaVec2* vec2 = static_cast<blaVec2*>(obj);
        writer->Double(vec2->x);
        writer->Double(vec2->y);
        writer->EndArray();
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        Deserializer* Double(double bd) override
        {
            float b = (float)bd;
            if(m_stage == 1) 
            {
                static_cast<blaVec2*>(m_obj)->x = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 2)
            {
                static_cast<blaVec2*>(m_obj)->y = b;
                ++m_stage; return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* EndArray(size_t count) override
        {
            if (m_stage == 3 && count == 2)
            {
                m_stage++;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 0;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaVec2>()
{
    static blaVec2Descriptor typeDesc;
    return &typeDesc;
}

struct blaVec3Descriptor : ExposedVarTypeDescriptor
{
    blaVec3Descriptor() : ExposedVarTypeDescriptor{ BlaStringId("Vector3"), sizeof(blaVec3) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaVec3* vec3 = static_cast<blaVec3*>(obj);
        writer->Double(vec3->x);
        writer->Double(vec3->y);
        writer->Double(vec3->z);
        writer->EndArray();
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        Deserializer* Double(double d) override
        {
            float f = (float)d;
            if (m_stage == 1)
            {
                static_cast<blaVec3*>(m_obj)->x = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 2)
            {
                static_cast<blaVec3*>(m_obj)->y = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 3)
            {
                static_cast<blaVec3*>(m_obj)->z = f;
                ++m_stage; return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* EndArray(size_t count) override
        {
            if (m_stage == 4 && count == 3)
            {
                m_stage++;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 0;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaVec3>()
{
    static blaVec3Descriptor typeDesc;
    return &typeDesc;
}

struct blaQuatDescriptor : ExposedVarTypeDescriptor
{
    blaQuatDescriptor() : ExposedVarTypeDescriptor{ BlaStringId("Quaternion"), sizeof(blaQuat) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaQuat* q = static_cast<blaQuat*>(obj);
        writer->Double(q->x);
        writer->Double(q->y);
        writer->Double(q->z);
        writer->Double(q->w);
        writer->EndArray();
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        Deserializer* Double(double bd) override
        {
            float b = (float)bd;
            if (m_stage == 1)
            {
                static_cast<blaQuat*>(m_obj)->x = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 2)
            {
                static_cast<blaQuat*>(m_obj)->y = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 3)
            {
                static_cast<blaQuat*>(m_obj)->z = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 4)
            {
                static_cast<blaQuat*>(m_obj)->w = b;
                ++m_stage; return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* EndArray(size_t count) override
        {
            if (m_stage == 5 && count == 4)
            {
                m_stage++;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 0;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaQuat>()
{
    static blaQuatDescriptor typeDesc;
    return &typeDesc;
}

struct blaPosQuatDescriptor : ExposedVarTypeDescriptor
{
    blaPosQuatDescriptor() : ExposedVarTypeDescriptor{ BlaStringId("Rigid Transform"), sizeof(blaPosQuat) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaPosQuat* pq = static_cast<blaPosQuat*>(obj);
        writer->Double(pq->GetTranslation().x);
        writer->Double(pq->GetTranslation().y);
        writer->Double(pq->GetTranslation().z);
        writer->Double(pq->GetRotation().x);
        writer->Double(pq->GetRotation().y);
        writer->Double(pq->GetRotation().z);
        writer->Double(pq->GetRotation().w);
        writer->EndArray();
    }


    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        Deserializer* Double(double d) override
        {
            float f = (float)d;
            if (m_stage == 1)
            {
                static_cast<blaPosQuat*>(m_obj)->GetTranslation().x = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 2)
            {
                static_cast<blaPosQuat*>(m_obj)->GetTranslation().y = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 3)
            {
                static_cast<blaPosQuat*>(m_obj)->GetTranslation().z = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 4)
            {
                static_cast<blaPosQuat*>(m_obj)->GetRotation().x = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 5)
            {
                static_cast<blaPosQuat*>(m_obj)->GetRotation().y = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 6)
            {
                static_cast<blaPosQuat*>(m_obj)->GetRotation().z = f;
                ++m_stage; return nullptr;
            }
            if (m_stage == 7)
            {
                static_cast<blaPosQuat*>(m_obj)->GetRotation().w = f;
                ++m_stage; return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* EndArray(size_t count) override
        {
            if (m_stage == 8 && count == 7)
            {
                m_stage++;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 0;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaPosQuat>()
{
    static blaPosQuatDescriptor typeDesc;
    return &typeDesc;
}

struct blaScaledTransformDescriptor : ExposedVarTypeDescriptor
{
    blaScaledTransformDescriptor(): ExposedVarTypeDescriptor{ BlaStringId("Scaled Rigid Transform"), sizeof(blaScaledTransform) }
    {}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaScaledTransform* scaledTransform = static_cast<blaScaledTransform*>(obj);
        writer->Double(scaledTransform->m_scale.x);
        writer->Double(scaledTransform->m_scale.y);
        writer->Double(scaledTransform->m_scale.z);
        blaPosQuat* pq = &scaledTransform->m_posQuat;
        writer->Double(pq->GetTranslation().x);
        writer->Double(pq->GetTranslation().y);
        writer->Double(pq->GetTranslation().z);
        writer->Double(pq->GetRotation().x);
        writer->Double(pq->GetRotation().y);
        writer->Double(pq->GetRotation().z);
        writer->Double(pq->GetRotation().w);
        writer->EndArray();
    }

    struct TypedDeserializer : Deserializer
    {
        TypedDeserializer(void* obj) : Deserializer(obj) {}

        Deserializer* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        Deserializer* Double(double bd) override
        {
            float b = (float)bd;
            if (m_stage == 1)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_scale.x = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 2)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_scale.y = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 3)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_scale.z = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 4)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetTranslation().x = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 5)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetTranslation().y = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 6)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetTranslation().z = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 7)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetRotation().x = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 8)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetRotation().y = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 9)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetRotation().z = b;
                ++m_stage; return nullptr;
            }
            if (m_stage == 10)
            {
                static_cast<blaScaledTransform*>(m_obj)->m_posQuat.GetRotation().w = b;
                ++m_stage; return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        Deserializer* EndArray(size_t count) override
        {
            if (m_stage == 11 && count == 10)
            {
                m_stage++;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 0;
    };

    Deserializer* GetDeserializer(void* obj) const override
    {
        return new TypedDeserializer(obj);
    };
};

template <>
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<blaScaledTransform>()
{
    static blaScaledTransformDescriptor typeDesc;
    return &typeDesc;
}

template<typename T>
struct blaVectorDescriptor : ExposedVarTypeDescriptor
{
    ExposedVarTypeDescriptor* m_itemType;
    size_t(*getSize)(const void*);
    const void* (*getItem)(const void*, size_t);

    blaVectorDescriptor() : ExposedVarTypeDescriptor
    { GenerateBlaStringId(blaString("Vector<") + blaString(TypeResolver<T>::get()->GetTypeID()) + ">"), sizeof(std::vector<T>) },
        m_itemType{ TypeResolver<T>::get() }
    {
        getSize = [](const void* vecPtr) -> size_t
        {
            const auto& vec = *(const std::vector<T>*) vecPtr;
            return vec.size();
        };
        getItem = [](const void* vecPtr, size_t index) -> const void*
        {
            const auto& vec = *(const std::vector<T>*) vecPtr;
            return &vec[index];
        };
    }

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaVector<T>* v = static_cast<blaVector<T>*>(obj);

        ExposedVarTypeDescriptor* TTypeDescriptor = TypeResolver<T>::get();
    	
        for(T& e : *v) 
        {
            TTypeDescriptor->Serialize(static_cast<void*>(&e), writer);
        }
        writer->EndArray();
    }
};

template<typename T1, typename T2>
struct PairDescriptor : ExposedVarTypeDescriptor
{
    ExposedVarTypeDescriptor* m_firstType, m_secondType;

    T1& (*GetFirst)(const void*);
    T2& (*GetSecond)(const void*);

    PairDescriptor() : ExposedVarTypeDescriptor
    { blaString("Pair<") + TypeResolver<T1>::get()->GetTypeID() + ", " + TypeResolver<T2>::get()->GetTypeID() + ">", sizeof(blaPair<T1,T2>) },
        m_firstType{ TypeResolver<T1>::get() }, m_secondType{ ypeResolver<T2>::get() }
    {
        GetFirst = [](const void* pair) -> T1&
        {
            const blaPair<T1, T2>* pair = *(const blaPair<T1,T2>*) pairPtr;
            return pairPtr->first;
        };

        GetSecond = [](const void* pair) -> T2&
        {
            const blaPair<T1, T2>* pair = *(const blaPair<T1, T2>*) pairPtr;
            return pairPtr->second;
        };
    }

    //BlaGuiElement* MakeEditGuiElement(const blaString& name, blaStringId groupId, void* obj) override
    //{
    //    return new BlaGuiEditElementVector<T>(name, (blaVector<T>*)obj);
    //}

    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaVector<T>* v = static_cast<blaVector<T>*>(obj);
        for (T& e : *v)
        {
            e->Serialize();
        }
        writer->EndArray();
    }
};

template <typename T1, typename T2>
ExposedVarTypeDescriptor* BLAInspectableVariables::TypeResolver<blaPair<T1, T2>>::get()
{
    static PairDescriptor<T1, T2> typeDesc;
    return &typeDesc;
}

void ExposedVarTypeDescriptor::Deserializer::ErrorMessage() const
{
    Console::LogError(m_errorMessage);
}

ExposedVarTypeDescriptor* TypeResolver<std::vector<std::basic_string<char>>>::get()
{
    static blaVectorDescriptor<blaString> typeDesc;
    return &typeDesc;
}
