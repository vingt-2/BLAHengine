#include "Console.h"
#pragma optimize("", off)

#define MAX(a,b) (a > b) ? a : b
#define MIN(a,b) (a < b) ? a : b

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(Console);

void ConsoleLog::GetLastNLogLines(int n, std::vector<std::string>& outVector)
{
    outVector.resize(MIN(n, m_consoleLineCache.size()));
    for(int i = MAX((int)m_consoleLineCache.size() - n, 0), j = 0;  i < m_consoleLineCache.size(); i++, j++)
    {
        outVector[j] = m_consoleLineCache[i];
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

void Console::InstanceLogMessage(const std::string& message)
{
    m_log.AddLine(message);
}

void Console::InstanceLogWarning(const std::string& warning)
{
    m_log.AddLine(warning);
}

void Console::InstanceLogError(const std::string& error)
{
    m_log.AddLine(error);
}

void Console::ExecuteCurrentCommand()
{
    std::string currentCommand = std::string(m_currentCommandBuffer);
    m_commandHistory.emplace_back(currentCommand);
    m_log.AddLine("> " + currentCommand);

    //Do Nothing else
}

void Console::DoCommandCompletion()
{

}

void Console::DoCommandHistory(blaS32 cursorOffset)
{
    if(m_commandHistory.empty())
    {
        return;
    }

    m_historyCursor += cursorOffset;
    m_historyCursor = MAX(MIN(m_historyCursor, m_commandHistory.size() - 1), 0);
    strcpy_s(m_currentCommandBuffer, m_commandHistory[m_commandHistory.size() - m_historyCursor - 1].data());
}
