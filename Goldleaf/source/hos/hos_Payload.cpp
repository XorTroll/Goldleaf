
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

#include <hos/hos_Payload.hpp>
#include <hos/hos_Common.hpp>
#include <fs/fs_FileSystem.hpp>
#include <bpcams/bpcams_Service.hpp>

namespace hos {

    bool RebootWithPayload(const std::string &path) {
        auto exp = fs::GetExplorerForPath(path);
        if(exp != nullptr) {
            const auto payload_size = exp->GetFileSize(path);
            if(payload_size > 0) {
                auto payload_buf = new u8[payload_size]();
                exp->ReadFile(path, 0, payload_size, payload_buf);

                if(R_SUCCEEDED(bpcams::Initialize())) {
                    const auto rc = bpcams::SetRebootPayload(payload_buf, payload_size);
                    bpcams::Exit();

                    if(R_SUCCEEDED(rc)) {
                        if(R_SUCCEEDED(Reboot())) {
                            return true;
                        }
                    }
                }

                delete[] payload_buf;
            }
        }
        return false;
    }

}