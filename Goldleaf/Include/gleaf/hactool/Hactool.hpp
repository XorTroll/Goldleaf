
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright © 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from SciresM's source

*/

#pragma once
#include <string>
#include <switch.h>

extern "C"
{
    #include <hactool/types.h>
    #include <hactool/utils.h>
    #include <hactool/settings.h>
    #include <hactool/pki.h>
    #include <hactool/nca.h>
    #include <hactool/xci.h>
    #include <hactool/nax0.h>
    #include <hactool/extkeys.h>
    #include <hactool/packages.h>
    #include <hactool/nso.h>
}

namespace gleaf::hactool
{
    enum class ExtractionFormat
    {
        XCI,
        NCA,
        PFS0,
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

    struct Extraction
    {
        bool DoExeFs;
        bool DoRomFs;
        bool DoLogo;
        std::string ExeFs;
        std::string RomFs;
        std::string Logo;
        bool HasTitleKey;
        std::string TitleKey;

        static Extraction MakeExeFs(std::string OutExeFs);
        static Extraction MakeRomFs(std::string OutRomFs);
        static Extraction MakeLogo(std::string OutLogo);
        static Extraction OnlyInformation();
    };

    struct ProcessResult
    {
        NCAType NCA;
        u64 AppId;
        bool Ok;
    };

    std::string NCATypeToString(NCAType NCA);
    ProcessResult Process(std::string Input, Extraction Mode, ExtractionFormat Format, std::string KeyFile);
}