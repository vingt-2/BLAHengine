#include "Time.h"
using namespace BLAengine;

Time::Time(glm::uint8 timeBufferSize)
{
    m_timeBufferSize = m_timeBufferSize;
    m_time = 0;
    m_elapsedTime = 0;
}

Time::~Time()
{
    m_timeBuffer.~vector();
}

float Time::GetTime() const
{
    return m_time;
}

float Time::GetDelta() const
{
    return m_elapsedTime;
}

void Time::Update()
{
    float time = 0;

#ifdef GLFW_INTERFACE
    time = (float) glfwGetTime();
#endif

    m_elapsedTime = time - m_time;

    m_time = time;

    /*if (m_timeBuffer.size() == m_timeBufferSize)
    {
    m_timeBuffer.erase(m_timeBuffer.begin());
    }

    m_timeBuffer.push_back(time);*/
}

float Time::GetFramerate() const
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