
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

#include <bpcams/bpcams_Service.hpp>

namespace bpcams {

    namespace {

        Service g_BpcAmsService;

    }

    Result Initialize() {
        if(serviceIsActive(&g_BpcAmsService)) {
            return 0;
        }

        Handle tmp_port_h;
        auto rc = svcConnectToNamedPort(&tmp_port_h, "bpc:ams");
        if(R_SUCCEEDED(rc)) {
            serviceCreate(&g_BpcAmsService, tmp_port_h);
        }
        return rc;
    }

    void Exit() {
        if(serviceIsActive(&g_BpcAmsService)) {
            serviceClose(&g_BpcAmsService);
        }
    }

    bool HasInitialized()
    {
        return serviceIsActive(&g_BpcAmsService);
    }

    Result SetRebootPayload(void *payload_buffer, size_t payload_size) {
        return serviceDispatch(&g_BpcAmsService, 65001,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { payload_buffer, payload_size } },
        );
    }

}