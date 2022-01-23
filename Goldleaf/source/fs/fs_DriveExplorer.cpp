
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <fs/fs_DriveExplorer.hpp>

namespace fs {

    DriveExplorer::DriveExplorer(UsbHsFsDevice &drive) : drv(drive) {
        const std::string drive_name = drive.name;
        const auto drive_name_without_dots = drive_name.substr(0, drive_name.length() - 1);
        this->SetNames(drive_name_without_dots, drive_name_without_dots);
    }

}