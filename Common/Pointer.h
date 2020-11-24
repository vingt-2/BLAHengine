// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.
#pragma once

template<typename T>
struct blaOwnedPtr
{
    blaOwnedPtr() { m_p = nullptr; }
    blaOwnedPtr(T* p) : m_p {}
    ~blaOwnedPtr() { SetPtr(nullptr); }
    operator=(T* p) { SetPtr(p); }

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
