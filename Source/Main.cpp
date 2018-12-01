#include <gleaf/gleaf.hpp>

// Latest test: almost fully implemented NAND/SD filesystem browser
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

gleaf::fs::Explorer gexp(gleaf::fs::Partition::SdCard);
u32 isel = 0;

void uexp()
{
    consoleClear();
    std::cout << "Goldleaf explorer test" << std::endl;
    std::cout << " - Press X to change partition!" << std::endl << std::endl;
    std::vector<std::string> cnts = gexp.GetContents();
    if(cnts.empty()) std::cout << "[Empty folder]";
    else for(u32 i = 0; i < cnts.size(); i++)
    {
        std::string cnt = cnts[i];
        bool isd = gleaf::fs::IsDirectory(gexp.FullPathFor(cnt));
        if(isd) std::cout << "[D] ";
        if(i == isel) std::cout << CONSOLE_CYAN << cnt << CONSOLE_RESET << std::endl;
        else std::cout << cnt << std::endl;
    }
}

int main()
{
    consoleInit(NULL);
    gleaf::Initialize();
    uexp();
    while(appletMainLoop())
    {
        hidScanInput();
        u64 k = hidKeysDown(CONTROLLER_P1_AUTO);
        if(k & KEY_UP)
        {
            if(isel > 0)
            {
                isel--;
                uexp();
            }
        }
        else if(k & KEY_DOWN)
        {
            if(isel < (gexp.GetContents().size() - 1))
            {
                isel++;
                uexp();
            }
        }
        else if(k & KEY_A)
        {
            std::string ccnt = gexp.GetContents()[isel];
            if(gexp.NavigateForward(ccnt))
            {
                isel = 0;
                uexp();
            }
        }
        else if(k & KEY_B)
        {
            if(gexp.NavigateBack())
            {
                isel = 0;
                uexp();
            }
        }
        else if(k & KEY_X)
        {
            consoleClear();
            std::cout << "Change partition" << std::endl << std::endl;
            std::cout << "Press Up to browse NAND (Safe)." << std::endl;
            std::cout << "Press Down to browse NAND (System)." << std::endl;
            std::cout << "Press Left to browse NAND (User)." << std::endl;
            std::cout << "Press Right to browse SD card." << std::endl;
            while(true)
            {
                hidScanInput();
                u64 k2 = hidKeysDown(CONTROLLER_P1_AUTO);
                if(k2 & KEY_UP)
                {
                    gexp.MovePartition(gleaf::fs::Partition::NANDSafe);
                    break;
                }
                else if(k2 & KEY_DOWN)
                {
                    gexp.MovePartition(gleaf::fs::Partition::NANDSystem);
                    break;
                }
                else if(k2 & KEY_LEFT)
                {
                    gexp.MovePartition(gleaf::fs::Partition::NANDUser);
                    break;
                }
                else if(k2 & KEY_RIGHT)
                {
                    gexp.MovePartition(gleaf::fs::Partition::SdCard);
                    break;
                }
                consoleUpdate(NULL);
            }
            isel = 0;
            uexp();
        }
        consoleUpdate(NULL);
    }
    consoleExit(NULL);
    return 0;
}