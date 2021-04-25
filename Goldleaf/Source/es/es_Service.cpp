
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

#include <es/es_Service.hpp>

namespace es {

    namespace {

        Service g_EsService;

    }

    Result Initialize() {
        if(serviceIsActive(&g_EsService)) {
            return 0;
        }
        return smGetService(&g_EsService, "es");
    }

    void Exit() {
        if(serviceIsActive(&g_EsService)) {
            serviceClose(&g_EsService);
        }
    }

    bool HasInitialized() {
        return serviceIsActive(&g_EsService);
    }

    Result ImportTicket(const void *tik, size_t tik_size, const void *cert, size_t cert_size) {
        return serviceDispatch(&g_EsService, 1,
            .buffer_attrs = {
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
                SfBufferAttr_HipcMapAlias | SfBufferAttr_In,
            },
            .buffers = {
                { tik, tik_size },
                { cert, cert_size },
            },
        );
    }

    Result DeleteTicket(const RightsId &rights_id) {
        return serviceDispatch(&g_EsService, 3,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
            .buffers = { { std::addressof(rights_id), sizeof(RightsId) }, },
        );
    }

    Result CountCommonTicket(u32 *out_count) {
        return serviceDispatchOut(&g_EsService, 9, *out_count);
    }

    Result CountPersonalizedTicket(u32 *out_count) {
        return serviceDispatchOut(&g_EsService, 10, *out_count);
    }

    Result ListCommonTicket(u32 *out_written, RightsId *out_rights_id_buf, size_t out_rights_id_buf_size) {
        return serviceDispatchOut(&g_EsService, 11, *out_written,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_rights_id_buf, out_rights_id_buf_size } },
        );
    }

    Result ListPersonalizedTicket(u32 *out_written, RightsId *out_rights_id_buf, size_t out_rights_id_buf_size) {
        return serviceDispatchOut(&g_EsService, 12, *out_written,
            .buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_Out },
            .buffers = { { out_rights_id_buf, out_rights_id_buf_size } },
        );
    }

}