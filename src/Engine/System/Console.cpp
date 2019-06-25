#include "Console.h"
#pragma optimize("", off)

#define MAX(a,b) ((a > b) ? a : b)

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(Console);

void ConsoleLog::GetLastNLogLines(int n, std::vector<std::string>& outVector)
{
    for(int i = MAX((int)m_consoleLineCache.size() - n, 0);  i < m_consoleLineCache.size(); i++)
    {
        outVector.push_back(m_consoleLineCache[i]);
    }
}

void ConsoleLog::AddLine(const std::string& logLine)
{
    m_consoleLineCache.push_back(logLine);
}

void Console::GetLastNLines(int n, std::vector<std::string> &outVector)
{
    m_log.GetLastNLogLines(n, outVector);
}

void Console::LogMessage(const std::string& message)
{
    m_log.AddLine(message);
}

void Console::LogWarning(const std::string& warning)
{
    m_log.AddLine(warning);
}

void Console::LogError(const std::string& error)
{
    m_log.AddLine(error);
}

void Console::ExecuteCommand(const std::string& commandAndArguments)
{
    m_log.AddLine(commandAndArguments);

    // Do nothing else
}

