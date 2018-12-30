#pragma once
#include "..\..\Common\StdInclude.h"
#include "..\..\Common\System.h"
#include "..\..\Common\Maths\Maths.h"

namespace BLAengine
{
    class BLACORE_API Timer
    {
    public:
        Timer(glm::uint8 timeBufferSize);
        ~Timer();

        float GetTime() const;
        float GetDelta() const;
        float GetFramerate() const;

        void Update();

    private:

        float m_time;
        float m_elapsedTime;

        vector<float> m_timeBuffer;
        glm::uint8 m_timeBufferSize;
    };
}
