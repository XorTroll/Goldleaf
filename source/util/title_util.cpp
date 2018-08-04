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