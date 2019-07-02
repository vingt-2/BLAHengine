#pragma once

#include <Common/DataStructures/Tree.h>

#include <Common/Maths/Maths.h>

namespace BLAengine
{
    class BlaGuiElement : public IntrusiveTree<BlaGuiElement>
    {
    public:
        BlaGuiElement() = default;
        ~BlaGuiElement();


        // Todo: Set options for how to display children

        virtual void Render();
    };

    class BlaGuiTextElement : public BlaGuiElement
    {
    public:
        explicit BlaGuiTextElement(const blaString& text)
            : m_text(text)
        {}

        void Render() override;

    private:
        blaString m_text;
    };
}