#include <gleaf/gleaf.hpp>

// The UI is still not finished, it is not even here.
// Latest test searched for "sd:/Test.nsp" and installed it, but installation still fails?...
// Go ahead, but I don't recommend building this project.

int main()
{
    consoleInit(NULL);
    gleaf::Initialize();
    gleaf::nsp::Installer inst(gleaf::Destination::SdCard, "@Sdcard://Test.nsp", true);
    inst.InitializeRecords();
    inst.InstallNCAs([](gleaf::ncm::ContentRecord record, u32 A, u32 B, int C)
    {
        std::cout << A << " - " << B << " - " << C << std::endl;
        consoleUpdate(NULL);
    });
    inst.Finish();
    consoleExit(NULL);
    gleaf::Finalize();
    return 0;
}