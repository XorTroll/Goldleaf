
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll, emerged from Adubbz's work with Tinfoil

    This project is licensed under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <Types.hpp>

namespace ui
{
    void SetClipboard(pu::String Path);
    void ClearClipboard();
    bool ClipboardEmpty();
    bool ClipboardNotEmpty();
    void ShowPowerTasksDialog(pu::String Title, pu::String Message);
    pu::String AskForText(pu::String Guide = "", pu::String Initial = "", int MaxSize = -1);
    void HandleResult(Result OSError, pu::String Context);
}