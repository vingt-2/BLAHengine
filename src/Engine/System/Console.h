#pragma once

#include <Common/System.h>
#include <Common/StdInclude.h>
#include <Common/BLASingleton.h>
#include <Common/BLAtime.h>

namespace BLAengine
{
    class ConsoleCommand
    {
        blaString m_name;
        blaVector<blaString> m_arguments;
        blaString m_commandDescription;
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

        BLA_DECLARE_SINGLETON(Console);

        void GetLastNLines(int n, blaVector<blaString>& outVector);

        void InstanceLogMessage(const blaString& message);
        void InstanceLogWarning(const blaString& warning);
        void InstanceLogError(const blaString& error);

        void ExecuteCurrentCommand();

        void DoCommandCompletion();
        void DoCommandHistory(blaS32 cursorOffset);

    public:

        Console():
            m_historyCursor(0)
        {}

        static void LogMessage(const blaString& message) { GetSingletonInstance()->InstanceLogMessage(message); }
        static void LogWarning(const blaString& warning) { GetSingletonInstance()->InstanceLogWarning(warning); }
        static void LogError(const blaString& error) { GetSingletonInstance()->InstanceLogError(error); }

    private:
        ConsoleLog m_log;

        char m_currentCommandBuffer[2048];
        blaVector<blaString> m_commandHistory;
        blaS32 m_historyCursor;
    };
}