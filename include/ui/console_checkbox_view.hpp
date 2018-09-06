#pragma once

#include <vector>
#include <switch.h>

#include "console_options_view.hpp"

namespace tin::ui
{
    class ConsoleCheckboxView : public ConsoleOptionsView
    {
        public:
            ConsoleCheckboxView(std::function<void ()> onDone = nullptr, std::string title="Tinfoil 0.1.0 by Adubbz", unsigned int unwindDistance = 1);

            std::vector<ConsoleEntry*> GetSelectedEntries();
            std::vector<IOptionValue*> GetSelectedOptionValues();

            std::function<void ()> m_onDone;

            void ProcessInput(u64 keys) override;
        protected:
            const char* PaddingAfterCursor() const override
            {
                return "   ";
            }

            std::vector<unsigned int> m_selectedEntries;

            void DisplayAll() override;

            void DisplayIndicators();
            void ClearIndicators();
            void DisplayIndicator(int entryIndex);
            void ClearIndicator(int entryIndex);
    };
}