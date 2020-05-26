// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "Console.h"
#include "BLAStringID.h"

#define MAX(a,b) (a > b) ? a : b
#define MIN(a,b) (a < b) ? a : b

using namespace BLA;

BLA_IMPLEMENT_SINGLETON(Console);

void ConsoleLog::GetLastNLogLines(int n, blaVector<blaString>& outVector)
{
    outVector.resize(MIN(n, m_consoleLineCache.size()));
    for (int i = MAX((int)m_consoleLineCache.size() - n, 0), j = 0; i < m_consoleLineCache.size(); i++, j++)
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
    for (auto library : m_commandDictionaryPerLibrary)
    {
        for (auto command : library.second)
        {
            if (command->GetName() == commandName)
            {
                ret = command->Call(args);
            }
        }
    }
    m_log.AddLine("> " + ret);
    m_commandHistory.emplace_back(currentCommand);
}

void Console::DoCommandCompletion()
{
    blaPair<int, const ConsoleCommandEntry*> bestMatch(0, nullptr);

    for (auto library : m_commandDictionaryPerLibrary)
    {
        for (const ConsoleCommandEntry* command : library.second)
        {
            int matchLength = 0;
            const blaString& commandName = command->GetName();
            for (int c = 0; c < commandName.length() && c < sizeof(m_currentCommandBuffer); ++c)
            {
                if (commandName.at(c) != m_currentCommandBuffer[c])
                    break;
                ++matchLength;
            }
            if (matchLength > bestMatch.first)
            {
                bestMatch.first = matchLength;
                bestMatch.second = command;
            }
        }
    }

    if (bestMatch.second != nullptr)
    {
        strcpy_s(m_currentCommandBuffer, bestMatch.second->GetName().data());
    }
}

void Console::DoCommandHistory(blaS32 cursorOffset)
{
    if (m_commandHistory.empty())
    {
        return;
    }

    m_historyCursor += cursorOffset;
    m_historyCursor = MAX(MIN(m_historyCursor, m_commandHistory.size() - 1), 0);
    strcpy_s(m_currentCommandBuffer, m_commandHistory[m_commandHistory.size() - m_historyCursor - 1].data());
}

void Console::RegisterConsoleCommand(const ConsoleCommandEntry* consoleCommand)
{
    m_commandDictionaryPerLibrary[m_currentRegisteringLibrary].push_back(consoleCommand);
}

void Console::UnloadConsoleCommandsForLibrary(blaU32 libraryId)
{
    for (auto library : m_commandDictionaryPerLibrary)
    {
        if(library.first == libraryId) 
        {
            library.second.clear();
            m_commandDictionaryPerLibrary.erase(library.first);
        }
    }
}

Console::Console() : m_historyCursor(0), m_currentRegisteringLibrary(BlaStringId("Native"))
{
    m_currentCommandBuffer[0] = 0;
}

void Console::LogMessage(const blaString& message)
{
	GetSingletonInstance()->InstanceLogMessage(message);
}

void Console::LogWarning(const blaString& warning)
{
	GetSingletonInstance()->InstanceLogWarning(warning);
}

void Console::LogError(const blaString& error)
{
	GetSingletonInstance()->InstanceLogError(error);
}

ConsoleCommandEntry::ConsoleCommandEntry(blaString name) : m_name(name)
{
    Console* c = Console::GetSingletonInstance();

    if (!c)
        c = Console::AssignAndReturnSingletonInstance(new Console());

    c->RegisterConsoleCommand(this);
}

template<typename T>
BLACORE_API T BLA::blaFromString(const blaString& str)
{
    // unimplemented
    // static assert here (see code currently on my laptop ...)
    return T();
}

template<>
BLACORE_API int BLA::blaFromString(const blaString& str)
{
    return std::stoi(str);
}

template<>
BLACORE_API blaU32 BLA::blaFromString(const blaString& str)
{
    return std::stoi(str);
}

template<>
BLACORE_API float BLA::blaFromString(const blaString& str)
{
    return std::stof(str);
}

template<>
BLACORE_API blaString BLA::blaFromString(const blaString& str)
{
    return str;
}

template<>
BLACORE_API bool BLA::blaFromString(const blaString& str)
{
    if (str == "true") return true;
    if (str == "1") return true;
    return false;
}

//TODO: Find a way to nest that templating ?
template<>
BLACORE_API blaVector<blaString> BLA::blaFromString(const blaString& str)
{
    return SplitString<blaString>(str, " ");
}

template<>
BLACORE_API blaVector<blaU32> BLA::blaFromString(const blaString& str)
{
    return SplitString<blaU32>(str, " ");
}

template<>
BLACORE_API blaVector<blaS32> BLA::blaFromString(const blaString& str)
{
    return SplitString<blaS32>(str, " ");
}

template<>
BLACORE_API blaVector<blaF32> BLA::blaFromString(const blaString& str)
{
    return SplitString<blaF32>(str, " ");
}

template<>
BLACORE_API blaVector<blaBool> BLA::blaFromString(const blaString& str)
{
    return SplitString<blaBool>(str, " ");
}