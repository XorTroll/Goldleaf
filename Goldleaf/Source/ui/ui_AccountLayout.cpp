#include <ui/ui_AccountLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    AccountLayout::AccountLayout() : pu::ui::Layout()
    {
        this->optsMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->optsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        pu::ui::elm::MenuItem *itm = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(208));
        itm->SetColor(gsets.CustomScheme.Text);
        itm->AddOnClick(std::bind(&AccountLayout::optsRename_Click, this));
        this->optsMenu->AddItem(itm);
        pu::ui::elm::MenuItem *itm2 = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(209));
        itm2->SetColor(gsets.CustomScheme.Text);
        itm2->AddOnClick(std::bind(&AccountLayout::optsIcon_Click, this));
        this->optsMenu->AddItem(itm2);
        pu::ui::elm::MenuItem *itm3 = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(210));
        itm3->SetColor(gsets.CustomScheme.Text);
        itm3->AddOnClick(std::bind(&AccountLayout::optsDelete_Click, this));
        this->optsMenu->AddItem(itm3);
        this->Add(this->optsMenu);
    }

    AccountLayout::~AccountLayout()
    {
        delete this->optsMenu;
    }

    void AccountLayout::Load(u128 UserId)
    {
        this->uid = UserId;
        Result rc = accountGetProfile(&this->prf, UserId);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            this->CleanData();
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        this->pbase = (AccountProfileBase*)malloc(sizeof(AccountProfileBase));
        this->udata = (AccountUserData*)malloc(sizeof(AccountUserData));
        rc = accountProfileGet(&this->prf, this->udata, this->pbase);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            this->CleanData();
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(212) + " " + std::string(this->pbase->username));
        rc = acc::GetProfileEditor(UserId, &this->pred);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            this->CleanData();
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        std::string iconpth = "sdmc:/" + GoldleafDir + "/userdata/" + hos::FormatHex128(UserId) + ".jpg";
        fs::DeleteFile(iconpth);
        size_t imgsz = 0;
        size_t pimgsz = 0;
        rc = accountProfileGetImageSize(&this->prf, &pimgsz);
        u8 *img = (u8*)malloc(pimgsz);
        rc = accountProfileLoadImage(&this->prf, img, pimgsz, &imgsz);
        FILE *f = fopen(iconpth.c_str(), "wb");
        if((rc == 0) && f) fwrite(img, pimgsz, 1, f);
        fclose(f);
        free(img);
    }

    void AccountLayout::CleanData()
    {
        if(this->uid != 0)
        {
            this->uid = 0;
            serviceClose(&this->prf.s);
            free(this->pbase);
            free(this->udata);
            this->pred.Close();
        }
    }

    void AccountLayout::optsRename_Click()
    {
        std::string name = AskForText(set::GetDictionaryEntry(213), "");
        if(name != "")
        {
            if(name.length() <= 10)
            {
                strcpy(this->pbase->username, name.c_str());
                Result rc = this->pred.Store(this->pbase, this->udata);
                if(rc == 0)
                {
                    mainapp->LoadMenuHead(set::GetDictionaryEntry(212) + " " + name);
                    mainapp->ShowNotification(set::GetDictionaryEntry(214) + " \'" + name + "\'.");
                }
                else HandleResult(rc, set::GetDictionaryEntry(215));
            }
            else mainapp->ShowNotification(set::GetDictionaryEntry(249));
        }
    }

    void AccountLayout::optsIcon_Click()
    {
        std::string iconpth = "/" + GoldleafDir + "/userdata/" + hos::FormatHex128(this->uid) + ".jpg";
        mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(217) + "\n\'SdCard:" + iconpth + "\'", { set::GetDictionaryEntry(234) }, false, "sdmc:" + iconpth);
    }

    void AccountLayout::optsDelete_Click()
    {
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(218), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt == 0)
        {
            s32 ucount = 0;
            accountGetUserCount(&ucount);
            if(ucount < 2)
            {
                mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(276), { set::GetDictionaryEntry(234) }, true);
                return;
            }
            Result rc = acc::DeleteUser(this->uid);
            if(rc == 0)
            {
                mainapp->ShowNotification(set::GetDictionaryEntry(219));
                this->CleanData();
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
            else HandleResult(rc, set::GetDictionaryEntry(220));
        }
    }
}