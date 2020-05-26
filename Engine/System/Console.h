// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "System.h"
#include "StdInclude.h"
#include "BLASingleton.h"
#include "BLAtime.h"
#include "SillyMacros.h"

#define __SILLY_MACRO__COMPARE_void(x) x

#define nothing(...)  

#define __BLA_CC_DECLARATION_MACRO__IF_RETTYPE_NOT_VOID_1(CommandName, ...)                                 \
return std::to_string(CommandName(EXPAND(__SILLY_MACRO__IIF(IS_EMPTY(__VA_ARGS__))( nothing(__VA_ARGS__),   \
    EXPAND(ENUM_WITH_PREFIX_MACRO(FROM_STRING_MACRO, GET_ARGUMENT_I, __VA_ARGS__))))));  

#define __BLA_CC_DECLARATION_MACRO__IF_RETTYPE_NOT_VOID_0(CommandName, ...)             \
        CommandName(EXPAND(__SILLY_MACRO__IIF(IS_EMPTY(__VA_ARGS__))(nothing(__VA_ARGS__) ,EXPAND(ENUM_WITH_PREFIX_MACRO(FROM_STRING_MACRO, GET_ARGUMENT_I, __VA_ARGS__))))); return "";  

#define __BLA_CC_DECLARATION_MACRO_SELECT_RETTYPE(RetType) \
    __SILLY_MACRO__CAT(__BLA_CC_DECLARATION_MACRO__IF_RETTYPE_NOT_VOID_,__SILLY_MACRO__NOT_EQUAL(RetType, void))

#define DefineConsoleCommand(RetType, CommandName, ...)                                                        \
    RetType CommandName(__VA_ARGS__);                                                                          \
    struct ConsoleCommandEntry_##CommandName : BLA::ConsoleCommandEntry                                        \
    {                                                                                                          \
        ConsoleCommandEntry_##CommandName() : ConsoleCommandEntry(#CommandName) {}                             \
        blaString Call(const blaVector<blaString>& arguments) const override                                   \
        {                                                                                                      \
            if(arguments.size() != __SILLY_MACRO__IIF(IS_EMPTY(__VA_ARGS__))(0,SIZE(__VA_ARGS__)))             \
            {                                                                                                  \
                Console::LogError("Inadequate number of arguments provided to " + m_name + ", expecting " +    \
                    std::to_string(__SILLY_MACRO__IIF(IS_EMPTY(__VA_ARGS__))(0,SIZE(__VA_ARGS__))));           \
                return "";                                                                                     \
            }                                                                                                  \
            EXPAND(__BLA_CC_DECLARATION_MACRO_SELECT_RETTYPE(RetType))(CommandName, __VA_ARGS__)               \
        }                                                                                                      \
        static ConsoleCommandEntry_##CommandName Init;                                                         \
    };                                                                                                         \
    ConsoleCommandEntry_##CommandName ConsoleCommandEntry_##CommandName::Init;                                 \
    RetType CommandName(__VA_ARGS__)                                                                                                              

#define DeclareConsoleVariable(type, name, defaultValue)                        \
    type g_##name = defaultValue;                                               \
    DefineConsoleCommand(int, name, type value) { g_##name = value; return 0; } \

namespace BLA
{
    /*
     * Todo: It'll be great if we can have ConsoleCommandDefinition to Component instances (as a seperate type of command) ...
     */

    template<typename T>
    BLACORE_API T blaFromString(const blaString& str);

    //template<template<class> class T, typename S>
    //T<S> blaFromString(const blaString& str);

    class BLACORE_API ConsoleCommandEntry
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
        friend class ConsoleCommandEntry;
        friend class ComponentLibrariesManager;

        BLA_DECLARE_EXPORTED_ACCESS_SINGLETON(Console);

        void GetLastNLines(int n, blaVector<blaString>& outVector);

        void InstanceLogMessage(const blaString& message);
        void InstanceLogWarning(const blaString& warning);
        void InstanceLogError(const blaString& error);

        void ExecuteCurrentCommand();

        void DoCommandCompletion();
        void DoCommandHistory(blaS32 cursorOffset);

        void RegisterConsoleCommand(const ConsoleCommandEntry* consoleCommand);

        void UnloadConsoleCommandsForLibrary(blaU32 libraryId);

    public:

        Console();

        BLACORE_API static void LogMessage(const blaString& message);
        BLACORE_API static void LogWarning(const blaString& warning);
        BLACORE_API static void LogError(const blaString& error);

    private:
        ConsoleLog m_log;

        blaMap<blaU32, blaVector<const ConsoleCommandEntry*>> m_commandDictionaryPerLibrary;

        char m_currentCommandBuffer[2048];
        blaVector<blaString> m_commandHistory;
        blaIndex m_historyCursor;

        blaU32 m_currentRegisteringLibrary;
    };

    template<typename T>
    blaVector<T> SplitString(blaString str, const blaString& delimiter)
    {
        blaVector<T> ret;
        blaIndex i = str.find_first_of(delimiter);
        while (i != blaString::npos)
        {
            ret.push_back(blaFromString<T>(str.substr(0, i)));
            str = str.substr(i + delimiter.length(), str.length() - 1);
            i = str.find_first_of(delimiter);
        }

        if (!str.empty())
        {
            ret.push_back(blaFromString<T>(str));
        }

        return ret;
    }

}