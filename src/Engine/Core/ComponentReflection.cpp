#include "ComponentReflection.h"

using namespace BLAengine;
using namespace ComponentReflection;

//--------------------------------------------------------
// A type descriptor for int
//--------------------------------------------------------

struct IntDescriptor : ExposedVarDescriptor 
{
    IntDescriptor() : ExposedVarDescriptor{ "int", sizeof(int) }
    {
    }
    virtual blaString ToString(const void* obj, int /* unused */) const override 
    {
        blaString s = "";
        s += "int{" + std::to_string(*(const int*)obj) + "}";
        return s;
    }
};

template <>
ExposedVarDescriptor* ComponentReflection::GetPrimitiveDescriptor<int>()
{
    static IntDescriptor typeDesc;
    return &typeDesc;
}

struct BoolDescriptor : ExposedVarDescriptor
{
    BoolDescriptor() : ExposedVarDescriptor{ "int", sizeof(int) }
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

//--------------------------------------------------------
// A type descriptor for std::string
//--------------------------------------------------------

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
