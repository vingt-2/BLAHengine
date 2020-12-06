// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/InspectableVariables.h"

#define DESERIALIZE_ERROR reinterpret_cast<SAXDeserializerObject*>(-2)
#define POP_DESERIALIZER reinterpret_cast<SAXDeserializerObject*>(-1)

namespace BLA
{
    struct SAXDeserializerObject
    {
        SAXDeserializerObject(void* obj) : m_obj(obj) {}

        virtual SAXDeserializerObject* Null() { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Bool(bool b) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Int(int i) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Uint(unsigned i) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Int64(int64_t i) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Uint64(uint64_t i) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Double(double d) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* RawNumber(const char* str, size_t length, bool copy) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* String(const char* str, size_t  length, bool copy) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* StartObject() { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* Key(const char* str, size_t  length, bool copy) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* EndObject(size_t  memberCount) { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* StartArray() { ErrorMessage(); return DESERIALIZE_ERROR; }
        virtual SAXDeserializerObject* EndArray(size_t  elementCount) { ErrorMessage(); return DESERIALIZE_ERROR; }

    protected:

        void ErrorMessage() const;
        blaString m_errorMessage;
        void* m_obj;
    };

    struct JSONSerializer
    {
        virtual void Serialize(void* obj, BLASerializeWriter* writer) const {};

        virtual SAXDeserializerObject* GetSAXDeserializerObject(void* obj) const { return new SAXDeserializerObject(obj); }
    };
    
    struct JSONSerializerManager
    {
        static const JSONSerializer* GetSerializer(blaStringId typeID, void* obj);

        static void __RegisterJSONSerializer(blaStringId type, JSONSerializer* serializer);
        
    private:
        
        typedef blaMap<blaStringId, const JSONSerializer*> Serializers;

        static Serializers ms_serializers;
    };

    
}