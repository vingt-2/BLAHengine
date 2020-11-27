// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "JSONSerializers.h"
#include "System.h"
#include "Maths/Maths.h"
#include "Core/GameObject.h"
#include "Core/GameComponent.h"
#include "System/Console.h"

#include "External/rapidjson/prettywriter.h"

using namespace BLA;

JSONSerializerManager::Serializers JSONSerializerManager::ms_serializers;

void SAXDeserializerObject::ErrorMessage() const
{
    Console::LogError(m_errorMessage);
}

const JSONSerializer* JSONSerializerManager::GetSerializer(blaStringId typeID, void* obj)
{
	Serializers::const_iterator it = ms_serializers.find(typeID);
	if (it != ms_serializers.end())
	{
		return it->second;
	}
	return nullptr;
}

void JSONSerializerManager::__RegisterJSONSerializer(blaStringId type, JSONSerializer* serializer)
{
    ms_serializers.insert(std::make_pair(type, serializer));
}

struct JSONSerializerRegistrator
{
    JSONSerializerRegistrator(blaStringId typeId, JSONSerializer* serializer)
    {
        JSONSerializerManager::__RegisterJSONSerializer(typeId, serializer);
    }
    ~JSONSerializerRegistrator() { delete m_serializer; }
private:
	
    JSONSerializer* m_serializer;
};

//TODO: Much like for the InspectableVariablesGuiElements, add in there a serializer object for vectors of primitives ! (with an exception required for blaBool when using std::vector...)
#define REGISTER_PRIMITIVE_SERIALIZER(type) \
	JSONSerializerRegistrator g_##type##Registrator(Core::InspectableVariables::TypeResolver<type>::GetDescriptor()->m_typeID, new type##JSONSerializer());

class BLASerializeWriter : public rapidjson::PrettyWriter<rapidjson::StringBuffer>
{};

struct blaBoolJSONSerializer : JSONSerializer
{
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->Bool(*(bool*)obj);
    }

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* Bool(bool b) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaBool)

struct blaF32JSONSerializer : JSONSerializer
{
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->Double(*(blaF32*)obj);
    }

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* Null() override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaF32*>(m_obj) = 0.f;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

        SAXDeserializerObject* Double(double d) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaF32)

struct blaS32JSONSerializer : JSONSerializer
{
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->Int(*(blaS32*)obj);
    }

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* Int(int i) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<blaS32*>(m_obj) = i;
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

        SAXDeserializerObject* Uint(unsigned i) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaS32)

struct blaStringJSONSerializer : JSONSerializer
{
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->String(static_cast<blaString*>(obj)->c_str());
    }

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* String(const char* str, size_t length, bool copy) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaString)

struct GameObjectJSONSerializer : JSONSerializer
{
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->String(static_cast<Core::GameObject*>(obj)->GetId());
    }

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* String(const char* str, size_t length, bool copy) override
        {
            if (m_stage == 1)
            {
                m_stage++;
                *static_cast<Core::GameObject*>(m_obj) = Core::GameObject(GenerateBlaStringId(blaString(str, length)));
                return POP_DESERIALIZER;
            }
            return DESERIALIZE_ERROR;
        }

    private:
        int m_stage = 1;
    };

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

using namespace Core;
REGISTER_PRIMITIVE_SERIALIZER(GameObject)

struct blaVec2JSONSerializer : JSONSerializer
{
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
        writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
        writer->StartArray();
        blaVec2* vec2 = static_cast<blaVec2*>(obj);
        writer->Double(vec2->x);
        writer->Double(vec2->y);
        writer->EndArray();
    }

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        SAXDeserializerObject* Double(double bd) override
        {
            float b = (float)bd;
            if (m_stage == 1)
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

        SAXDeserializerObject* EndArray(size_t count) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaVec2)

struct blaVec3JSONSerializer : JSONSerializer
{
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

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        SAXDeserializerObject* Double(double d) override
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

        SAXDeserializerObject* EndArray(size_t count) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaVec3)

struct blaQuatJSONSerializer : JSONSerializer
{
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

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        SAXDeserializerObject* Double(double bd) override
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

        SAXDeserializerObject* EndArray(size_t count) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaQuat)

