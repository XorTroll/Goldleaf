
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
    class SettingsLayout : public pu::ui::Layout
    {
        public:
            SettingsLayout();
            PU_SMART_CTOR(SettingsLayout)

            void ExportUpdateToDirectory(pu::String Input, SetSysFirmwareVersion Fw);
            void ExportUpdateToNSP(pu::String Input, SetSysFirmwareVersion Fw);
            void HandleUpdate(pu::String Base, SetSysFirmwareVersion Fw);
            void optsConfig_Click();
            void optsFirmware_Click();
            void optsMemory_Click();
        private:
            pu::ui::elm::Menu::Ref optsMenu;
            pu::ui::elm::ProgressBar::Ref progressInfo;
    };
}