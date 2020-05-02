#pragma once
#include "StdInclude.h"
#include "Gui/GuiElements.h"
#include "Maths/Maths.h"
#include "BLAStringID.h"

#define DESERIALIZE_ERROR reinterpret_cast<Deserializer*>(-2)
#define POP_DESERIALIZER reinterpret_cast<Deserializer*>(-1)

// Todo: Move MakeEditGuiElement and the JSON serializer stuff outside of here, and even more, outside of Engine...
// Todo: Make deserializer work either for JSON or binary serialization 
class BLASerializeWriter;
namespace BLA
{
    class GameObject;
    class GameComponent;
	class BLAGuiElement;

	namespace BLAInspectableVariables
	{
		struct BLACORE_API ExposedVarTypeDescriptor
		{
			blaStringId m_typeName;
			size_t size;

			ExposedVarTypeDescriptor(const blaStringId& name, size_t size) : m_typeName{ name }, size{ size } {}
			virtual ~ExposedVarTypeDescriptor() = default;
			virtual blaStringId GetName() const { return m_typeName; }

			//Todo: This does not need to be here for non editor builds... Find a way to take it out without disgusting macros
		    virtual BlaGuiElement* MakeEditGuiElement(const blaString& name, blaStringId groupId, void* obj) { return nullptr; };

			virtual void Serialize(void* obj, BLASerializeWriter* writer) const {};

			struct Deserializer
		    {
				Deserializer(void* obj) : m_obj(obj) {}

				virtual Deserializer* Null() { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Bool(bool b) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Int(int i) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Uint(unsigned i) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Int64(int64_t i) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Uint64(uint64_t i) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Double(double d) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* RawNumber(const char* str, size_t length, bool copy) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* String(const char* str, size_t  length, bool copy) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* StartObject() { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* Key(const char* str, size_t  length, bool copy) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* EndObject(size_t  memberCount) { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* StartArray() { ErrorMessage(); return DESERIALIZE_ERROR; }
				virtual Deserializer* EndArray(size_t  elementCount) { ErrorMessage(); return DESERIALIZE_ERROR; }

			protected:

				void ErrorMessage() const;
				blaString m_errorMessage;
				void* m_obj;
			};

			virtual Deserializer* GetDeserializer(void* obj) const { return new Deserializer(obj); }
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
			static ExposedVarTypeDescriptor* get()
			{
				return &T::Reflection;
			}

			// This version is called otherwise:
			template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
			static ExposedVarTypeDescriptor* get()
			{
				return GetPrimitiveDescriptor<T>();
			}
		};

		// This is the primary class template for finding all TypeDescriptors:
		template <typename T>
		struct TypeResolver
		{
			static ExposedVarTypeDescriptor* get()
			{
				return DefaultResolver::get<T>();
			}
		};
	};
}
