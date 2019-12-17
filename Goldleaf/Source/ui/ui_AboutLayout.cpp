
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_AboutLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern set::Settings global_settings;

namespace ui
{
    AboutLayout::AboutLayout()
    {
        this->logoImage = pu::ui::elm::Image::New(85, 150, global_settings.PathForResource("/Logo.png"));
        this->Add(this->logoImage);
    }
}