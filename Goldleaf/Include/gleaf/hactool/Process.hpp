
#pragma once
#include <gleaf/Types.hpp>

namespace gleaf::hactool
{
    enum class ExtractionFormat
    {
        XCI,
        NCA,
        PFS0,
    };

    struct Extraction
    {
        bool DoExeFs;
        bool DoRomFs;
        bool DoLogo;
        std::string ExeFs;
        std::string RomFs;
        std::string Logo;

        static Extraction MakeExeFs(std::string OutExeFs);
        static Extraction MakeRomFs(std::string OutRomFs);
        static Extraction MakeLogo(std::string OutLogo);
    };

    bool Process(std::string Input, Extraction Mode, ExtractionFormat Format, std::string KeyFile);
}