struct blaPosQuatJSONSerializer : JSONSerializer
{
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


    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        SAXDeserializerObject* Double(double d) override
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

        SAXDeserializerObject* EndArray(size_t count) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};

REGISTER_PRIMITIVE_SERIALIZER(blaPosQuat)

struct blaScaledTransformJSONSerializer : JSONSerializer
{
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

    struct TypedSAXDeserializerObject : SAXDeserializerObject
    {
        TypedSAXDeserializerObject(void* obj) : SAXDeserializerObject(obj) {}

        SAXDeserializerObject* StartArray() override
        {
            if (m_stage == 0)
            {
                m_stage++;
                return nullptr;
            }
            return DESERIALIZE_ERROR;
        }

        //Todo: Support ints ?
        SAXDeserializerObject* Double(double bd) override
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

        SAXDeserializerObject* EndArray(size_t count) override
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

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        return new TypedSAXDeserializerObject(obj);
    };
};


REGISTER_PRIMITIVE_SERIALIZER(blaScaledTransform)

//TODO: Implement pair deserializer
template<typename T1, typename T2>
struct PairJSONSerializer : JSONSerializer
{
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

struct GameComponentJSONSerializer : JSONSerializer
{	
    void Serialize(void* obj, BLASerializeWriter* writer) const override
    {
    	//TODO: This here is a bit dangerous. I'd like not to have to trust we pass the right pointer and have a static check...
        const Core::ComponentDescriptor& desc = reinterpret_cast<Core::GameComponent*>(obj)->GetComponentDescriptor();
        writer->StartObject();
        for (const Core::ComponentDescriptor::ExposedMember& member : desc.m_members)
        {
            writer->Key(ToString(member.m_name).c_str());
            const JSONSerializer* memberSerializer = JSONSerializerManager::GetSerializer(member.m_type->m_typeID, (char*)obj + member.m_offset);
        	if(!memberSerializer)
        	{
                Console::LogError("Could not find proper JSON Serializer for type " + ToString(member.m_type->m_typeID) + " while serializing component of type " + ToString(desc.m_typeID) + ".");
        	}
            else
            {
                memberSerializer->Serialize((char*)obj + member.m_offset, writer);
            }
        }
        writer->EndObject();
    }

    struct TypedDeserializer : SAXDeserializerObject
    {
        TypedDeserializer(void* obj, const Core::ComponentDescriptor* componentDescriptor) : SAXDeserializerObject(obj), m_componentDescriptor(componentDescriptor) {}

        virtual SAXDeserializerObject* StartObject() { return nullptr; }
        virtual SAXDeserializerObject* Key(const char* str, size_t  length, bool copy)
        {
            blaStringId varName = FromString(blaString(str, length));
            for (const Core::ComponentDescriptor::ExposedMember& member : m_componentDescriptor->m_members)
            {
                if (member.m_name == varName)
                {
                    const JSONSerializer* memberSerializer = JSONSerializerManager::GetSerializer(member.m_type->m_typeID, (char*)m_obj + member.m_offset);
                    if (!memberSerializer)
                    {
                        Console::LogError("Could not find proper JSON Serializer for type " + ToString(member.m_type->m_typeID) + " while deserializing component of type " + ToString(m_componentDescriptor->m_typeID) + ".");
                        return DESERIALIZE_ERROR;
                    }
                    return memberSerializer->GetSAXDeserializerObject((char*)m_obj + member.m_offset);
                }
            }
            return DESERIALIZE_ERROR;
        }
        virtual SAXDeserializerObject* EndObject(size_t  memberCount)
        {
            return POP_DESERIALIZER;
        }

    private:
        const Core::ComponentDescriptor* m_componentDescriptor;
        int m_stage = 0;
        int m_parsingMember = 0;
    };

    SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const override
    {
        //TODO: This here is a bit dangerous. I'd like not to have to trust we pass the right pointer and have a static check...
        const Core::ComponentDescriptor& desc = reinterpret_cast<Core::GameComponent*>(obj)->GetComponentDescriptor();
	    return new TypedDeserializer(obj, &desc);
    }
};

JSONSerializerRegistrator g_gameComponentSerializerRegistrator(BlaStringId("GameComponent"), new GameComponentJSONSerializer());
