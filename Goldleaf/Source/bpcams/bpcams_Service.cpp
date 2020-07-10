
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

namespace bpcams
{
    static Service bpcams_srv;

    Result Initialize()
    {
        if(serviceIsActive(&bpcams_srv)) return 0;
        Handle tmp;
        auto rc = svcConnectToNamedPort(&tmp, "bpc:ams");
        if(R_SUCCEEDED(rc)) serviceCreate(&bpcams_srv, tmp);
        return rc;
    }

    void Exit()
    {
        if(serviceIsActive(&bpcams_srv)) serviceClose(&bpcams_srv);
    }

    bool HasInitialized()
    {
        return serviceIsActive(&bpcams_srv);
    }

    Result SetRebootPayload(void *payload_buffer, size_t payload_size)
    {
        return serviceDispatch(&bpcams_srv, 65001,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { payload_buffer, payload_size } },
        );
    }
}