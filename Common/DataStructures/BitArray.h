#pragma once

#include "../System.h"
#define INTEGER_SIZE_BITS 32

template<int size>
class BitArray
{
private:
    blaU32 m_buffer[1 + (size / INTEGER_SIZE_BITS)];

public:
    BitArray()
    {
        ClearAll();
    }

    BitArray(const BitArray& other)
    {
        for (blaU32 i = 0; i <= size / INTEGER_SIZE_BITS; ++i)
        {
            m_buffer[i] = other.m_buffer[i];
        }
    }

    template<int otherSize>
    BitArray& operator=(const BitArray<otherSize>& other)
    {
        if (size != otherSize)
        {
            // TRAP:
            return nullptr;
        }
        // check for self-assignment
        if (&other == this)
            return *this;

        for (blaU32 i = 0; i <= size / INTEGER_SIZE_BITS; ++i)
        {
            m_buffer[i] = other.m_buffer[i];
        }
        return *this;
    }

    void ClearAll()
    {
        blaU32 i;
        for (i = 0; i <= size / INTEGER_SIZE_BITS; i++)
        {
            m_buffer[i] = 0;
        }
    }

    void SetAll()
    {
        blaU32 i;
        for (i = 0; i <= size / INTEGER_SIZE_BITS; i++)
        {
            m_buffer[i] = 1;
        }
    }

    blaBool IsBitSet(blaU32 bit) const
    {
        //TODO: Add Check boundaries with trap system...
        return m_buffer[bit / INTEGER_SIZE_BITS] & 1 << bit % INTEGER_SIZE_BITS;
    }

    void Set(blaU32 bit)
    {
        //TODO: Add Check boundaries with trap system...
        m_buffer[bit / INTEGER_SIZE_BITS] |= (1 << bit % INTEGER_SIZE_BITS);
    }

    blaBool Clear(blaU32 bit)
    {
        //TODO: Add Check boundaries with trap system...
        return m_buffer[bit / INTEGER_SIZE_BITS] &= ~(1 << bit % INTEGER_SIZE_BITS);
    }
};

//! Fast bits reference might be useful for later:
//
//Sometimes you want to walk the set bits of an integer and do something for each bit.For example, consider printing a human readable version of a flag field(which could have up to say 32 unique flags set).
//
//The naive way to accomplish this is of course to visit every bit and see if something is set.But we’re cooler than that.
//
//A better approach is to repeatedly pick off the lowest set bit until there’s nothing left.Here’s what that looks like :
//
//while (bits) {
//    uint32_t lsb = bits & ~(bits - 1);
//    // do something with lsb here..
//    bits &= ~lsb; // mask off lsb and keep going
//}
//
//This gives you the value of each bit as you’re walking it(i.e.bit 7 will have the value 0x80 if we’re counting from 0).If you need the bit’s position as well, it’s convenient to use the Intel BSF(bit scan forward) instruction via compiler intrinsics :
//
//int bitpos;
//while (0 != (bitpos = __builtin_ffs(bits))) {
//    uint32_t lsb = 1 << (bitpos - 1);
//    // use bitpos, lsb as required..
//    // bitpos is 1-based on GCC
//    bits &= ~lsb;
//}
//
//It sort of sucks that we still have to compute the LSB value so we can remove it and move on to the next bit.If your CPU has a fast CTZ instructions(count trailing zeroes) we can substitute that for the barrel shift :
//
//while (bits) {
//    uint32_t lsb = bits & ~(bits - 1);
//    int bitpos = __builtin_ctz(lsb);
//    // use bitpos, lsb as required..
//    bits &= ~lsb; // mask off lsb and keep going
//}
//
//The intrinsics shown here work on GCC and Clang.There are similar ones for MSVC but I’m too lazy to write Windows examples!
