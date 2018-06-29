#include "console_panel.hpp"

#include <iostream>

namespace menu
{
    //
    // SectionEntry
    //
    SectionEntry::SectionEntry(std::string name, std::function<void()> onSelected) :
        name(name), onSelected(onSelected)
    {

    }

    //
    // Section
    //
    Section::Section(std::string name) :
        name(name)
    {
    }

    void Section::addEntry(std::string name, std::function<void()> onSelected)
    {
        this->entries.push_back(std::make_shared<SectionEntry>(name, onSelected));
    }

    //
    // ConsolePanel
    //

    ConsolePanel::ConsolePanel(PrintConsole *console, std::string title) :
        m_title(title), m_console(console), m_cursorPosition(0)
    {
        
    }

    // When the panel first becomes active we want a fresh start
    void ConsolePanel::onBecomeActive()
    {
        consoleClear();
        size_t overallEntryIndex = 0;

        // Iterate over all of the sections
        for (auto &section : this->m_sections)
        {
            std::cout << std::endl << section->name << std::endl << "----------" << std::endl;

            for (size_t i = 0; i < section->entries.size(); i++)
            {
                auto entry = section->entries[i];

                if (overallEntryIndex == this->getCursorPosition())
                    std::cout << "> " << entry->name << std::endl;
                else
                    std::cout << "  " << entry->name << std::endl;

                overallEntryIndex++;
            }
        }
    }

    void ConsolePanel::processInput(Menu *menu, u64 keysDown)
    {
        if (keysDown & KEY_UP || keysDown & KEY_DOWN)
        {
            if (keysDown & KEY_UP)
                this->moveCursor(-1);

            if (keysDown & KEY_DOWN)
                this->moveCursor(1); 
        }
        else if (keysDown & KEY_A)
        {
            auto section = this->m_sections[this->getCursorSectionIndex(this->m_cursorPosition)];
            auto sectionEntry = section->entries[this->getCursorSectionEntryIndex(this->m_cursorPosition)];
            sectionEntry->onSelected();
        }
        else if (keysDown & KEY_B)
        {
            menu->back();
        }
    }

    void ConsolePanel::updateDisplay() {}

    std::shared_ptr<Section> ConsolePanel::addSection(std::string name)
    {
        auto section = std::make_shared<Section>(name);
        this->m_sections.push_back(section);
        return section;
    }

    void ConsolePanel::moveCursor(int offset)
    {
        size_t totalEntries = getTotalSectionEntries();
        unsigned int prevCursorPos = this->m_cursorPosition;
        this->m_cursorPosition = std::max(std::min(static_cast<int>(this->m_cursorPosition + offset), static_cast<int>(totalEntries - 1)), 0); 
        
        if (prevCursorPos != this->m_cursorPosition)
            this->onChangeCursorPosition(prevCursorPos);
    }

    unsigned int ConsolePanel::getCursorPosition()
    {
        return this->m_cursorPosition;
    }

    size_t ConsolePanel::getCursorSectionIndex(unsigned int cursorPosition)
    {
        size_t overallEntryIndex = 0;

        // Iterate over all of the sections
        for (size_t i = 0; i < this->m_sections.size(); i++)
        {
            std::shared_ptr<Section> section = this->m_sections[i];

            if (cursorPosition >= overallEntryIndex && cursorPosition < (overallEntryIndex + section->entries.size()))
                return i;

            overallEntryIndex += section->entries.size();
        }

        // TODO: Call fatal here instead?
        std::cout << "getCursorSectionIndex: Cursor position out of bounds" << cursorPosition << std::endl;
        return 0;
    }

    size_t ConsolePanel::getCursorSectionEntryIndex(unsigned int cursorPosition)
    {
        size_t overallEntryIndex = 0;

        // Iterate over all of the sections
        for (size_t i = 0; i < this->m_sections.size(); i++)
        {
            std::shared_ptr<Section> section = this->m_sections[i];

            if (cursorPosition >= overallEntryIndex && cursorPosition < (overallEntryIndex + section->entries.size()))
                return cursorPosition - overallEntryIndex;

            overallEntryIndex += section->entries.size();
        }

        // TODO: Call fatal here instead?
        std::cout << "getCursorSectionEntryIndex: Cursor position out of bounds" << cursorPosition << std::endl;
        return 0;
    }
    
    unsigned int ConsolePanel::getCursorRowInConsole(unsigned int cursorPosition)
    {
        size_t sectionIndex = this->getCursorSectionIndex(cursorPosition);
        size_t sectionEntryIndex = this->getCursorSectionEntryIndex(cursorPosition);
        size_t sectionStartEntryOverallIndex = this->getSectionStartEntryOverallIndex(sectionIndex);

        return sectionStartEntryOverallIndex + sectionEntryIndex + (sectionIndex + 1) * 3;
    }

    void ConsolePanel::onChangeCursorPosition(unsigned int prevPos)
    {
        // Put > beside the currently selected entry
        size_t currentCursorRow = this->getCursorRowInConsole(this->m_cursorPosition);
        this->m_console->cursorX = 0;
        this->m_console->cursorY = currentCursorRow;
        std::cout << "> ";

        // Remove > from the previously selected entry
        size_t prevCursorRow = this->getCursorRowInConsole(prevPos);
        this->m_console->cursorX = 0;
        this->m_console->cursorY = prevCursorRow;
        std::cout << "  ";
    }

    size_t ConsolePanel::getTotalSectionEntries()
    {
        size_t ret = 0;

        for (auto &section : this->m_sections)
        {
            ret += section->entries.size();
        }

        return ret;
    }

    size_t ConsolePanel::getSectionStartEntryOverallIndex(size_t sectionEntryIndex)
    {
        size_t overallEntryIndex = 0;

        for (size_t i = 0; i < sectionEntryIndex; i++)
        {
            std::shared_ptr<Section> section = this->m_sections[i];
            overallEntryIndex += section->entries.size();
        }

        return overallEntryIndex;
    }
}