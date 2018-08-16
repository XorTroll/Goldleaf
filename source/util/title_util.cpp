#include "util/title_util.hpp"

#include <machine/endian.h>
#include "error.hpp"

namespace tin::util
{
    u64 GetRightsIdTid(RightsId rightsId)
    {
        return __bswap64(*(u64 *)rightsId.c);
    }

    u64 GetRightsIdKeyGen(RightsId rightsId)
    {
        return __bswap64(*(u64 *)(rightsId.c + 8));
    }

    std::string GetNcaIdString(const NcmNcaId& ncaId)
    {
        char ncaIdStr[FS_MAX_PATH] = {0};
        u64 ncaIdLower = __bswap64(*(u64 *)ncaId.c);
        u64 ncaIdUpper = __bswap64(*(u64 *)(ncaId.c + 0x8));
        snprintf(ncaIdStr, FS_MAX_PATH, "%016lx%016lx", ncaIdLower, ncaIdUpper);
        return std::string(ncaIdStr);
    }

    NcmNcaId GetNcaIdFromString(std::string ncaIdStr)
    {
        NcmNcaId ncaId = {0};
        char lowerU64[17] = {0};
        char upperU64[17] = {0};
        memcpy(lowerU64, ncaIdStr.c_str(), 16);
        memcpy(upperU64, ncaIdStr.c_str() + 16, 16);

        *(u64 *)ncaId.c = __bswap64(strtoul(lowerU64, NULL, 16));
        *(u64 *)(ncaId.c + 8) = __bswap64(strtoul(upperU64, NULL, 16));

        return ncaId;
    }

    std::string GetTitleName(u64 titleId)
    {
        NsApplicationControlData appControlData;
        size_t sizeRead;

        ASSERT_OK(nsGetApplicationControlData(0x1, titleId, &appControlData, sizeof(NsApplicationControlData), &sizeRead), "Failed to get application control data");

        if (sizeRead < sizeof(appControlData.nacp))
        {
            throw std::runtime_error("Incorrect size for nacp");
        }

        NacpLanguageEntry *languageEntry;

        ASSERT_OK(nacpGetLanguageEntry(&appControlData.nacp, &languageEntry), "Failed to get language entry");

        if (languageEntry == NULL)
        {
            throw std::runtime_error("Language entry is null!");
        }

        return languageEntry->name;
    }
}