#pragma once

#include <Common/System.h>
#include <Common/StdInclude.h>
#include <Common/BLASingleton.h>
#include <Common/BLAtime.h>

namespace BLAengine
{
    class ConsoleCommand
    {
        std::string m_name;
        std::vector<std::string> m_arguments;
        std::string m_commandDescription;
    };

    class ConsoleLog
    {
        std::vector<std::string> m_consoleLineCache;
        blaU64 m_sessionId;
        blaTime m_sessionTime;

        void FlushToFile() {};
    public:

        void GetLastNLogLines(int n, std::vector<std::string> &outVector);
        void AddLine(const std::string& logLine);
    };

    class Console
    {
        friend class BlaGuiConsole;

        BLA_DECLARE_SINGLETON(Console);

        void GetLastNLines(int n, std::vector<std::string>& outVector);

        void InstanceLogMessage(const std::string& message);
        void InstanceLogWarning(const std::string& warning);
        void InstanceLogError(const std::string& error);

        void ExecuteCurrentCommand();

        void DoCommandCompletion();
        void DoCommandHistory(blaS32 cursorOffset);

    public:

        Console():
            m_historyCursor(0)
        {}

        static void LogMessage(const std::string& message) { GetSingletonInstance()->InstanceLogMessage(message); }
        static void LogWarning(const std::string& warning) { GetSingletonInstance()->InstanceLogWarning(warning); }
        static void LogError(const std::string& error) { GetSingletonInstance()->InstanceLogError(error); }

    private:
        ConsoleLog m_log;

        char m_currentCommandBuffer[2048];
        std::vector<std::string> m_commandHistory;
        blaS32 m_historyCursor;
    };
}