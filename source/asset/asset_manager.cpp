#include "asset/asset_manager.hpp"

#include <stdlib.h>
#include <errno.h>
#include <sys/iosupport.h>
#include <unistd.h>

#include "error.hpp"
#include "debug.h"

namespace tin::asset
{
    AssetManager::AssetManager()
    {
    }

    AssetManager::~AssetManager()
    {
    }

    AssetManager& AssetManager::Instance()
    {
        static AssetManager instance;
        return instance;
    }

    void AssetManager::ReadAsset(tin::data::ByteBuffer& buffer, std::string path)
    {
        std::string root = "romfs:/";
        std::string absPath = root + path;
        struct stat st;

        if (stat(absPath.c_str(), &st) != 0)
            THROW_FORMAT("Failed to get file size!\n");

        buffer.Resize(st.st_size);
        FILE* af = fopen(absPath.c_str(), "r");
        fread(buffer.GetData(), st.st_size, 1, af);
        fclose(af);
    }
}