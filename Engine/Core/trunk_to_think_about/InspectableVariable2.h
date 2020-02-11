#pragma once

#include <System/Console.h>

#define NON_INSPECTABLE_TYPE_ERROR "You exposed a non-inspectable type. You can't do that."

#define IS_INSPECTABLE_TEMPLATE(T_ARG) std::disjunction<std::is_base_of<InspectableClass, T_ARG>, std::is_fundamental<T_ARG>>::value

namespace BLAengine
{
    class InspectableClass
    {  };

    class InspectableInt : public InspectableClass
    {
    public:
        InspectableInt(int value) : m_value(value) {};
        int m_value;
    };

    // Here below is what will be created when adding Exposed variables on a component.
    // It expects an InspectableClass and will not compile if the type is not properly reflected

    template<typename T>
    class ExposedVar
    {
    public:

        template <typename U = T, typename std::enable_if<IS_INSPECTABLE_TEMPLATE(U), int>::type = 0>
        ExposedVar(const U& value, const blaString& name) : m_value(value), m_name(name)
        {};
        template <typename U = T, typename std::enable_if<!IS_INSPECTABLE_TEMPLATE(U), int>::type = 0>
        ExposedVar(const U & value, const blaString & name) : m_value(value), m_name(name) { static_assert(NON_INSPECTABLE_TYPE_ERROR); };

        template <typename U = T, typename std::enable_if<IS_INSPECTABLE_TEMPLATE(U), int>::type = 0>
        U GetValue()
        {
            return m_value;
        }
        template <typename U = T, typename std::enable_if<!IS_INSPECTABLE_TEMPLATE(U), int>::type = 0>
        U GetValue() { static_assert(false, NON_INSPECTABLE_TYPE_ERROR); }

        template <typename U = T, typename std::enable_if<IS_INSPECTABLE_TEMPLATE(U), int>::type = 0>
        void SetValue(U value)
        {
            m_value = value;
        }
        template <typename U = T, typename std::enable_if<!IS_INSPECTABLE_TEMPLATE(U), int>::type = 0>
        void SetValue(U value) { static_assert(false, NON_INSPECTABLE_TYPE_ERROR); }

        blaString GetName() { return m_name; }
    private:
        T m_value;

        blaString m_name;
    };
};