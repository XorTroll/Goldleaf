
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <Types.hpp>

namespace ui
{
    void SetClipboard(std::string Path);
    void ClearClipboard();
    bool ClipboardEmpty();
    bool ClipboardNotEmpty();
    void ShowPowerTasksDialog(std::string Title, std::string Message);
    std::string AskForText(std::string Guide, std::string Initial);
    u128 AskForUser();
    void HandleResult(Result OSError, std::string Context);
}