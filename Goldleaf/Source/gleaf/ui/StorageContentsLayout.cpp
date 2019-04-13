#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    StorageContentsLayout::StorageContentsLayout()
    {
        this->contentsMenu = new pu::element::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->contentsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->noContentsText = new pu::element::TextBlock(30, 630, set::GetDictionaryEntry(188));
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
        horizon::Title selcnt = this->contents[this->contentsMenu->GetSelectedIndex()];
        if(selcnt.IsBaseTitle() || (selcnt.Location == Storage::NANDSystem))
        {
            mainapp->GetContentInformationLayout()->LoadContent(selcnt);
            mainapp->LoadLayout(mainapp->GetContentInformationLayout());
        }
    }

    void StorageContentsLayout::LoadFromStorage(Storage Location)
    {
        if(!this->contents.empty())
        {
            this->contentsMenu->ClearItems();
            this->contents.clear();
        }
        std::vector<horizon::Title> cnts = horizon::SearchTitles(ncm::ContentMetaType::Any, Location);
        if(!cnts.empty()) for(u32 i = 0; i < cnts.size(); i++)
        {
            if(cnts[i].IsBaseTitle() || (cnts[i].Location == Storage::NANDSystem)) this->contents.push_back(cnts[i]);
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
                if(this->contents[i].IsBaseTitle() || (this->contents[i].Location == Storage::NANDSystem))
                {
                    NacpStruct *nacp = this->contents[i].TryGetNACP();
                    std::string name = horizon::FormatApplicationId(this->contents[i].ApplicationId);
                    if(nacp != NULL)
                    {
                        name = horizon::GetNACPName(nacp);
                        free(nacp);
                    }
                    pu::element::MenuItem *itm = new pu::element::MenuItem(name);
                    itm->SetColor(gsets.CustomScheme.Text);
                    bool hicon = this->contents[i].DumpControlData();
                    if(hicon) itm->SetIcon(horizon::GetExportedIconPath(this->contents[i].ApplicationId));
                    itm->AddOnClick(std::bind(&StorageContentsLayout::contents_Click, this));
                    this->contentsMenu->AddItem(itm);
                }
            }
            this->contentsMenu->SetSelectedIndex(0);
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(189));
    }

    std::vector<horizon::Title> StorageContentsLayout::GetContents()
    {
        return this->contents;
    }
}