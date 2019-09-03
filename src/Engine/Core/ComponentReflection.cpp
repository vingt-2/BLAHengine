#include "ComponentReflection.h"
#include <Common/System.h>

using namespace BLAengine;
using namespace ComponentReflection;

struct S32Descriptor : ExposedVarDescriptor 
{
    S32Descriptor() : ExposedVarDescriptor{ "S32", sizeof(blaS32) }
    {
    }
    virtual blaString ToString(const void* obj, int /* unused */) const override 
    {
        blaString s = "";
        s += "S32{" + std::to_string(*(const int*)obj) + "}";
        return s;
    }

	// TODO: Make it ...
	//BlaGuiElement* MakeEditGuiElement(const void* obj) override
	//{
	//	return new BlaGuiEditElement<blaF32>(GetName(), (float*)obj);
	//}
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<int>()
{
    static S32Descriptor typeDesc;
    return &typeDesc;
}

struct BoolDescriptor : ExposedVarDescriptor
{
    BoolDescriptor() : ExposedVarDescriptor{ "bool", sizeof(bool) }
    {
    }
    virtual blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        blaString tf = ((const bool*)obj ? "true" : "false");
        s += "bool{" + tf + "}";
        return s;
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

	BlaGuiElement* MakeEditGuiElement(const blaString& name, const void* obj) override
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

struct StringDescriptor : ExposedVarDescriptor 
{
    StringDescriptor() : ExposedVarDescriptor{ "std::string", sizeof(blaString) }
    {}

    virtual blaString ToString(const void* obj, int /* unused */) const override
    {
        blaString s = "";
        s += "int{" + *(const blaString*) obj + "}";
        return s;
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<blaString>()
{
    static StringDescriptor typeDesc;
    return &typeDesc;
}

BlaGuiElement* ExposedVarDescriptor::MakeEditGuiElement(const blaString& name, const void* obj) { return nullptr; }
