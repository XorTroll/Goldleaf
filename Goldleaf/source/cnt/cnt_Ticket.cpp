
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

#include <cnt/cnt_Ticket.hpp>
#include <cnt/cnt_Content.hpp>
#include <util/util_String.hpp>
#include <fs/fs_FileSystem.hpp>

namespace cnt {

    namespace {

        std::vector<Ticket> g_Tickets;
        Lock g_TicketsLock;

        Thread g_LoadTicketsThread;
        std::atomic_bool g_LoadTicketsThreadDone = true;

        void ScanTickets() {
            ScopedLock lk(g_TicketsLock);
            g_Tickets.clear();

            const auto common_count = esCountCommonTicket();
            if(common_count > 0) {
                const auto ids_size = common_count * sizeof(EsRightsId);
                auto ids = new EsRightsId[common_count]();
                u32 written = 0;
                if(R_SUCCEEDED(esListCommonTicket(&written, ids, ids_size))) {
                    for(u32 i = 0; i < written; i++) {
                        const Ticket common_tik = {
                            .rights_id = ids[i],
                            .type = TicketType::Common
                        };
                        g_Tickets.push_back(common_tik);
                    }
                }
                delete[] ids;
            }
    
            const auto personalized_count = esCountPersonalizedTicket();
            if(personalized_count > 0) {
                const auto ids_size = personalized_count * sizeof(EsRightsId);
                auto ids = new EsRightsId[personalized_count]();
                u32 written = 0;
                if(R_SUCCEEDED(esListPersonalizedTicket(&written, ids, ids_size))) {
                    for(u32 i = 0; i < written; i++) {
                        const Ticket personalized_tik = {
                            .rights_id = ids[i],
                            .type = TicketType::Personalized
                        };
                        g_Tickets.push_back(personalized_tik);
                    }
                }
                delete[] ids;
            }

            /*
            for(const auto &tik: g_Tickets) {
                GLEAF_LOG_FMT("Ticket for application ID %016lX:", esGetRightsIdApplicationId(&tik.rights_id));
                if(tik.type == TicketType::Common) {
                    GLEAF_LOG_FMT("  - Common ticket");
                }
                else {
                    GLEAF_LOG_FMT("  - Personalized ticket");
                }
                GLEAF_LOG_FMT("  - Key generation: %d", esGetRightsIdKeyGeneration(&tik.rights_id));
            }
            */
        }

        void LoadTicketsMain(void*) {
            SetThreadName("cnt.LoadTicketsThread");

            GLEAF_LOG_FMT("Scanning tickets...");
            ScanTickets();

            GLEAF_LOG_FMT("Done! Exiting thread...");
            g_LoadTicketsThreadDone = true;
        }

        void RequestLoadTickets() {
            if(!g_LoadTicketsThreadDone) {
                threadWaitForExit(&g_LoadTicketsThread);
            }
            threadClose(&g_LoadTicketsThread);

            g_LoadTicketsThreadDone = false;
            GLEAF_RC_ASSERT(threadCreate(&g_LoadTicketsThread, LoadTicketsMain, nullptr, nullptr, 512_KB, 0x1F, -2));
            GLEAF_RC_ASSERT(threadStart(&g_LoadTicketsThread));
        }

    }

    std::string Ticket::ToString() const {
        return util::FormatApplicationId(esGetRightsIdApplicationId(&this->rights_id)) + util::FormatApplicationId(esGetRightsIdKeyGeneration(&this->rights_id));
    }

    std::string TicketData::GetTitleKey() const {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::hex;
        for(u32 i = 0; i < 0x10; i++) {
            strm << static_cast<u32>(this->title_key_block[i]);
        }
        return strm.str();
    }

    Result RemoveTicket(const Ticket &tik) {
        const auto rc = esDeleteTicket(&tik.rights_id);
        if(R_SUCCEEDED(rc)) {
            NotifyTicketsChanged();
        }
        return rc;
    }

