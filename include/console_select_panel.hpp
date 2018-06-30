#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <switch.h>
#include "panel.hpp"

namespace menu
{
    struct SectionEntry
    {
        std::string name;
        std::function<void()> onSelected;

        SectionEntry(std::string name, std::function<void()> onSelected);
    };

    struct Section
    {
        std::string name;
        std::vector<std::shared_ptr<SectionEntry>> entries;

        Section(std::string name);

        void addEntry(std::string name, std::function<void()> onSelected);
    };

    class ConsoleSelectPanel : public Panel
    {
        public:
            const std::string m_title;
            std::vector<std::shared_ptr<Section>> m_sections;

            ConsoleSelectPanel(PrintConsole *console, std::string title);

            virtual void onBecomeActive() override;
            virtual void processInput(u64 keysDown) override;
            virtual void updateDisplay() override;

            std::shared_ptr<Section> addSection(std::string name);

        protected:
            PrintConsole *m_console;
            unsigned int m_cursorPosition;

            void moveCursor(int offset);
            unsigned int getCursorPosition();
            size_t getCursorSectionIndex(unsigned int cursorPosition);
            size_t getCursorSectionEntryIndex(unsigned int cursorPosition);
            unsigned int getCursorRowInConsole(unsigned int cursorPosition);
            void onChangeCursorPosition(unsigned int prevPos);

            size_t getTotalSectionEntries();
            size_t getSectionStartEntryOverallIndex(size_t sectionEntryIndex);
    };
}