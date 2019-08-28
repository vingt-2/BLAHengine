#pragma once

#include <Common/System.h>
#include <Common/StdInclude.h>
#include <Common/BLASingleton.h>
#include <Common/BLAtime.h>
#include <Common/SillyMacros.h>

#define GET_ARGUMENT_I(i) arguments[i])
#define TO_STRING_MACRO( i ) blaFromString< EAT_VAR_NAME(i) > LPAREN

//TODO: For now, commands required a non void return type and at least one argument, will polish later
#define DEFINE_CONSOLE_COMMAND(RetType, CommandName, ...)                                                                                         \
    RetType CommandName(__VA_ARGS__);                                                                                                             \
    struct ConsoleCommandEntry_##CommandName : BLAengine::ConsoleCommandEntry                                                                     \
    {                                                                                                                                             \
        ConsoleCommandEntry_##CommandName() : ConsoleCommandEntry(#CommandName) {}                                                                \
        blaString Call(const blaVector<blaString>& arguments) const override                                                                      \
        {                                                                                                                                         \
            if(arguments.size() != IIF(IS_EMPTY(__VA_ARGS__))(0,SIZE(__VA_ARGS__)))                                                                                             \
            {                                                                                                                                     \
                Console::LogError("Insufficient number of arguments provided to " + m_name + ", expecting " + std::to_string(IIF(IS_EMPTY(__VA_ARGS__))(0,SIZE(__VA_ARGS__)))); \
                return "";                                                                                                                        \
            }                                                                                                                                     \
            return std::to_string(CommandName(IIF(IS_EMPTY(__VA_ARGS__))( ,ENUM_WITH_PREFIX_MACRO(TO_STRING_MACRO, GET_ARGUMENT_I, __VA_ARGS__)))); \
        }                                                                                                                                         \
        static ConsoleCommandEntry_##CommandName Init;                                                                                            \
    };                                                                                                                                            \
    ConsoleCommandEntry_##CommandName ConsoleCommandEntry_##CommandName::Init;                                                                    \
    RetType CommandName(__VA_ARGS__)                                                                                                              

namespace BLAengine
{
    /*
     * Todo: It'll be great if we can have ConsoleCommandDefinition to Component instances (as a seperate type of command) ...
     */

    template<typename T>
    T blaFromString(const blaString& str);

    //template<template<class> class T, typename S>
    //T<S> blaFromString(const blaString& str);

    class ConsoleCommandEntry
    {
    protected:
        ConsoleCommandEntry(blaString name);

        blaString m_name;
        blaString m_commandDescription;
    
    public:
        virtual blaString Call(const blaVector<blaString>& arguments) const = 0;
        const blaString& GetName() const { return m_name; };
    };

    class ConsoleLog
    {
        blaVector<blaString> m_consoleLineCache;
        blaU64 m_sessionId;
        blaTime m_sessionTime;

        void FlushToFile() {};
    public:

        void GetLastNLogLines(int n, blaVector<blaString> &outVector);
        void AddLine(const blaString& logLine);
    };

    class Console
    {
        friend class BlaGuiConsole;
        friend struct ConsoleCommandEntry;

        BLA_DECLARE_SINGLETON(Console);

        void GetLastNLines(int n, blaVector<blaString>& outVector);

        void InstanceLogMessage(const blaString& message);
        void InstanceLogWarning(const blaString& warning);
        void InstanceLogError(const blaString& error);

        void ExecuteCurrentCommand();

        void DoCommandCompletion();
        void DoCommandHistory(blaS32 cursorOffset);

        void RegisterConsoleCommand(const ConsoleCommandEntry* consoleCommand);

    public:

        Console():
            m_historyCursor(0)
        {}

        static void LogMessage(const blaString& message) { GetSingletonInstance()->InstanceLogMessage(message); }
        static void LogWarning(const blaString& warning) { GetSingletonInstance()->InstanceLogWarning(warning); }
        static void LogError(const blaString& error) { GetSingletonInstance()->InstanceLogError(error); }

    private:
        ConsoleLog m_log;

        blaVector<const ConsoleCommandEntry*> m_commandDictionary;

        char m_currentCommandBuffer[2048];
        blaVector<blaString> m_commandHistory;
        blaS32 m_historyCursor;
    };
}