#include <Forwarder.hpp>

int main()
{
    std::string gnro = GetGoldleaf();
    if(gnro.empty()) FatalError("find Goldleaf NRO", "Place it on 'sd:/switch' (or 'sd:/switch/Goldleaf') folder.");
    targetNro(gnro.c_str(), gnro.c_str());
}