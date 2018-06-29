#include "utils.hpp"

#include <iomanip>
#include <sstream>

namespace utils
{
    std::string toHexString(u8 *in, size_t length)
    {
        std::stringstream ss;
        ss << std::hex;

        for (size_t i = 0; i < length; i++)
            ss << std::setw(2) << std::setfill('0') << (u32)in[i];
        
        return ss.str();
    }
}