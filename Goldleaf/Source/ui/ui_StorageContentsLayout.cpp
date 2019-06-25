#include <ui/ui_StorageContentsLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    StorageContentsLayout::StorageContentsLayout()
    {
        this->contentsMenu = new pu::element::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->contentsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->noContentsText = new pu::element::TextBlock(0, 0, set::GetDictionaryEntry(188));
        this->noContentsText->SetHorizontalAlign(pu::element::HorizontalAlign::Center);
        this->noContentsText->SetVerticalAlign(pu::element::VerticalAlign::Center);
        this->noContentsText->SetColor(gsets.CustomScheme.Text);
        this->noContentsText->SetVisible(false);
        this->Add(this->noContentsText);
        this->Add(this->contentsMenu);
    }

    StorageContentsLayout::~StorageContentsLayout()
    {
        delete this->noContentsText;
        delete this->contentsMenu;
    }

    void StorageContentsLayout::contents_Click()
    {
        hos::Title selcnt = this->contents[this->contentsMenu->GetSelectedIndex()];
        mainapp->GetContentInformationLayout()->LoadContent(selcnt);
        mainapp->LoadLayout(mainapp->GetContentInformationLayout());
    }

    void StorageContentsLayout::LoadFromStorage(Storage Location)
    {
        if(!this->contents.empty())
        {
            this->contentsMenu->ClearItems();
            this->contents.clear();
        }
        std::vector<hos::Title> cnts = hos::SearchTitles(ncm::ContentMetaType::Any, Location);
        if(!cnts.empty()) for(u32 i = 0; i < cnts.size(); i++)
        {
            hos::Title cnt = cnts[i];
            bool ok = true;
            if(!this->contents.empty()) for(u32 j = 0; j < this->contents.size(); j++)
            {
                if(cnt.ApplicationId == this->contents[j].ApplicationId)
                {
                    ok = false;
                    break;
                }
                else
                {
                    u64 baseid = hos::GetBaseApplicationId(cnt.ApplicationId, cnt.Type);
                    if(baseid == this->contents[j].ApplicationId)
                    {
                        ok = false;
                        break;
                    }
                }
            }
            if(ok) this->contents.push_back(cnt);
        }
        cnts.clear();
        if(this->contents.empty())
        {
            this->noContentsText->SetVisible(true);
            this->contentsMenu->SetVisible(false);
        }
        else
        {
            this->contentsMenu->SetCooldownEnabled(true);
            this->noContentsText->SetVisible(false);
            this->contentsMenu->SetVisible(true);
            for(u32 i = 0; i < this->contents.size(); i++)
            {
                NacpStruct *nacp = this->contents[i].TryGetNACP();
                std::string name = hos::FormatApplicationId(this->contents[i].ApplicationId);
                if(nacp != NULL)
                {
                    name = hos::GetNACPName(nacp);
                    free(nacp);
                }
                pu::element::MenuItem *itm = new pu::element::MenuItem(name);
                itm->SetColor(gsets.CustomScheme.Text);
                bool hicon = this->contents[i].DumpControlData();
                if(hicon) itm->SetIcon(hos::GetExportedIconPath(this->contents[i].ApplicationId));
                itm->AddOnClick(std::bind(&StorageContentsLayout::contents_Click, this));
                this->contentsMenu->AddItem(itm);
            }
            this->contentsMenu->SetSelectedIndex(0);
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(189));
    }

    std::vector<hos::Title> StorageContentsLayout::GetContents()
    {
        return this->contents;
    }
}