    void InitializeTickets() {
        NotifyTicketsChanged();
    }

    void FinalizeTickets() {
        threadClose(&g_LoadTicketsThread);
    }

    void NotifyTicketsChanged() {
        RequestLoadTickets();
    }

    std::vector<Ticket> &GetTickets() {
        ScopedLock lk(g_TicketsLock);
        return g_Tickets;
    }

    bool TryFindApplicationTicket(const u64 app_id, Ticket &out_tik) {
        ScopedLock lk(g_TicketsLock);
        const auto tik_it = std::find_if(g_Tickets.begin(), g_Tickets.end(), [app_id](const Ticket &tik) {
            return esGetRightsIdApplicationId(&tik.rights_id) == app_id;
        });
        if(tik_it != g_Tickets.end()) {
            out_tik = *tik_it;
            return true;
        }
        else {
            return false;
        }
    }

    TicketFile ReadTicket(const std::string &path) {
        TicketFile tik_file = {};

        auto exp = fs::GetExplorerForPath(path);
        exp->StartFile(path, fs::FileMode::Read);
        exp->ReadFile(path, 0, sizeof(tik_file.signature), &tik_file.signature);

        const auto tik_sig_data_size = GetTicketSignatureDataSize(tik_file.signature);
        exp->ReadFile(path, sizeof(tik_file.signature), tik_sig_data_size, tik_file.signature_data);

        const auto tik_data_offset = GetTicketSignatureSize(tik_file.signature);
        exp->ReadFile(path, tik_data_offset, sizeof(tik_file.data), &tik_file.data);

        exp->EndFile();
        return tik_file;
    }

    void SaveTicket(fs::Explorer *exp, const std::string &path, const TicketFile tik_file) {
        exp->DeleteFile(path);

        exp->StartFile(path, fs::FileMode::Append);
        exp->WriteFile(path, &tik_file.signature, sizeof(tik_file.signature));
        exp->WriteFile(path, tik_file.signature_data, GetTicketSignatureDataSize(tik_file.signature));
        exp->WriteFile(path, &tik_file.data, sizeof(tik_file.data));
        exp->EndFile();
    }

    std::string GetKeyGenerationRange(const u8 key_gen) {
        const auto master_key = key_gen - 1;
        // https://github.com/Atmosphere-NX/Atmosphere/blob/master/libraries/libstratosphere/source/fs/impl/fs_id_string_impl.os.generic.cpp
        switch(master_key) {
            case 0x00: {
                return "1.0.0 - 2.3.0";
            }
            case 0x01: {
                return "3.0.0";
            }
            case 0x02: {
                return "3.0.1 - 3.0.2";
            }
            case 0x03: {
                return "4.0.0 - 4.1.0";
            }
            case 0x04: {
                return "5.0.0 - 5.1.0";
            }
            case 0x05: {
                return "6.0.0 - 6.1.0";
            }
            case 0x06: {
                return "6.2.0";
            }
            case 0x07: {
                return "7.0.0 - 8.0.1";
            }
            case 0x08: {
                return "8.1.0 - 8.1.1";
            }
            case 0x09: {
                return "9.0.0 - 9.0.1";
            }
            case 0x0A: {
                return "9.1.0 - 12.0.3";
            }
            case 0x0B: {
                return "12.1.0";
            }
            case 0x0C: {
                return "13.0.0 - 13.2.1";
            }
            case 0x0D: {
                return "14.0.0 - 14.1.2";
            }
            case 0x0E: {
                return "15.0.0 - 15.0.1";
            }
            case 0x0F: {
                return "16.0.0 - 16.0.3";
            }
            case 0x10: {
                return "17.0.0 - 17.0.1";
            }
            case 0x11: {
                return "18.0.0 - 18.1.0";
            }
            case 0x12: {
                return "19.0.0 - 19.0.1";
            }
            case 0x13: {
                return "20.0.0 -";
            }
            default: {
                return "<unknown>";
            }
        }
    }

}
