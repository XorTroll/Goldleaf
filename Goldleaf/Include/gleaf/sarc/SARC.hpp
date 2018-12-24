
#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <gleaf/Buffer.hpp>

namespace gleaf::sarc
{
    class SARC
    {
    public:
        struct PackedSarc 
        {
            std::vector<u8> data;
            u32 align;
        };

        struct SarcData
        {
            std::vector<std::string> names; //This is needed to keep the original file order
            std::unordered_map<std::string, std::vector<u8>> files;
            Endianness endianness;
            bool HashOnly;
        };

        static PackedSarc Pack(SarcData &data, s32 _align = -1);
        static SarcData Unpack(std::vector<u8> &data);

    private:
        static u32 NameHash(std::string name);
        static u32 StringHashToUint(std::string name);
        static std::string GuessFileExtension(std::vector<u8> &file);
        static u32 GuessAlignment(std::unordered_map<std::string, std::vector<u8>> &files);
        static u32 GuessFileAlignment(std::vector<u8> &file);
    };
}