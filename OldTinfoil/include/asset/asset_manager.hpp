#pragma once

#include <map>
#include <memory>
#include <string>

#include "data/byte_buffer.hpp"

namespace tin::asset
{
    class Asset
    {

    };

    class AssetManager final
    {
        public:
            AssetManager();
            ~AssetManager();
            AssetManager(AssetManager const&) = delete;
            void operator=(AssetManager const&)  = delete;

            static AssetManager& Instance();

            void ReadAsset(tin::data::ByteBuffer& buffer, std::string path);
    };
}