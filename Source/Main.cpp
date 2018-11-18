#include <gleaf/gleaf.hpp>

void reprint(std::string user, std::string time, std::string battery)
{
    consoleClear();
    std::cout << user << std::endl << time << std::endl << battery;
    consoleUpdate(NULL);
}

// Latest test: attempts to install "Test.nsp" from the SD card
// Will output an error to console and pause the installation if it's catched

int main()
{
    gleaf::nsp::Installer inst(gleaf::Destination::SdCard, "@Sdcard://Test.nsp", true);
    inst.InitializeRecords();
    inst.InstallNCAs([](gleaf::ncm::ContentRecord a, u32 b, u32 c, int d)
    {

    });
    inst.Finish();
    return 0;
}