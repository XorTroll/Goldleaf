#pragma once

#include <string>
#include <switch.h>

namespace tin::install
{
    class IGameContainer
    {
        public:
            virtual Result OpenContainer(std::string path) = 0;
            virtual void CloseContainer() = 0;

            virtual Result ReadFile(std::string name, u8* buff, size_t size, size_t offset) = 0;
            virtual Result GetFileSize(std::string name, size_t* sizeOut) = 0;

            virtual bool HasFile(std::string name) = 0;
            virtual std::string GetFileNameFromExtension(std::string extension) = 0;
    };
}