#include "Console.h"
#pragma optimize("", off)

#define MAX(a,b) (a > b) ? a : b
#define MIN(a,b) (a < b) ? a : b

using namespace BLAengine;

BLA_IMPLEMENT_SINGLETON(Console);

void ConsoleLog::GetLastNLogLines(int n, blaVector<blaString>& outVector)
{
    outVector.resize(MIN(n, m_consoleLineCache.size()));
    for(int i = MAX((int)m_consoleLineCache.size() - n, 0), j = 0;  i < m_consoleLineCache.size(); i++, j++)
    {
        outVector[j] = m_consoleLineCache[i];
    }
}

void ConsoleLog::AddLine(const blaString& logLine)
{
    m_consoleLineCache.push_back(logLine);
}

void Console::GetLastNLines(int n, blaVector<blaString> &outVector)
{
    m_log.GetLastNLogLines(n, outVector);
}

void Console::InstanceLogMessage(const blaString& message)
{
    m_log.AddLine(message);
}

void Console::InstanceLogWarning(const blaString& warning)
{
    m_log.AddLine(warning);
}

void Console::InstanceLogError(const blaString& error)
{
    m_log.AddLine(error);
}

void Console::ExecuteCurrentCommand()
{
    blaString currentCommand = blaString(m_currentCommandBuffer);

    blaVector<blaString> args = SplitString<blaString>(currentCommand, " ");

    const blaString commandName = args[0];
	
	m_log.AddLine("> " + currentCommand);

    args.erase(args.begin());
    blaString ret = "";
    for(auto command : m_commandDictionary)
    {
        if (command->GetName() == commandName)
        {
            ret = command->Call(args);
        }
    }
    m_log.AddLine("> " + ret);
	m_commandHistory.emplace_back(currentCommand);
}

void Console::DoCommandCompletion()
{
    blaPair<int, const ConsoleCommandEntry*> bestMatch(0, nullptr);
    
    for (const ConsoleCommandEntry* command : m_commandDictionary)
    {
        int matchLength = 0;
        const blaString& commandName = command->GetName();
        for(int c = 0; c < commandName.length() && c < sizeof(m_currentCommandBuffer); ++c)
        {
            if (commandName.at(c) != m_currentCommandBuffer[c])
                break;
            ++matchLength;
        }
        if(matchLength > bestMatch.first)
        {
            bestMatch.first = matchLength;
            bestMatch.second = command;
        }
    }

    if(bestMatch.second != nullptr)
    {
        strcpy(m_currentCommandBuffer, bestMatch.second->GetName().data());
    }
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

void Console::RegisterConsoleCommand(const ConsoleCommandEntry* consoleCommand)
{
    m_commandDictionary.push_back(consoleCommand);
}

ConsoleCommandEntry::ConsoleCommandEntry(blaString name) : m_name(name)
{
    Console* c = Console::GetSingletonInstance();

    if (!c)
        c = Console::AssignAndReturnSingletonInstance(new Console());

    c->RegisterConsoleCommand(this);
}

template<typename T>
T BLAengine::blaFromString(const blaString& str)
{
    // unimplemented
    // static assert here (see code currently on my laptop ...)
    return T();
}

template<>
int BLAengine::blaFromString(const blaString& str)
{
    return std::stoi(str);
}

template<>
blaU32 BLAengine::blaFromString(const blaString& str)
{
    return std::stoi(str);
}

template<>
float BLAengine::blaFromString(const blaString& str)
{
    return std::stof(str);
}

template<>
blaString BLAengine::blaFromString(const blaString& str) 
{
    return str;
}

//TODO: Find a way to nest that templating ?
template<>
blaVector<blaString> BLAengine::blaFromString(const blaString& str)
{
    return SplitString<blaString>(str, " ");
}

template<>
blaVector<blaU32> BLAengine::blaFromString(const blaString& str)
{
    return SplitString<blaU32>(str, " ");
}

template<>
blaVector<blaS32> BLAengine::blaFromString(const blaString& str)
{
    return SplitString<blaS32>(str, " ");
}

template<>
blaVector<blaF32> BLAengine::blaFromString(const blaString& str)
{
    return SplitString<blaF32>(str, " ");
}

template<>
blaVector<blaBool> BLAengine::blaFromString(const blaString& str)
{
    return SplitString<blaBool>(str, " ");
}