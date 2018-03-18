#pragma once
#include "..\..\Common\StdInclude.h"
#include "..\..\Common\System.h"
#include "..\..\Common\Maths.h"

namespace BLAengine
{
    class BLACORE_API Time
    {
    public:
        Time(uint8_t timeBufferSize);
        ~Time();

        double GetTime();
        double GetDelta();
        double GetFramerate();

        void Update();

    private:

        double m_time;
        double m_elapsedTime;

        vector<double> m_timeBuffer;
        uint8_t m_timeBufferSize;
    };
}
