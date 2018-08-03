#pragma once

#include <switch.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ui/console_view.hpp"

namespace tin::ui
{
    struct IOptionValue
    {
        public:
            virtual std::string GetText() = 0;
    };

    struct TextOptionValue : public IOptionValue
    {
        std::string name;

        TextOptionValue(std::string name);

        std::string GetText() override;
    };

    struct TitleIdOptionValue : public IOptionValue
    {
        u64 titleId;

        TitleIdOptionValue(u64 titleId);

        std::string GetText() override;
    };

    enum class ConsoleEntrySelectType
    {
        NONE, SELECT, SELECT_INACTIVE, HEADING
    };

    struct ConsoleEntry
    {
        std::shared_ptr<IOptionValue> optionValue;
        ConsoleEntrySelectType selectType;
        std::function<void ()> onSelected;
    };

    class ConsoleOptionsView : public ConsoleView
    {
        private:
            std::vector<ConsoleEntry> m_consoleEntries;
            unsigned int m_cursorPos;

        public:
            ConsoleOptionsView();

            virtual void OnPresented() override;
            virtual void ProcessInput(u64 keys) override;

            void AddEntry(std::shared_ptr<IOptionValue> value, ConsoleEntrySelectType selectType, std::function<void ()> onSelected);
            void AddEntry(std::string text, ConsoleEntrySelectType selectType, std::function<void ()> onSelected);

            std::shared_ptr<IOptionValue> GetSelectedOptionValue();

        protected:
            void MoveCursor(signed char off);
            void DisplayAll();
            void DisplayCursor();
            void ClearCursor();
    };
}