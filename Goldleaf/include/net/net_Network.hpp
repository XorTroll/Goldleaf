
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

#pragma once
#include <base.hpp>

namespace net {

    using RetrieveOnProgressCallback = std::function<void(const double now_downloaded, const double total_to_download)>;

    std::string RetrieveContent(const std::string &url, const std::string &mime_type = "");
    void RetrieveToFile(const std::string &url, const std::string &path, RetrieveOnProgressCallback on_progress_cb);
    bool HasConnection();

}
