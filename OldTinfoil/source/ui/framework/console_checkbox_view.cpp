#include "ui/framework/console_checkbox_view.hpp"

#include "util/graphics_util.hpp"
#include "util/title_util.hpp"
#include "error.hpp"

namespace tin::ui
{
    ConsoleCheckboxView::ConsoleCheckboxView(std::function<void ()> _onDone, std::string title, unsigned int unwindDistance) :
        ConsoleOptionsView(title, unwindDistance), m_onDone(_onDone)
    {

    }

    void ConsoleCheckboxView::ProcessInput(u64 keys)
    {
        // Account for a potential lack of any selectable console entries
        if (keys & KEY_X || keys & KEY_A)
        {
            ConsoleEntry* consoleEntry = m_consoleEntries.at(m_cursorPos).get();

            if (consoleEntry->selectType != ConsoleEntrySelectType::SELECT)
                return;
        }

        if (keys & KEY_X)
        {
            bool found = false;
            for (std::vector<unsigned int>::size_type i = 0; i < m_selectedEntries.size(); i++)
            {
                auto& index = m_selectedEntries[i];
                if (index == m_cursorPos)
                {
                    found = true;
                    m_selectedEntries.erase(m_selectedEntries.begin() + i);
                    ClearIndicator(m_cursorPos);
                    break;
                }
            }
            if (!found)
            {
                m_selectedEntries.push_back(m_cursorPos);
                DisplayIndicator(m_cursorPos);
            }
        }
        else if (keys & KEY_A)
        {
            if (m_selectedEntries.empty())
            {
                m_selectedEntries.push_back(m_cursorPos);
            }

            m_onDone();
        }
        else
            ConsoleOptionsView::ProcessInput(keys);
    }

    std::vector<ConsoleEntry*> ConsoleCheckboxView::GetSelectedEntries()
    {
        std::vector<ConsoleEntry*> selected;

        for (auto& index : m_selectedEntries)
        {
            ConsoleEntry* entry = m_consoleEntries.at(index).get();
            selected.push_back(entry);
        }

        return selected;
    }

    std::vector<IOptionValue*> ConsoleCheckboxView::GetSelectedOptionValues()
    {
        std::vector<IOptionValue*> selectedOptions;
        std::vector<ConsoleEntry*> selectedEntries = GetSelectedEntries();

        for (auto& entry : selectedEntries)
        {
            selectedOptions.push_back(entry->optionValue.get());
        }

        return selectedOptions;
    }

    void ConsoleCheckboxView::DisplayAll()
    {
        auto console = ViewManager::Instance().m_printConsole;
        ConsoleOptionsView::DisplayAll();
        this->DisplayIndicators();
        console->cursorX = 0;
        console->cursorY = m_consoleEntries.size() + 3;
        console->flags |= CONSOLE_COLOR_BOLD;
        tin::util::PrintTextCentred("(X) to toggle option, (A) to continue");
        console->flags &= ~CONSOLE_COLOR_BOLD;
    }

    void ConsoleCheckboxView::DisplayIndicators()
    {
        auto console = ViewManager::Instance().m_printConsole;

        for (auto& selected : m_selectedEntries)
        {
            console->cursorX = 2;
            console->cursorY = selected + 2;
            console->flags |= CONSOLE_COLOR_BOLD;
            printf("*");
            console->flags &= ~CONSOLE_COLOR_BOLD;
        }
    }

    void ConsoleCheckboxView::ClearIndicators()
    {
        auto console = ViewManager::Instance().m_printConsole;

        for (auto& selected : m_selectedEntries)
        {
            console->cursorX = 2;
            console->cursorY = selected + 2;
            printf(" ");
        }
    }

    void ConsoleCheckboxView::DisplayIndicator(int entryIndex)
    {
        auto console = ViewManager::Instance().m_printConsole;
        console->cursorX = 2;
        console->cursorY = entryIndex + 2;
        console->flags |= CONSOLE_COLOR_BOLD;
        printf("*");
        console->flags &= ~CONSOLE_COLOR_BOLD;
    }

    void ConsoleCheckboxView::ClearIndicator(int entryIndex)
    {
        auto console = ViewManager::Instance().m_printConsole;
        console->cursorX = 2;
        console->cursorY = entryIndex + 2;
        printf(" ");
    }
}
