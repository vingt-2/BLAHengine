#include "Time.h"

Time::Time(uint8_t timeBufferSize)
{
	m_timeBufferSize = m_timeBufferSize;
	m_time = 0;
	m_elapsedTime = 0;
}

Time::~Time()
{
	m_timeBuffer.~vector();
}

double Time::GetTime()
{
	return m_time;
}

double Time::GetDelta()
{
	return m_elapsedTime;
}

void Time::Update()
{
	double time = 0;

#ifdef GLFW_INTERFACE
	time = glfwGetTime();
#endif

	m_elapsedTime = time - m_time;

	m_time = time;

	if (m_timeBuffer.size() == m_timeBufferSize)
	{
		m_timeBuffer.erase(m_timeBuffer.begin());
	}

	m_timeBuffer.push_back(time);
}

double Time::GetFramerate()
{
	if (m_timeBuffer.size() < 2)
		return 0.0;

	double averageDelta = 0.0;
	for (int i = 0; i < m_timeBuffer.size() - 1; i++)
	{
		averageDelta += (m_timeBuffer[i + 1] - m_timeBuffer[i]);
	}

	averageDelta /= m_timeBuffer.size();

	return 1.0 / averageDelta;
}