#include <gleaf/Types.hpp>

namespace gleaf
{
    bool InstallerResult::IsSuccess()
    {
        return (this->Type == InstallerError::Success);
    }
}