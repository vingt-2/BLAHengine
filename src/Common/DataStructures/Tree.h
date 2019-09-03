#pragma once

#include <Common/System.h>
#include <Common/BLAReference.h>
#include <Iterator>

template<class T>
class IntrusiveTree
{
public:

    class ChildrenIterator
    {
    public:
        typedef ChildrenIterator self_type;

        typedef std::forward_iterator_tag Iterator_category;
        typedef int difference_type;
        ChildrenIterator(T* ptr) : ptr_(ptr) { }
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

    virtual ~IntrusiveTree()
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
        if (m_child == nullptr)
        {
            m_child = childToAdd;
        }
        else
        {
            IntrusiveTree<T>* child = reinterpret_cast<IntrusiveTree<T>*>(m_child);

            while (child->m_next != nullptr)
            {
                child = reinterpret_cast<IntrusiveTree<T>*>(child->m_next);
            }
            child->m_next = childToAdd;
        }

        reinterpret_cast<IntrusiveTree<T>*>(childToAdd)->m_parent = reinterpret_cast<T*>(this);
    }

    blaBool AddChildAfterNode(T* childToAdd, T* previousNode)
    {
        if (m_child == nullptr)
        {
            return false;
        }

        if (m_child == previousNode)
        {
            m_next = childToAdd;
            reinterpret_cast<IntrusiveTree<T>*>(childToAdd)->m_parent = reinterpret_cast<T*>(this);

            return true;
        }
        
        IntrusiveTree<T>* child = reinterpret_cast<IntrusiveTree<T>*>(m_child);
        blaBool result = false;
        while (child->m_next != nullptr)
        {
            if (child->m_next == previousNode)
            {
                child = reinterpret_cast<IntrusiveTree<T>*>(child->m_next);

                T* previousNext = child->m_next;

                child->m_next = childToAdd;

                IntrusiveTree<T*> addedChild = reinterpret_cast<IntrusiveTree<T*>>(childToAdd);
                addedChild->m_next = previousNext;
                addedChild.m_parent = reinterpret_cast<T*>(this);
            }
        }

        return result;
    }

    T* GetNext()
    {
        return m_next;
    }

    const T* GetNext() const
    {
        return m_next;
    }

    T* GetParent()
    {
        return m_parent;
    }

    const T* GetParent() const
    {
        return m_parent;
    }

    T* GetUnderlying()
    {
        return reinterpret_cast<T*>(this);
    }

    blaU32 GetNumberChildren() const
    {
        auto child = reinterpret_cast<IntrusiveTree<T>*>(m_child);

        blaU32 count = 0;
        while (child != nullptr)
        {
            ++count;
            child = child->GetNext();
        }

        return count;
    }

    blaBool HasChild(T* value) const
    {
        auto child = reinterpret_cast<IntrusiveTree<T>*>(m_child);
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

    ChildrenIterator begin()
    {
        return ChildrenIterator(m_child);
    }

    ChildrenIterator end()
    {
        return ChildrenIterator(nullptr);
    }

    ChildrenIterator begin() const
    {
        return ChildrenIterator(m_child);
    }

    ChildrenIterator end() const
    {
        return ChildrenIterator(nullptr);
    }

protected:
    T* m_parent;
    T* m_next;
    T* m_child;
};