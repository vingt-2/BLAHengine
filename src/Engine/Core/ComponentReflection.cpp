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
        s += "String{" + *(const blaString*) obj + "}";
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

BlaGuiElement* ExposedVarDescriptor::MakeEditGuiElement(const blaString& name, void* obj) { return nullptr; }
