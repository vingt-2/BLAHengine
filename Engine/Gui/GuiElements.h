#pragma once

#include <DataStructures/Tree.h>
#include <StdInclude.h>

//TODO: Hide Render() from the element managers

namespace BLA
{
    class BlaGuiElement;
    struct BlaGuiElementEventPayload
    {
        enum EventType
        {
            SELECTED = 1 << 0,
            DOUBLE_CLICKED = 1 << 0
        } m_eventType;

        BlaGuiElement* m_pGuiElement;
    };

    typedef void(*EventCallBack)(void*, const BlaGuiElementEventPayload&);

    struct BlaGuiRegisteredEvents
    {
        blaU32 m_eventTriggerFlags = 0;
        EventCallBack m_callback = nullptr;
        void* m_callerPtr = nullptr;
    };

    class BLACORE_API BlaGuiElement : public IntrusiveTree<BlaGuiElement>
    {
        blaString m_name;

        blaVector<BlaGuiRegisteredEvents> m_registeredCallbacks;

    protected:
        void SendEvent(BlaGuiElementEventPayload::EventType eventType);

    public:
        BlaGuiElement(const blaString& name) : m_name(name), m_isSelected(false) {}

        // Todo: Set options for how to display children

        const blaString& GetName() const { return m_name; }

        void RegisterEvents(BlaGuiRegisteredEvents& cb);
        void UnRegisterEvents(BlaGuiRegisteredEvents& cb);

        virtual void Render();

        bool m_isSelected;
    };

    class BLACORE_API BlaGuiCollapsibleElement : public BlaGuiElement
    {
    public:
        BlaGuiCollapsibleElement(const blaString& name) : BlaGuiElement(name) {}

        virtual void Render();
    };

    class BLACORE_API BlaGuiCollapsibleHeaderElement : public BlaGuiElement
    {
    public:
        BlaGuiCollapsibleHeaderElement(const blaString& name) : BlaGuiElement(name) {}

        virtual void Render();
    };

    class BLACORE_API BlaGuiSimpleTextElement : public BlaGuiElement
    {
    public:
        BlaGuiSimpleTextElement(const blaString& name, const blaString& text) :
            BlaGuiElement(name),
            m_text(text) {}

        void Render() override;

    private:
        blaString m_text;
    };

    template<typename T>
    class BlaGuiEditElement : public BlaGuiElement
    {
    public:
        BlaGuiEditElement(const blaString& name, T* pToValue) : BlaGuiElement(name), m_pToValue(pToValue)
        {}

        void Render() override;

    private:
        T* m_pToValue;
    };

    template<typename T>
    class BlaGuiEditElementVector : public BlaGuiElement
    {
    public:
        BlaGuiEditElementVector(const blaString& name, blaVector<T*> pToVector) : BlaGuiElement(name), m_pToVector(pToVector)
        {}

        void Render() override;

    private:
        blaVector<T>* m_pToVector;
    };

}