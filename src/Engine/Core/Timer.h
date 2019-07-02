#pragma once
#include <Common/BLASingleton.h>
#include <Common/StdInclude.h>
#include <Common/System.h>
#include <Common/Maths/Maths.h>
namespace BLAengine
{
    class BLACORE_API Timer
    {
        BLA_DECLARE_SINGLETON(Timer);
        
    public:
        Timer(glm::uint8 timeBufferSize);
        ~Timer();

        float GetTime() const;
        float GetDelta() const;
        float GetFramerate() const;

        void Update(blaF32 systemTime);

    private:

        float m_time;
        float m_elapsedTime;

        blaVector<float> m_timeBuffer;
        glm::uint8 m_timeBufferSize;
    };
}
