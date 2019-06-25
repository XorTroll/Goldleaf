#include <Forwarder.hpp>

int main()
{
    std::string gnro = GetGoldleaf();
    if(gnro.empty()) FatalError("find Goldleaf NRO", "Place it on '/switch' folder.");
    targetNro(gnro.c_str(), gnro.c_str());
}