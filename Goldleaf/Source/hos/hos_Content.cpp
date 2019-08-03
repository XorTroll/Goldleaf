#include <hos/hos_Content.hpp>

namespace hos
{
    std::string ContentIdAsString(const NcmNcaId &NCAId)
    {
        char idstr[FS_MAX_PATH] = { 0 };
        u64 lower = __bswap64(*(u64*)NCAId.c);
        u64 upper = __bswap64(*(u64*)(NCAId.c + 0x8));
        snprintf(idstr, FS_MAX_PATH, "%016lx%016lx", lower, upper);
        return std::string(idstr);
    }

    NcmNcaId StringAsContentId(std::string NCAId)
    {
        NcmNcaId nid = { 0 };
        char lower[0x20] = { 0 };
        char upper[0x20] = { 0 };
        memcpy(lower, NCAId.c_str(), 16);
        memcpy(upper, NCAId.c_str() + 16, 16);
        *(u64*)nid.c = __bswap64(strtoul(lower, NULL, 16));
        *(u64*)(nid.c + 0x8) = __bswap64(strtoul(upper, NULL, 16));
        return nid;
    }
}