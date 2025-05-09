#include <res/res_Results.hpp>
#include <cfg/cfg_Strings.hpp>

namespace res {

    namespace {

        const std::pair<Result, std::pair<const char*, u32>> g_SpecialResultTable[] = {
            { rc::account::ResultUnableToRemoveLinkedAccount, { "account", 477 } },
            { rc::es::ResultUnableToImportTicket, { "es", 478 } },
            { rc::fs::ResultRomNcaHeaderSignature1VerificationFailed, { "fs", 479 } },
            { rc::fs::ResultRomNcaHeaderSignature2VerificationFailed, { "fs", 479 } },
            { rc::goldleaf::ResultMetaNotFound, { "goldleaf", 485 } },
            { rc::goldleaf::ResultInvalidMeta, { "goldleaf", 486 } },
        };

    }

    bool IsSpecialResult(const Result rc, std::string &out_module_name, std::string &out_rc_name) {
        for(const auto &pair : g_SpecialResultTable) {
            if(rc == pair.first) {
                out_module_name = pair.second.first;
                out_rc_name = cfg::Strings.GetString(pair.second.second);
                return true;
            }
        }
        
        return false;
    }

}