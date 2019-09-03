#pragma once

#include <Common/DataStructures/Tree.h>
#include <Common/StdInclude.h>

namespace BLAengine
{
    class BlaGuiElement : public IntrusiveTree<BlaGuiElement>
    {
		blaString m_name;
    public:
		BlaGuiElement(const blaString& name) : m_name(name) {}

        // Todo: Set options for how to display children

		const blaString& GetName() const { return m_name; }

        virtual void Render();
    };

	class BlaGuiCollapsibleElement : public BlaGuiElement
	{
		blaString m_name;
	public:
		BlaGuiCollapsibleElement(const blaString& name) : BlaGuiElement(name) {}

		virtual void Render();
	};

	class BlaGuiCollapsibleHeaderElement : public BlaGuiElement
	{
		blaString m_name;
	public:
		BlaGuiCollapsibleHeaderElement(const blaString& name) : BlaGuiElement(name) {}

		virtual void Render();
	};

    class BlaGuiSimpleTextElement : public BlaGuiElement
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
}