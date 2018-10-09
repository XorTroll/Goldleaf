#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "nx/content_meta.hpp"

namespace tin::util
{
    std::tuple<std::string, nx::ncm::ContentRecord> GetCNMTNCAInfo(std::string nspPath);
    nx::ncm::ContentMeta GetContentMetaFromNCA(std::string ncaPath);
    std::vector<std::string> GetNSPList();
}