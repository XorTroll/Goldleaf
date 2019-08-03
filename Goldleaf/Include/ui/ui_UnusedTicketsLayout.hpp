
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <ui/ui_Includes.hpp>
#include <pu/Plutonium>

namespace ui
{
    class UnusedTicketsLayout : public pu::ui::Layout
    {
        public:
            UnusedTicketsLayout();
            ~UnusedTicketsLayout();
            void UpdateElements(bool Cooldown);
            void tickets_Click();
        private:
            std::vector<hos::Ticket> tickets;
            pu::ui::elm::TextBlock *notTicketsText;
            pu::ui::elm::Menu *ticketsMenu;
    };
}