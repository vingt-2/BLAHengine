#pragma once

#include "DataStructures/Tree.h"
#include "StdInclude.h"
#include "BLAStringID.h"

//TODO: Hide Render() from the element managers

namespace BLA
{
    class BlaGuiElement;

    struct BlaDroppablePayload
    {
        BlaGuiElement* m_receiverElement = nullptr;
        BlaGuiElement* m_droppedElement = nullptr;
    };

    struct BlaGuiElementEventPayload
    {
        enum EventType
        {
            SELECTED = 1 << 0,
            DOUBLE_CLICKED = 1 << 1,
            ELEMENT_DROPPED = 1 << 2,
        } m_eventType;

        void* m_pEventPayload;
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
        void SendEvent(BlaGuiElementEventPayload::EventType eventType, void* pEventPayload);

        void HandleDragDropOfElements();

    public:
        BlaGuiElement(const blaString& name, blaStringId groupId) :
        m_name(name),
        m_groupId(groupId),
        m_isSelected(false),
        m_isDragable(false),
        m_isDroppable(false) {}

        // Todo: Set options for how to display children

        const blaString& GetName() const { return m_name; }

        void RegisterEvents(BlaGuiRegisteredEvents& cb);
        void UnRegisterEvents(BlaGuiRegisteredEvents& cb);

        virtual void Render();

        bool m_isDragable;
        bool m_isDroppable;
        bool m_isSelected;

        blaStringId m_groupId;
    };

    class BLACORE_API BlaGuiCollapsibleElement : public BlaGuiElement
    {
    public:
        BlaGuiCollapsibleElement(const blaString& name, blaStringId groupId) : BlaGuiElement(name, groupId) {}

        virtual void Render();
    };

    class BLACORE_API BlaGuiCollapsibleHeaderElement : public BlaGuiElement
    {
    public:
        BlaGuiCollapsibleHeaderElement(const blaString& name, blaStringId groupId) : BlaGuiElement(name, groupId) {}

        virtual void Render();
    };

    class BLACORE_API BlaGuiSimpleTextElement : public BlaGuiElement
    {
    public:
        BlaGuiSimpleTextElement(const blaString& name, blaStringId groupId, const blaString& text) :
            BlaGuiElement(name, groupId),
            m_text(text) {}

        void Render() override;

    private:
        blaString m_text;
    };

    template<typename T>
    class BlaGuiEditElement : public BlaGuiElement
    {
    public:
        BlaGuiEditElement(const blaString& name, blaStringId groupId, T* pToValue) :
        BlaGuiElement(name, groupId),
        m_pToValue(pToValue)
        {}

        void Render() override;

    private:
        T* m_pToValue;
    };

    template<typename T>
    class BlaGuiEditElementVector : public BlaGuiElement
    {
    public:
        BlaGuiEditElementVector(const blaString& name, blaStringId groupId, blaVector<T*> pToVector) :
        BlaGuiElement(name, groupId),
        m_pToVector(pToVector)
        {}

        void Render() override;

    private:
        blaVector<T>* m_pToVector;
    };

}