#include "InspectableVariables.h"
#include "Core/GameObject.h"
#include "Maths/Maths.h"
#include "System.h"
#include "System/Console.h"

using namespace BLA;
using namespace BLAInspectableVariables;

#define DECLARE_PRIMITIVE_DESCRIPTOR(TypeName, TypeStringName)                                          \
	struct TypeName##Descriptor : ExposedVarTypeDescriptor                                              \
{                                                                                                       \
    TypeName##Descriptor() : ExposedVarTypeDescriptor{ BlaStringId(TypeStringName), sizeof(TypeName) }  \
    {}                                                                                                  \
};                                                                                                      \
template <>                                                                                             \
BLACORE_API ExposedVarTypeDescriptor* BLAInspectableVariables::GetPrimitiveDescriptor<TypeName>()       \
{                                                                                                       \
    static TypeName##Descriptor typeDesc;                                                               \
    return &typeDesc;                                                                                   \
}

/*
 * Declare all primitive descriptors (template specialization)
 */
DECLARE_PRIMITIVE_DESCRIPTOR(bool,"Boolean(8)")
DECLARE_PRIMITIVE_DESCRIPTOR(blaF32, "Float(32)")
DECLARE_PRIMITIVE_DESCRIPTOR(blaS32, "Integer(32)")
DECLARE_PRIMITIVE_DESCRIPTOR(blaString, "String")
DECLARE_PRIMITIVE_DESCRIPTOR(GameObject, "GameObject")
DECLARE_PRIMITIVE_DESCRIPTOR(blaVec2, "2D Vector (float)")
DECLARE_PRIMITIVE_DESCRIPTOR(blaVec3, "3D Vector (float)")
DECLARE_PRIMITIVE_DESCRIPTOR(blaQuat, "Quaternion")
DECLARE_PRIMITIVE_DESCRIPTOR(blaPosQuat, "Rigid Transform (PosQuat)")
DECLARE_PRIMITIVE_DESCRIPTOR(blaScaledTransform, "Scaled Rigid Transform")

/*
 * Declare container and other templated descriptors
 */
template<typename T1, typename T2>
struct PairDescriptor : ExposedVarTypeDescriptor
{
    ExposedVarTypeDescriptor* m_firstType, m_secondType;

    T1& (*GetFirst)(const void*);
    T2& (*GetSecond)(const void*);

    PairDescriptor() : ExposedVarTypeDescriptor
    { blaString("Pair<") + TypeResolver<T1>::GetDescriptor()->m_typeID + ", " + TypeResolver<T2>::GetDescriptor()->m_typeID + ">", sizeof(blaPair<T1,T2>) },
        m_firstType{ TypeResolver<T1>::GetDescriptor() }, m_secondType{ TypeResolver<T2>::GetDescriptor() }
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
};

template <typename T1, typename T2>
ExposedVarTypeDescriptor* BLAInspectableVariables::TypeResolver<blaPair<T1, T2>>::GetDescriptor()
{
    static PairDescriptor<T1, T2> typeDesc;
    return &typeDesc;
}

//TODO: Remove me ?
ExposedVarTypeDescriptor* TypeResolver<std::vector<std::basic_string<char>>>::GetDescriptor()
{
    static blaVectorDescriptor<blaString> typeDesc;
    return &typeDesc;
}
