#include <Common/StdInclude.h>

// Todo: This can all fit in GameComponent.h right ?
// Todo: Support component extension
// Todo: Support Abstract Component declarations

namespace BLAengine 
{
    class GameComponent;
namespace ComponentReflection
{
    struct ExposedVarDescriptor 
    {
        blaString m_typeName;
        size_t size;

        ExposedVarDescriptor(const blaString& name, size_t size) : m_typeName{ name }, size{ size } {}
        virtual ~ExposedVarDescriptor() {}
        virtual const blaString& GetName() const { return m_typeName; }
        virtual blaString ToString(const void* obj, int indentLevel = 0) const { return ""; };
    };

    // Declare the function template that handles primitive types such as int, std::string, etc.:
    template <typename T>
    ExposedVarDescriptor* GetPrimitiveDescriptor();

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
        static ExposedVarDescriptor* get() 
        {
            return &T::Reflection;
        }

        // This version is called otherwise:
        template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
        static ExposedVarDescriptor* get() 
        {
            return GetPrimitiveDescriptor<T>();
        }
    };

    // This is the primary class template for finding all TypeDescriptors:
    template <typename T>
    struct TypeResolver 
    {
        static ExposedVarDescriptor* get() 
        {
            return DefaultResolver::get<T>();
        }
    };

    struct ComponentDescriptor : ExposedVarDescriptor 
    {
        struct ExposedMember
        {
            blaString m_name;
            size_t m_offset;
            ExposedVarDescriptor* m_type;
        };

        std::vector<ExposedMember> m_members;

        ComponentDescriptor(void(*init)(ComponentDescriptor*)) : ExposedVarDescriptor{ "", 0 } 
        {
            init(this);
        }

        ComponentDescriptor(const char* name, size_t size, const std::initializer_list<ExposedMember>& init) : ExposedVarDescriptor{ nullptr, 0 }, m_members{ init } 
        {}

        virtual blaString ToString(const void* obj, int indentLevel) const override 
        {
            blaString s = "";
            s += m_typeName + "\n{\n";
            for (const ExposedMember& member : m_members) 
            {
                s += blaString(4 * (indentLevel + 1), ' ') + member.m_name + " = ";
                s += member.m_type->ToString((char*)obj + member.m_offset, indentLevel + 1);
                s += "\n";
            }
            s += blaString(4 * indentLevel, ' ') + "}";

            return s;
        }
    };

#define BEGIN_COMPONENT_DECLARATION(CompName)                                                               \
    class BLACORE_API CompName : public GameComponent {                                                     \
    friend struct ComponentReflection::DefaultResolver;                                                     \
    friend class ComponentManager;                                                                          \
    static ComponentReflection::ComponentDescriptor ms_componentDescriptor;                                 \
    static void InitReflection(ComponentReflection::ComponentDescriptor*);                                  \
    static GameComponent* Factory(GameObjectReference objR) { return new CompName(objR); }                  \
	virtual const ComponentReflection::ComponentDescriptor& GetComponentDescriptor() const {				\
		return ms_componentDescriptor; }																	\
    public:                                                                                                 \

#define END_DECLARATION() };

#define BEGIN_COMPONENT_DESCRIPTION(type)																	\
    ComponentReflection::ComponentDescriptor type::ms_componentDescriptor{type::InitReflection};            \
    void type::InitReflection(ComponentReflection::ComponentDescriptor* typeDesc) {                         \
        using T = type;                                                                                     \
        typeDesc->m_typeName = blaString(#type);                                                            \
        typeDesc->size = sizeof(T);                                                                         \
        typeDesc->m_members = {

#define EXPOSE(name)                                                                                        \
            {#name, offsetof(T, name), ComponentReflection::TypeResolver<decltype(T::name)>::get()},

#define END_DESCRIPTION()                                                                                   \
        };                                                                                                  \
        GameComponentManager* manager = GameComponentManager::GetSingletonInstance();                       \
        if(!manager)                                                                                        \
            manager = GameComponentManager::AssignAndReturnSingletonInstance(new GameComponentManager());   \
        manager->__RegisterComponent(typeDesc->m_typeName, T::Factory);                                     \
    }

    struct VectorDescriptor : ExposedVarDescriptor 
    {
        ExposedVarDescriptor* m_itemType;
        size_t(*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);

        template <typename ItemType>
        VectorDescriptor(ItemType* a) : ExposedVarDescriptor
            { blaString("std::vector<") + a->GetName() + ">", sizeof(std::vector<ItemType>) },
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
            return blaString("std::vector<") + m_itemType->GetName() + ">";
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
                s  += blaString(4 * indentLevel, ' ') + "}";
            }
        }
    };

    // Partially specialize TypeResolver<> for std::vectors:
    template <typename T>
    class TypeResolver<std::vector<T>> 
    {
    public:
        static ExposedVarDescriptor* get() 
        {
            static VectorDescriptor typeDesc{ (T*) nullptr };
            return &typeDesc;
        }
    };
}
} // namespace reflect
