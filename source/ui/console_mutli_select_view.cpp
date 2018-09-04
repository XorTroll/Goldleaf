#include "ui/console_multi_select_view.hpp"

#include "util/graphics_util.hpp"
#include "util/title_util.hpp"
#include "error.hpp"

namespace tin::ui
{
    ConsoleMultiSelectView::ConsoleMultiSelectView(std::function<void ()> _onDone, std::string title, unsigned int unwindDistance) :
        ConsoleOptionsView(title, unwindDistance), onDone(_onDone)
    {

    }

    void ConsoleMultiSelectView::ProcessInput(u64 keys)
    {
        if (keys & KEY_A)
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
        else if (keys & KEY_X)
            onDone();
        else
            ConsoleOptionsView::ProcessInput(keys);
    }

    std::vector<ConsoleEntry*> ConsoleMultiSelectView::GetSelectedEntries()
    {
        std::vector<ConsoleEntry*> selected;

        for (auto& index : m_selectedEntries)
        {
            ConsoleEntry* entry = m_consoleEntries.at(index).get();
            selected.push_back(entry);
        }

        return selected;
    }

    std::vector<IOptionValue*> ConsoleMultiSelectView::GetSelectedOptionValues()
    {
        std::vector<IOptionValue*> selectedOptions;
        std::vector<ConsoleEntry*> selectedEntries = GetSelectedEntries();

        for (auto& entry : selectedEntries)
        {
            selectedOptions.push_back(entry->optionValue.get());
        }

        return selectedOptions;
    }

    void ConsoleMultiSelectView::DisplayAll()
    {
        auto console = ViewManager::Instance().m_printConsole;
        ConsoleOptionsView::DisplayAll();
        this->DisplayIndicators();
        console->cursorX = 0;
        console->cursorY = m_consoleEntries.size() + 3;
        console->flags |= CONSOLE_COLOR_BOLD;
        tin::util::PrintTextCentred("(A) to toggle option, (X) to continue");
        console->flags &= ~CONSOLE_COLOR_BOLD;
    }

    void ConsoleMultiSelectView::DisplayIndicators()
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

    void ConsoleMultiSelectView::ClearIndicators()
    {
        auto console = ViewManager::Instance().m_printConsole;

        for (auto& selected : m_selectedEntries)
        {
            console->cursorX = 2;
            console->cursorY = selected + 2;
            printf(" ");
        }
    }

    void ConsoleMultiSelectView::DisplayIndicator(int entryIndex)
    {
        auto console = ViewManager::Instance().m_printConsole;
        console->cursorX = 2;
        console->cursorY = entryIndex + 2;
        console->flags |= CONSOLE_COLOR_BOLD;
        printf("*");
        console->flags &= ~CONSOLE_COLOR_BOLD;
    }

    void ConsoleMultiSelectView::ClearIndicator(int entryIndex)
    {
        auto console = ViewManager::Instance().m_printConsole;
        console->cursorX = 2;
        console->cursorY = entryIndex + 2;
        printf(" ");
    }
}
