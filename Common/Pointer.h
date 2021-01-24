// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

template<typename T>
struct blaOwnedPtr
{
    blaOwnedPtr() { m_p = nullptr; }

    blaOwnedPtr(T* p) : m_p {}

    ~blaOwnedPtr() { SetPtr(nullptr); }

    void operator=(T* p) { SetPtr(p); }

    T& operator*() { return *m_p; }

    T* operator->() { return m_p; }

    operator T* () { return m_p; }
    operator bool() const { return m_p != nullptr; }
    void SetPtr(T* p)
    {
        if(m_p)
        {
            delete m_p;
        }
        m_p = p;
    }

    T* m_p;
};
