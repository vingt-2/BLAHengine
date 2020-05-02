#pragma once
#include "BLASingleton.h"
#include "StdInclude.h"
#include "System.h"
#include "Maths/Maths.h"
namespace BLA
{
    class BLACORE_API Timer
    {
        BLA_DECLARE_SINGLETON(Timer);

        float m_time;
        float m_elapsedTime;

        blaVector<float> m_timeBuffer;
        glm::uint8 m_timeBufferSize;

    public:
        Timer(glm::uint8 timeBufferSize);
        ~Timer();

        float GetTime() const;
        float GetDelta() const;
        float GetFramerate() const;

        void Update(blaF32 newTime);
    };
}
