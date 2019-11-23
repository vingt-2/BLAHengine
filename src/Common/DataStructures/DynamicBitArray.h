#include <Common/System.h>

class DynamicBitArray
{
private:
    blaU32* m_buffer;
    blaU32 m_size;

public:
    DynamicBitArray(blaU32 size) :
        m_size(size)
    {
        m_buffer = (blaU32*)malloc(size / sizeof(blaU32));

        ClearAll();
    }

    ~DynamicBitArray()
    {
        delete m_buffer;
    }

    void ClearAll()
    {
        blaU32 i;
        for (i = 0; i < m_size; i++)
        {
            m_buffer[i] = 0;
        }
    }

    void SetAll()
    {
        blaU32 i;
        for (i = 0; i < m_size; i++)
        {
            m_buffer[i] = 1;
        }
    }

    blaBool IsBitSet(blaU32 bit)
    {
        //TODO: Add Check boundaries with trap system...
        return m_buffer[bit / sizeof(blaU32)] & 1 << bit % sizeof(blaU32);
    }

    void Set(blaU32 bit)
    {
        //TODO: Add Check boundaries with trap system...
        m_buffer[bit / sizeof(blaU32)] |= (1 << bit % sizeof(blaU32));
    }

    blaBool Clear(blaU32 bit)
    {
        //TODO: Add Check boundaries with trap system...
        return m_buffer[bit / sizeof(blaU32)] &= ~(1 << bit % sizeof(blaU32));
    }
};