
#pragma once
#include <vector>
#include <string>
#include <tuple>
#include <gleaf/Buffer.hpp>
#include <gleaf/lyt/BRTI.hpp>
#include <gleaf/lyt/DDS.hpp>

namespace gleaf::lyt
{
    class QuickBntx 
    {
    public:
        std::vector<BRTI> Textures;
        std::vector<u8> Rlt;

        QuickBntx(Buffer &Reader);

        std::vector<u8> Write();
        void ReplaceTex(const std::string &name, const DDSLoadResult &tex);
    private:
        std::vector<u8> Head;
        BRTI* FindTex(const std::string &name);
    };
}