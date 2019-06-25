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
        BLA_DECLARE_SINGLETON(Console);

        std::string m_currentCommand;

        ConsoleLog m_log;

    public:

        void GetLastNLines(int n, std::vector<std::string>& outVector);

        void LogMessage(const std::string& message);
        void LogWarning(const std::string& warning);
        void LogError(const std::string& error);

        void ExecuteCommand(const std::string& commandAndArguments);
    };
}