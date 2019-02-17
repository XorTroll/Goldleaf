
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <gleaf/Goldleaf>
#include <pu/Plutonium>

namespace gleaf::ui
{
    void AddToClipboard(std::string Path);
    void ClearClipboard();
    void ShowPowerTasksDialog(std::string Title, std::string Message);
    std::string AskForText(std::string Guide, std::string Initial);
    u128 AskForUser();
    void HandleResult(Result OSError);
}