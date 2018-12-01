#include <gleaf/gleaf.hpp>

// Latest test: prints some info about the system

void update()
{
    consoleClear();
    std::cout << std::endl;
    std::cout << "Goldleaf system information test" << std::endl;
    std::cout << std::endl;
    std::cout << "Time: " << gleaf::horizon::GetCurrentTime() << std::endl;
    std::cout << "Battery: " << std::to_string(gleaf::horizon::GetBatteryLevel()) << (gleaf::horizon::IsCharging() ? "% (charging)" : "%") << std::endl;
    std::cout << "User: " << (gleaf::horizon::IsUserSelected() ? gleaf::horizon::GetOpenedUserName() : "<no user opened>") << std::endl;
    gleaf::horizon::FwVersion fw = gleaf::horizon::GetFwVersion();
    std::cout << "Firmware: " << fw.ToString() << " (" << fw.DisplayName << ")" << std::endl;
    std::cout << "Internet connection: " << (gleaf::horizon::HasInternetConnection() ? "Yes" : "No") << std::endl;
    std::cout << std::endl << "Press A button to refresh info." << std::endl;
}

int main()
{
    gleaf::Initialize();
    consoleInit(NULL);
    update();
    while(appletMainLoop())
    {
        hidScanInput();
        if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_A) update();
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    gleaf::Finalize();
    return 0;
}