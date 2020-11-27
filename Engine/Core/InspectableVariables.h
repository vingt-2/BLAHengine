// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "Maths/Maths.h"
#include "BLAStringID.h"

// Todo: Move MakeEditGuiElement and the JSON serializer stuff outside of here, and even more, outside of Engine...
// Todo: Make deserializer work either for JSON or binary serialization 
class BLASerializeWriter;
namespace BLA
{
	class DevGuiElement;

	namespace Core
	{
		class GameObject;
		class GameComponent;

		namespace InspectableVariables
		{
			struct BLACORE_API ExposedVarTypeDescriptor
			{
				ExposedVarTypeDescriptor(const blaStringId& typeID, size_t size) : m_typeID{ typeID }, size{ size } {}
				virtual ~ExposedVarTypeDescriptor() = default;

				blaStringId m_typeID;
				size_t size;
			};

			// Declare the function template that handles primitive types such as int, std::string, etc.:
			template <typename T>
			BLACORE_API ExposedVarTypeDescriptor* GetPrimitiveDescriptor();

			// A helper class to find TypeDescriptors in different ways:
			struct DefaultResolver
			{
				template <typename T> static char func(decltype(&T::Reflection));
				template <typename T> static int func(...);
				template <typename T>
				struct IsReflected
				{
					enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
				};

				// This version is called if T has a static member named "Reflection":
				template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
				static ExposedVarTypeDescriptor* GetDescriptor()
				{
					return &T::Reflection;
				}

				// This version is called otherwise:
				// We assert statically if we reach this template expansion and T isn't a primitive type
				template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
				static ExposedVarTypeDescriptor* GetDescriptor()
				{
					return GetPrimitiveDescriptor<T>();
				}
			};

			// This is the primary class template for finding all TypeDescriptors:
			template <typename T>
			struct TypeResolver
			{
				static ExposedVarTypeDescriptor* GetDescriptor()
				{
					return DefaultResolver::GetDescriptor<T>();
				}
			};

			// Declaring Primitive descriptor specialization and TypeResolver Specialization:
			template <typename T1, typename T2>
			struct TypeResolver<blaPair<T1, T2>>
			{
				static ExposedVarTypeDescriptor* GetDescriptor();
			};

			template<typename T>
			struct blaVectorDescriptor : ExposedVarTypeDescriptor
			{
				ExposedVarTypeDescriptor* m_itemType;
				blaSize(*getSize)(const void*);
				const void* (*getItem)(const void*, blaSize);

				blaVectorDescriptor() : ExposedVarTypeDescriptor
				{ GenerateBlaStringId(blaString("Vector<") + blaString(TypeResolver<T>::GetDescriptor()->m_typeID) + ">"), sizeof(blaVector<T>) },
					m_itemType{ TypeResolver<T>::GetDescriptor() }
				{
					getSize = [](const void* vecPtr) -> blaSize
					{
						return static_cast<const blaVector<T>*>(vecPtr)->size();
					};
					getItem = [](const void* vecPtr, blaSize index) -> const void*
					{
						return static_cast<const void*>(&(*static_cast<const blaVector<T>*>(vecPtr))[index]);
					};
				}
			};

			template <typename T>
			struct TypeResolver<blaVector<T>>
			{
				static ExposedVarTypeDescriptor* GetDescriptor()
				{
					static blaVectorDescriptor<T> typeDesc;
					return &typeDesc;
				}
			};
		}
	}
}
