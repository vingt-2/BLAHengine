#include "Timer.h"
using namespace BLA;

Timer::Timer(glm::uint8 timeBufferSize)
{
    m_timeBufferSize = m_timeBufferSize;
    m_time = 0;
    m_elapsedTime = 0;
}

Timer::~Timer()
{
    m_timeBuffer.~vector();
}

float Timer::GetTime() const
{
    return m_time;
}

float Timer::GetDelta() const
{
    return m_elapsedTime;
}

void Timer::Update(blaF32 systemTime)
{
    m_elapsedTime = systemTime - m_time;

    m_time = systemTime;

    /*if (m_timeBuffer.size() == m_timeBufferSize)
    {
    m_timeBuffer.erase(m_timeBuffer.begin());
    }

    m_timeBuffer.push_back(time);*/
}

float Timer::GetFramerate() const
{
    if (m_timeBuffer.size() < 2)
        return 0.0f;

    float averageDelta = 0.0f;
    for (size_t i = 0; i < m_timeBuffer.size() - 1; i++)
    {
        averageDelta += (m_timeBuffer[i + 1] - m_timeBuffer[i]);
    }

    averageDelta /= m_timeBuffer.size();

    return 1.0f / averageDelta;
}