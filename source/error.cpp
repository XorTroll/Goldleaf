#include "error.hpp"

#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>
#include "menu.hpp"

namespace error
{
    static std::vector<LogMsg> g_logMsgs;

    void log(LogLevel level, std::string location, std::string desc, Result code)
    {
        LogMsg msg { level, location, desc, code };
        g_logMsgs.push_back(msg);
    }

    void critical(std::string location, std::string desc, Result code)
    {
        auto panel = std::make_shared<ErrorPanel>(location, desc, code);
        menu::g_menu->pushPanel(panel);
    }

    ErrorPanel::ErrorPanel(std::string location, std::string desc, Result code) :
        m_location(location), m_desc(desc), m_code(code)
    {
    }

    void ErrorPanel::onBecomeActive()
    {
        consoleClear();
        std::cout << "A critical error has occurred." << std::endl;
        std::cout << "Location: " << this->m_location << std::endl;
        std::cout << "Desc: " << this->m_desc << std::endl;
        std::cout << "Code: " << std::setfill('0') << std::setw(sizeof(u32) * 2) << std::hex << this->m_code << std::endl;
        std::cout << "Log : " << std::endl;
        
        for (auto &msg : g_logMsgs)
        {
            std::string levelName;

            switch (msg.level)
            {
                case LogLevel::WARNING:
                    levelName = "[Warning]";
                    break;

                case LogLevel::ERROR:
                    levelName = "[Error]";
                    break;
            }

            std::cout << levelName << " " << msg.location << ": " << msg.desc << ". Error code: "  << std::setw(sizeof(u32) * 2) << std::hex << msg.code << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Press any key to exit." << std::endl; 
    }

    void ErrorPanel::processInput(u64 keysDown)
    {
        if (keysDown != 0)
            menu::g_menu->m_exitRequested = true;
    }

    void ErrorPanel::updateDisplay() {}
}