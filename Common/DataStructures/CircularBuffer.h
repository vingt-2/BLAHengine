#pragma once

#include "../System.h"

#define INTEGER_SIZE_BITS 32

template<typename T, size_t size>
class CircularBuffer
{
    T m_buffer[size];
    int m_head;
    bool m_filled;

public:

    CircularBuffer() : m_filled(false), m_head(-1) {}

    T Peek(int i) const
    {
        BLA_ASSERT(m_filled ? size - i : m_head - i > 0);
        int index = (m_head - i);
        if (index < 0) index = size + index;
        return m_buffer[index];
    }

    void Push(T v)
    {
        m_head++;
        if (m_head == size) m_head = 0;
        if(!m_filled) m_filled = (bool)(m_head / (size - 1));
        m_buffer[m_head] = v;
    }

    size_t GetCount() const
    {
        return m_filled ? size : m_head;
    }
};