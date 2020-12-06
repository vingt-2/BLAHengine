// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "BLASingleton.h"
#include "StdInclude.h"
#include "System.h"
#include "Maths/Maths.h"
namespace BLA
{
    class Timer
    {
        BLA_DECLARE_EXPORTED_ACCESS_SINGLETON(Timer);

        float m_time;
        float m_elapsedTime;

        blaVector<float> m_timeBuffer;
        glm::uint8 m_timeBufferSize;

    public:
        // TODO: Don't export, this belongs to the engine
        BLACORE_API Timer(glm::uint8 timeBufferSize);
        // TODO: Don't export, this belongs to the engine
        BLACORE_API ~Timer();

        BLACORE_API float GetTime() const;
        BLACORE_API float GetDelta() const;
        BLACORE_API float GetFramerate() const;

        // TODO: Don't export, this belongs to the engine
        BLACORE_API void Update(blaF32 curTime);
    };
}
