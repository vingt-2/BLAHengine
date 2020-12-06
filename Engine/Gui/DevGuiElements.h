// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "IntrusiveTree.h"
#include "StdInclude.h"
#include "BLAStringID.h"

//TODO: Hide Render() from the element managers

namespace BLA
{
    class DevGuiElement;

    struct BlaDroppablePayload
    {
        DevGuiElement* m_receiverElement = nullptr;
        DevGuiElement* m_droppedElement = nullptr;
    };

    struct DevGuiElementEventPayload
    {
        enum EventType
        {
            SELECTED = 1 << 0,
            DOUBLE_CLICKED = 1 << 1,
            ELEMENT_DROPPED = 1 << 2,
        } m_eventType;

        void* m_pEventPayload;
    };

    typedef void(*EventCallBack)(void*, const DevGuiElementEventPayload&);

    struct DevGuiRegisteredEvents
    {
        blaU32 m_eventTriggerFlags = 0;
        EventCallBack m_callback = nullptr;
        void* m_callerPtr = nullptr;
    };

    class BLACORE_API DevGuiElement : public IntrusiveTree<DevGuiElement>
    {
        blaString m_name;

        blaVector<DevGuiRegisteredEvents> m_registeredCallbacks;

    protected:
        void SendEvent(DevGuiElementEventPayload::EventType eventType, void* pEventPayload);

        void HandleDragDropOfElements();

    public:
        DevGuiElement(const blaString& name, blaStringId groupId) :
        m_name(name),
        m_isSelected(false),
        m_groupId(groupId),
        m_isDragable(false),
        m_isDroppable(false) {}

        // Todo: Set options for how to display children

        const blaString& GetName() const { return m_name; }

        void RegisterEvents(DevGuiRegisteredEvents& cb);
        void UnRegisterEvents(DevGuiRegisteredEvents& cb);

        virtual void Render();

        bool m_isDragable;
        bool m_isDroppable;
        bool m_isSelected;

        blaStringId m_groupId;
    };

    class BLACORE_API DevGuiCollapsibleElement : public DevGuiElement
    {
    public:
        DevGuiCollapsibleElement(const blaString& name, blaStringId groupId);

        bool m_decorateHeader = false;
        
        virtual void Render();
    };

    class BLACORE_API DevGuiCollapsibleHeaderElement : public DevGuiElement
    {
    public:
        DevGuiCollapsibleHeaderElement(const blaString& name, blaStringId groupId) : DevGuiElement(name, groupId) {}

        virtual void Render();
    };

    class BLACORE_API DevGuiSimpleTextElement : public DevGuiElement
    {
    public:
        DevGuiSimpleTextElement(const blaString& name, blaStringId groupId, const blaString& text) :
            DevGuiElement(name, groupId),
            m_text(text) {}

        void Render() override;

    private:
        blaString m_text;
    };

    template<typename T>
    class DevGuiEditElement : public DevGuiElement
    {
    public:
        DevGuiEditElement(const blaString& name, blaStringId groupId,
            blaLambda<void(const char*, const char*, blaSize)> onEditFunctor, T* pToValue) :
            DevGuiElement(name, groupId),
            m_pToValue(pToValue),
            m_onEditFunctor(onEditFunctor)
        {}

        BLACORE_API void Render() override;

    private:
        T* m_pToValue;
        blaLambda<void(const char*, const char*, blaSize)> m_onEditFunctor;
    };

    BLACORE_API bool DevGuiEditElementVectorPreRender(DevGuiElement* element);
    BLACORE_API void DevGuiEditElementVectorPostRender(DevGuiElement* element);
    
    template<typename T>
    class DevGuiEditElementVector : public DevGuiElement
    {
    public:
        DevGuiEditElementVector(const blaString& name, blaStringId groupId, blaVector<T>* pToVector) :
            DevGuiElement(name, groupId),
            m_pToVector(pToVector)
        {}

        void Render() override
        {
            if (DevGuiEditElementVectorPreRender(this))
            {
                for (int i = 0; i < m_pToVector->size(); i++)
                {
                    DevGuiEditElement<T> toRender(std::to_string(i), m_groupId, [](const char*, const char*, blaSize){}, static_cast<T*>(&((*m_pToVector)[0])) + i);
                    toRender.Render();
                }
                DevGuiEditElementVectorPostRender(this);
            }
        }

    private:
        
        blaVector<T>* m_pToVector;
    };

    template<typename T1, typename T2>
    class DevGuiEditElementPair : public DevGuiElement
    {
    public:
        DevGuiEditElementPair(const blaString& name, blaStringId groupId, blaPair<T1, T2>* pToPair) :
            DevGuiElement(name, groupId),
            m_pToPair(pToPair)
        {}

        void Render() override;

    private:
        blaPair<T1,T2>* m_pToPair;
    };
}
