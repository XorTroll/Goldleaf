#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    USBInstallLayout::USBInstallLayout() : pu::Layout()
    {
        this->installText = new pu::element::TextBlock(150, 320, set::GetDictionaryEntry(151));
        this->installText->SetTextAlign(pu::element::TextAlign::CenterHorizontal);
        this->installText->SetColor(gsets.CustomScheme.Text);
        this->installBar = new pu::element::ProgressBar(340, 360, 600, 30);
        this->installBar->SetVisible(false);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    bool USBCallback()
    {
        mainapp->CallForRender();
        return usb::IsStateNotReady();
    }

    void USBInstallLayout::StartUSBConnection()
    {
        this->installText->SetText(set::GetDictionaryEntry(152) + " " + set::GetDictionaryEntry(153));
        mainapp->CallForRender();
        if(usb::IsStateNotReady())
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(152) + "\n" + set::GetDictionaryEntry(153), { set::GetDictionaryEntry(234) }, true);
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        this->installText->SetText(set::GetDictionaryEntry(155));
        mainapp->CallForRender();
        usb::Command req = usb::ReadCommand(USBCallback);
        if(usb::CommandMagicOk(req))
        {
            if(usb::IsCommandId(req, usb::CommandId::ConnectionRequest))
            {
                this->installText->SetText(set::GetDictionaryEntry(241));
                mainapp->CallForRender();
                if(!usb::WriteCommand(usb::CommandId::ConnectionResponse)) HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));;
                req = usb::ReadCommand(USBCallback);
                if(usb::CommandMagicOk(req))
                {
                    if(usb::IsCommandId(req, usb::CommandId::NSPName))
                    {
                        std::string nspname = usb::ReadString(USBCallback);
                        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(157) + " \'" + nspname + "\'\n" + set::GetDictionaryEntry(158), { set::GetDictionaryEntry(65), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0)
                        {
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            usb::WriteCommand(usb::CommandId::Finish);
                            return;
                        }
                        mainapp->LoadMenuHead(set::GetDictionaryEntry(145) + " \'" + nspname + "\'");
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(78), { set::GetDictionaryEntry(19), set::GetDictionaryEntry(79), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0)
                        {
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            usb::WriteCommand(usb::CommandId::Finish);
                            return;
                        }
                        Storage dst = Storage::SdCard;
                        if(sopt == 1) dst = Storage::NANDUser;
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(80), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0)
                        {
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            usb::WriteCommand(usb::CommandId::Finish);
                            return;
                        }
                        bool ignorev = (sopt == 0);
                        usb::WriteCommand(usb::CommandId::Start);
                        if(IsInstalledTitle()) appletBeginBlockingHomeButton(0);
                        this->installText->SetText(set::GetDictionaryEntry(144));
                        mainapp->CallForRender();
                        usb::Installer inst(dst, ignorev, USBCallback);
                        Result rc = inst.GetLatestResult();
                        if(rc != 0)
                        {
                            if(IsInstalledTitle()) appletEndBlockingHomeButton();
                            if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
                            {
                                horizon::Title t = horizon::Locate(inst.GetApplicationId());
                                horizon::RemoveTitle(t);
                            }
                            HandleResult(rc, set::GetDictionaryEntry(251));
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            usb::WriteCommand(usb::CommandId::Finish);
                            return;
                        }
                        this->installText->SetText(set::GetDictionaryEntry(146));
                        mainapp->CallForRender();
                        rc = inst.ProcessRecords([&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed)
                        {
                            std::string name = set::GetDictionaryEntry(148) + " \'"  + Name + "\'... (" + std::to_string(Index + 1) + " " + set::GetDictionaryEntry(149) + " " + std::to_string(Count) + ")";
                            this->installText->SetText(name);
                            this->installBar->SetProgress((u8)Percentage);
                            mainapp->UpdateFooter(set::GetDictionaryEntry(159) + " " + horizon::DoubleToString(Speed) + " MB/s");
                            mainapp->CallForRender();
                        });
                        if(rc != 0)
                        {
                            if(IsInstalledTitle()) appletEndBlockingHomeButton();
                            if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
                            {
                                horizon::Title t = horizon::Locate(inst.GetApplicationId());
                                horizon::RemoveTitle(t);
                            }
                            HandleResult(rc, set::GetDictionaryEntry(251));
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            usb::WriteCommand(usb::CommandId::Finish);
                            return;
                        }
                        this->installText->SetText(set::GetDictionaryEntry(147));
                        mainapp->CallForRender();
                        this->installBar->SetVisible(true);
                        rc = inst.ProcessContents([&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed)
                        {
                            std::string name = set::GetDictionaryEntry(148) + " \'"  + Name + "\'... (" + std::to_string(Index + 1) + " " + set::GetDictionaryEntry(149) + " " + std::to_string(Count) + ")";
                            this->installText->SetText(name);
                            this->installBar->SetProgress((u8)Percentage);
                            mainapp->UpdateFooter(set::GetDictionaryEntry(159) + " " + horizon::DoubleToString(Speed) + " MB/s");
                            mainapp->CallForRender();
                        });
                        this->installBar->SetVisible(false);
                        if(IsInstalledTitle()) appletEndBlockingHomeButton();
                        if(rc != 0)
                        {
                            if(rc != err::Make(err::ErrorDescription::TitleAlreadyInstalled))
                            {
                                horizon::Title t = horizon::Locate(inst.GetApplicationId());
                                horizon::RemoveTitle(t);
                            }
                            HandleResult(rc, set::GetDictionaryEntry(251));
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            usb::WriteCommand(usb::CommandId::Finish);
                            return;
                        }
                        inst.Finalize();
                        mainapp->UpdateFooter(set::GetDictionaryEntry(160));
                        mainapp->CallForRender();
                    }
                    else if(usb::IsCommandId(req, usb::CommandId::Finish)) mainapp->UpdateFooter(set::GetDictionaryEntry(242));
                    else
                    {
                        HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
                        usb::WriteCommand(usb::CommandId::Finish);
                    }
                }
                else
                {
                    HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
                    usb::WriteCommand(usb::CommandId::Finish);
                }
            }
            else if(usb::IsCommandId(req, usb::CommandId::Finish)) mainapp->UpdateFooter(set::GetDictionaryEntry(242));
            else
            {
                HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
                usb::WriteCommand(usb::CommandId::Finish);
            }
        }
        else
        {
            HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
            usb::WriteCommand(usb::CommandId::Finish);
        }
        mainapp->UnloadMenuData();
        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
    }
}