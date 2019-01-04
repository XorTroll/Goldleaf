
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright © 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from SciresM's source

*/

#pragma once
#include <string>
#include <switch.h>

namespace gleaf::hacpack
{
    extern "C"
    {
        #include <pack/nca.h>
        #include <pack/utils.h>
        #include <pack/settings.h>
        #include <pack/pki.h>
        #include <pack/extkeys.h>
        #include <pack/version.h>
        #include <pack/nacp.h>
        #include <pack/npdm.h>
        #include <pack/pfs0.h>
    }

    enum class PackageFormat
    {
        NCA,
        NSP,
    };

    enum class NCAType
    {
        Program,
        Meta,
        Control,
        Manual,
        Data,
        AOCData,
    };

    struct Build
    {
        NCAType NCA;
        bool NSP;
        u64 ApplicationId;

        std::string ExeFs;
        std::string RomFs;
        std::string Logo;

        std::string MProgramNCA;
        std::string MControlNCA;
        std::string MLegalInfoNCA;
        std::string MOfflineHtmlNCA;

        std::string NCADir;

        static Build MakeProgramNCA(u64 ApplicationId, std::string ExeFs, std::string RomFs, std::string Logo);
        static Build MakeControlNCA(u64 ApplicationId, std::string ControlFs);
        static Build MakeManualNCA(u64 ApplicationId, std::string HtmlFs);
        static Build MakeMetaNCA(u64 ApplicationId, std::string Program, std::string Control, std::string LegalInfo, std::string OfflineHtml);
        static Build MakeNSP(u64 ApplicationId, std::string NCADir);
    };

    bool Process(std::string Output, Build Mode, PackageFormat Format, std::string KeyFile);
}