#include <gleaf/ui/MainApplication.hpp>
#include <threads.h>

gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;
    std::vector<std::string> clipboard;

    void AddToClipboard(std::string Path)
    {
        if(!clipboard.empty()) for(u32 i = 0; i < clipboard.size(); i++) if(clipboard[i] == Path) return;
        clipboard.push_back(Path);
    }
    
    void ClearClipboard()
    {
        clipboard.clear();
    }

    void ShowPowerTasksDialog(std::string Title, std::string Message)
    {
        int sopt = mainapp->CreateShowDialog(Title, Message, { set::GetDictionaryEntry(233), set::GetDictionaryEntry(232), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        else switch(sopt)
        {
            case 0:
                bpcShutdownSystem();
                break;
            case 1:
                bpcRebootSystem();
                break;
        }
    }

    std::string AskForText(std::string Guide, std::string Initial)
    {
        std::string out = "";
        char tmpout[FS_MAX_PATH] = { 0 };
        SwkbdConfig kbd;
        Result rc = swkbdCreate(&kbd, 0);
        if(rc == 0)
        {
            swkbdConfigMakePresetDefault(&kbd);
            if(Guide != "") swkbdConfigSetGuideText(&kbd, Guide.c_str());
            if(Initial != "") swkbdConfigSetInitialText(&kbd, Initial.c_str());
            rc = swkbdShow(&kbd, tmpout, sizeof(tmpout));
            if(rc == 0) out = std::string(tmpout);
        }
        swkbdClose(&kbd);
        return out;
    }

    u128 AskForUser()
    {
        AppletHolder aph;
        AppletStorage hast1;
        LibAppletArgs args;
        appletCreateLibraryApplet(&aph, AppletId_playerSelect, LibAppletMode_AllForeground);
        libappletArgsCreate(&args, 0);
        libappletArgsPush(&args, &aph);
        appletCreateStorage(&hast1, 0xa0);
        u8 indata[0xa0] = { 0 };
        appletStorageWrite(&hast1, 0, indata, 0xa0);
        appletHolderPushInData(&aph, &hast1);
        appletHolderStart(&aph);
        appletStorageClose(&hast1);
        while(appletHolderWaitInteractiveOut(&aph));
        appletHolderJoin(&aph);
        AppletStorage ost;
        appletHolderPopOutData(&aph, &ost);
        u8 out[24] = { 0 };
        appletStorageRead(&ost, 0, out, 24);
        appletStorageClose(&ost);
        appletHolderClose(&aph);
        return *(u128*)&out[8];
    }

    void HandleResult(Result OSError)
    {
        if(OSError != 0)
        {
            err::Error err = err::DetermineError(OSError);
            // dialog?
        }
    }
}