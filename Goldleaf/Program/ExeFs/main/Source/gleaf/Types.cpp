#include <gleaf/Types.hpp>

namespace gleaf
{
    bool InstallerResult::IsSuccess()
    {
        return R_SUCCEEDED(this->Error);
    }
}