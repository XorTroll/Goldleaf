#pragma once

#include <switch.h>
#include <string>
#include "nx/ipc/tin_ipc.h"

namespace tin::util
{
    u64 GetRightsIdTid(RightsId rightsId);
    u64 GetRightsIdKeyGen(RightsId rightsId);
    std::string GetTitleName(u64 titleId);
}