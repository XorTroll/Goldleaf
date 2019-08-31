#include <ui/ui_MainMenuLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    MainMenuLayout::MainMenuLayout() : pu::ui::Layout()
    {
        this->optionMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->optionMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->optionMenu);
        this->exploreMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(277));
        this->exploreMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->exploreMenuItem->SetColor(gsets.CustomScheme.Text);
        this->exploreMenuItem->AddOnClick(std::bind(&MainMenuLayout::exploreMenuItem_Click, this));
        this->titleMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(3));
        this->titleMenuItem->SetIcon(gsets.PathForResource("/Common/Storage.png"));
        this->titleMenuItem->SetColor(gsets.CustomScheme.Text);
        this->titleMenuItem->AddOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->webMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(5));
        this->webMenuItem->SetIcon(gsets.PathForResource("/Common/Browser.png"));
        this->webMenuItem->SetColor(gsets.CustomScheme.Text);
        this->webMenuItem->AddOnClick(std::bind(&MainMenuLayout::webMenuItem_Click, this));
        this->accountMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(6));
        this->accountMenuItem->SetIcon(gsets.PathForResource("/Common/Accounts.png"));
        this->accountMenuItem->SetColor(gsets.CustomScheme.Text);
        this->accountMenuItem->AddOnClick(std::bind(&MainMenuLayout::accountMenuItem_Click, this));
        this->amiiboMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(283));
        this->amiiboMenuItem->SetIcon(gsets.PathForResource("/Common/Amiibo.png"));
        this->amiiboMenuItem->SetColor(gsets.CustomScheme.Text);
        this->amiiboMenuItem->AddOnClick(std::bind(&MainMenuLayout::amiiboMenuItem_Click, this));
        this->sysinfoMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(7));
        this->sysinfoMenuItem->SetIcon(gsets.PathForResource("/Common/Settings.png"));
        this->sysinfoMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sysinfoMenuItem->AddOnClick(std::bind(&MainMenuLayout::sysinfoMenuItem_Click, this));
        this->updateMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(284));
        this->updateMenuItem->SetIcon(gsets.PathForResource("/Common/Update.png"));
        this->updateMenuItem->SetColor(gsets.CustomScheme.Text);
        this->updateMenuItem->AddOnClick(std::bind(&MainMenuLayout::updateMenuItem_Click, this));
        this->aboutMenuItem = pu::ui::elm::MenuItem::New(set::GetDictionaryEntry(8));
        this->aboutMenuItem->SetIcon(gsets.PathForResource("/Common/Info.png"));
        this->aboutMenuItem->SetColor(gsets.CustomScheme.Text);
        this->aboutMenuItem->AddOnClick(std::bind(&MainMenuLayout::aboutMenuItem_Click, this));
        this->optionMenu->AddItem(this->exploreMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->webMenuItem);
        this->optionMenu->AddItem(this->accountMenuItem);
        this->optionMenu->AddItem(this->amiiboMenuItem);
        this->optionMenu->AddItem(this->sysinfoMenuItem);
        this->optionMenu->AddItem(this->updateMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->Add(this->optionMenu);
    }

    void MainMenuLayout::exploreMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(277), "Storage", set::GetDictionaryEntry(278));
        mainapp->LoadLayout(mainapp->GetExploreMenuLayout());
    }

    void MainMenuLayout::titleMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(32), "Storage", set::GetDictionaryEntry(33));
        EnsureDirectories();
        mainapp->LoadLayout(mainapp->GetContentManagerLayout());
    }

    void MainMenuLayout::webMenuItem_Click()
    {
        if(GetCurrentApplicationId() != GOLDLEAF_APPID)
        {
            mainapp->CreateShowDialog(set::GetDictionaryEntry(5), set::GetDictionaryEntry(292), { set::GetDictionaryEntry(234) }, true);
            return;
        }
        pu::String out = AskForText(set::GetDictionaryEntry(38), "https://");
        if(out.empty()) return;
        else
        {
            bool nothttp = (out.AsUTF8().substr(0, 6) != "http:/");
            bool nothttps = (out.AsUTF8().substr(0, 7) != "https:/");
            if(nothttp && nothttps)
            {
                mainapp->CreateShowDialog(set::GetDictionaryEntry(36), set::GetDictionaryEntry(39), { set::GetDictionaryEntry(234) }, false);
                return;
            }
        }
        WebCommonConfig web;
        webPageCreate(&web, out.AsUTF8().c_str());
        WebCommonReply wout;
        webConfigShow(&web, &wout);  
    }

    void MainMenuLayout::accountMenuItem_Click()
    {
        if(acc::GetSelectedUser() == 0)
        {
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(348), set::GetDictionaryEntry(349), {set::GetDictionaryEntry(111), set::GetDictionaryEntry(18)}, true);
            if(sopt != 0) return;
            if(acc::SelectUser()) mainapp->ShowNotification(set::GetDictionaryEntry(324));
            else mainapp->ShowNotification(set::GetDictionaryEntry(350));
        }
        mainapp->LoadMenuData(set::GetDictionaryEntry(41), "Accounts", set::GetDictionaryEntry(42));
        mainapp->GetAccountLayout()->Load();
        mainapp->LoadLayout(mainapp->GetAccountLayout());
    }

    void MainMenuLayout::amiiboMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(283), "Amiibo", set::GetDictionaryEntry(301));
        mainapp->LoadLayout(mainapp->GetAmiiboDumpLayout());
        mainapp->GetAmiiboDumpLayout()->StartDump();
        mainapp->UnloadMenuData();
        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
    }

    void MainMenuLayout::sysinfoMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(43), "Settings", set::GetDictionaryEntry(44));
        mainapp->GetSystemInfoLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSystemInfoLayout());
    }

    void MainMenuLayout::updateMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(284), "Update", set::GetDictionaryEntry(302));
        mainapp->LoadLayout(mainapp->GetUpdateLayout());
        mainapp->GetUpdateLayout()->StartUpdateSearch();
    }

    void MainMenuLayout::aboutMenuItem_Click()
    {
        pu::String exmode = set::GetDictionaryEntry(288);
        if(GetExecutableMode() == ExecutableMode::NSO) exmode = set::GetDictionaryEntry(289);

        pu::String lmode;
        if(GetLaunchMode() == LaunchMode::Applet) lmode = set::GetDictionaryEntry(290);
        if(GetLaunchMode() == LaunchMode::Application) lmode = set::GetDictionaryEntry(291);

        mainapp->LoadMenuData("Goldleaf v" + GetVersion(), "Info", exmode.AsUTF8() + ", " + lmode.AsUTF8());
        mainapp->LoadLayout(mainapp->GetAboutLayout());
    }
}