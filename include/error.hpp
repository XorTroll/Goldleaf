#pragma once

#include <string>
#include <switch.h>
#include "panel.hpp"

namespace error
{
    enum class LogLevel
    {
        WARNING, ERROR
    };

    struct LogMsg
    {
        LogLevel level;
        std::string location;
        std::string desc;
        Result code;
    };

    void log(LogLevel level, std::string location, std::string desc, Result code);
    void critical(std::string location, std::string desc, Result code);

    class ErrorPanel : public menu::Panel
    {
        public:
            ErrorPanel(std::string location, std::string desc, Result code);

            void onBecomeActive() override;
            void processInput(u64 keysDown) override;
            void updateDisplay() override;
    
        private:
            const std::string m_location;
            const std::string m_desc;
            const Result m_code;

            std::string getErrorCodeDesc();
    };
}