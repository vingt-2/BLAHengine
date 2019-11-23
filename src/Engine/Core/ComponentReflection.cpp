#include "ComponentReflection.h"
#include <Engine/Core/GameObject.h>
#include <Common/System.h>

using namespace BLAengine;
using namespace ComponentReflection;

struct BoolDescriptor : ExposedVarDescriptor
{
    BoolDescriptor() : ExposedVarDescriptor{ "bool", sizeof(bool) }
    {}

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        blaString tf = ((const bool*)obj ? "true" : "false");
        s += "bool{" + tf + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<bool>(name, (bool*)obj);
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<bool>()
{
    static BoolDescriptor typeDesc;
    return &typeDesc;
}

struct F32Descriptor : ExposedVarDescriptor
{
    F32Descriptor() : ExposedVarDescriptor{ "F32", sizeof(blaF32) }
    {
    }

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "F32{" + std::to_string(*(const int*)obj) + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<blaF32>(name, (float*)obj);
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<blaF32>()
{
    static F32Descriptor typeDesc;
    return &typeDesc;
}

struct S32Descriptor : ExposedVarDescriptor
{
    S32Descriptor() : ExposedVarDescriptor{ "S32", sizeof(blaS32) }
    {
    }

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "S32{" + std::to_string(*(const int*)obj) + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<blaS32>(name, (blaS32*)obj);
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<blaS32>()
{
    static S32Descriptor typeDesc;
    return &typeDesc;
}

struct StringDescriptor : ExposedVarDescriptor
{
    StringDescriptor() : ExposedVarDescriptor{ "String", sizeof(blaString) }
    {}

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "String{" + *(const blaString*)obj + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<blaString>(name, (blaString*)obj);
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<blaString>()
{
    static StringDescriptor typeDesc;
    return &typeDesc;
}

struct GameObjectReferenceDescriptor : ExposedVarDescriptor
{
    GameObjectReferenceDescriptor() : ExposedVarDescriptor{ "GameObject", sizeof(GameObjectReference) }
    {
    }

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "GameObject {" + static_cast<const GameObjectReference*>(obj)->GetObject().GetName() + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<GameObjectReference>(name, static_cast<GameObjectReference*>(obj));
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<GameObjectReference>()
{
    static GameObjectReferenceDescriptor typeDesc;
    return &typeDesc;
}

struct blaVec2Descriptor : ExposedVarDescriptor
{
    blaVec2Descriptor() : ExposedVarDescriptor{ "Vector2", sizeof(blaVec2) }
    {}

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "Vector2{" + *(const blaString*)obj + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<blaVec2>(name, (blaVec2*)obj);
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<blaVec2>()
{
    static blaVec2Descriptor typeDesc;
    return &typeDesc;
}

struct blaVec3Descriptor : ExposedVarDescriptor
{
    blaVec3Descriptor() : ExposedVarDescriptor{ "Vector3", sizeof(blaVec2) }
    {}

    blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "Vector3{" + *(const blaString*)obj + "}";
        return s;
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<blaVec3>(name, (blaVec3*)obj);
    }
};

template<typename T>
struct VectorDescriptor : ExposedVarDescriptor
{
    ExposedVarDescriptor* m_itemType;
    size_t(*getSize)(const void*);
    const void* (*getItem)(const void*, size_t);

    template <typename ItemType>
    VectorDescriptor(ItemType* a) : ExposedVarDescriptor
    { blaString("blaVector<") + a->GetName() + ">", sizeof(std::vector<ItemType>) },
        m_itemType{ TypeResolver<ItemType>::get() }
    {
        getSize = [](const void* vecPtr) -> size_t
        {
            const auto& vec = *(const std::vector<ItemType>*) vecPtr;
            return vec.size();
        };
        getItem = [](const void* vecPtr, size_t index) -> const void*
        {
            const auto& vec = *(const std::vector<ItemType>*) vecPtr;
            return &vec[index];
        };
    }

    virtual const blaString& GetName() const override
    {
        return blaString("blaVector<") + m_itemType->GetName() + ">";
    }

    virtual blaString ToString(const void* obj, int indentLevel) const override
    {
        blaString s = "";
        size_t numItems = getSize(obj);
        s += GetName();
        if (numItems == 0)
        {
            s += "{}";
        }
        else
        {
            s += "{\n";
            for (size_t index = 0; index < numItems; index++)
            {
                s += blaString(4 * (indentLevel + 1), ' ') + "[" + std::to_string(index) + "]\n";
                m_itemType->ToString(getItem(obj, index), indentLevel + 1);
                s += "\n";
            }
            s += blaString(4 * indentLevel, ' ') + "}";
        }
    }

    BlaGuiElement* MakeEditGuiElement(const blaString& name, void* obj) override
    {
        return new BlaGuiEditElement<blaVec3>(name, (blaVec3*)obj);
    }
};

// Partially specialize TypeResolver<> for std::vectors:
template <typename T>
class TypeResolver<blaVector<T>>
{
public:
    static ExposedVarDescriptor* get()
    {
        static VectorDescriptor<T> typeDesc{ (T*) nullptr };
        return &typeDesc;
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<blaVec3>()
{
    static blaVec3Descriptor typeDesc;
    return &typeDesc;
}

BlaGuiElement* ExposedVarDescriptor::MakeEditGuiElement(const blaString& name, void* obj) { return nullptr; }
