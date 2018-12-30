#pragma once
#include "../System.h"
#include <Iterator>

template<class T>
class IntrusiveTree
{
public:

	class Iterator
	{
	public:
		typedef Iterator self_type;

		typedef std::forward_iterator_tag Iterator_category;
		typedef int difference_type;
		Iterator(T* ptr) : ptr_(ptr) { }
		self_type operator++() { self_type i = *this; ptr_ = static_cast<IntrusiveTree<T>*>(ptr_)->GetNext(); return i; }
		self_type operator++(int junk) { ptr_ = static_cast<IntrusiveTree<T>*>(ptr_)->GetNext(); return *this; }
		T& operator*() { return *ptr_; }
		T* operator->() { return ptr_; }
		bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
		bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
	private:
		T* ptr_;
	};

	IntrusiveTree():
		m_child(nullptr), m_next(nullptr)
	{};

	IntrusiveTree(IntrusiveTree<T>& otherIntrusiveTree):
		m_child(otherIntrusiveTree.GetChild()), m_next(otherIntrusiveTree.GetNext())
	{};

	~IntrusiveTree()
	{
		auto child = m_child;
		while(child != nullptr)
		{
			auto nextChild = child->m_next;
			delete child;
			child = nextChild;
		}
	}

	T* GetChild()
	{
		return m_child;
	}

	const T* GetChild() const
	{
		return m_child;
	}

	void AddChild(T* childToAdd)
	{
		IntrusiveTree<T>* child = m_child;

		if(child == nullptr)
		{
			m_child = childToAdd;
			return;
		}

		while (child->m_next != nullptr)
		{
			child = child->m_next;
		}
		child->m_next = childToAdd;
	}

	T* GetNext()
	{
		return m_next;
	}

	const T* GetNext() const
	{
		return m_next;
	}

	T* GetUnderlying()
	{
		return static_cast<T*>(this);
	}

	blaU32 GetNumberChildren()
	{
		auto child = static_cast<IntrusiveTree<T>*>(m_child);

		blaU32 count = 0;
		while (child != nullptr)
		{
			++count;
			child = child->GetNext();
		}

		return count;
	}

	blaBool HasChild(T* value)
	{
		auto child = static_cast<IntrusiveTree<T>*>(m_child);
		while (child != nullptr)
		{
			if (child == value)
			{
				return true;
			}
			child = child->m_next;
		}
		return false;
	}

	Iterator begin()
	{
		return Iterator(m_child);
	}

	Iterator end()
	{
		return Iterator(nullptr);
	}

	Iterator begin() const
	{
		return Iterator(m_child);
	}

	Iterator end() const
	{
		return Iterator(nullptr);
	}

protected:
	T* m_next;
	T* m_child;
};

//template<class T>
//class TreeIterator
//{
//	// ... state variables ...
//
//public:
//	// Constructor
//	TreeIterator(IntrusiveTree<T>* ptr)
//	{
//		return ptr;
//	}
//
//	TreeIterator& operator++() { /* increment */ return (*this).m_next; }
//
//	IntrusiveTree<T> operator*() const { return *this; }
//
//	bool operator!= (const TreeIterator& o) const
//	{
//		return this == o;
//	}
//};
//
