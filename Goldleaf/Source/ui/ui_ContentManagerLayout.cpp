#include <ui/ui_ContentManagerLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    ContentManagerLayout::ContentManagerLayout() : pu::ui::Layout()
    {
        this->typesMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->typesMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->sdCardMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(19));
        this->sdCardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ContentManagerLayout::sdCardMenuItem_Click, this));
        this->nandUserMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(28));
        this->nandUserMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ContentManagerLayout::nandUserMenuItem_Click, this));
        this->nandSystemMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(29));
        this->nandSystemMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandSystemMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandSystemMenuItem->AddOnClick(std::bind(&ContentManagerLayout::nandSystemMenuItem_Click, this));
        this->gameCartMenuItem = new pu::ui::elm::MenuItem(set::GetDictionaryEntry(190));
        this->gameCartMenuItem->SetIcon(gsets.PathForResource("/Common/GameCart.png"));
        this->gameCartMenuItem->SetColor(gsets.CustomScheme.Text);
        this->gameCartMenuItem->AddOnClick(std::bind(&ContentManagerLayout::gameCartMenuItem_Click, this));
        this->unusedTicketsMenuItem = new pu::ui::elm::MenuItem("Unused tickets");
        this->unusedTicketsMenuItem->SetIcon(gsets.PathForResource("/Common/Ticket.png"));
        this->unusedTicketsMenuItem->SetColor(gsets.CustomScheme.Text);
        this->unusedTicketsMenuItem->AddOnClick(std::bind(&ContentManagerLayout::unusedTicketsMenuItem_Click, this));
        this->typesMenu->AddItem(this->sdCardMenuItem);
        this->typesMenu->AddItem(this->nandUserMenuItem);
        this->typesMenu->AddItem(this->nandSystemMenuItem);
        this->typesMenu->AddItem(this->gameCartMenuItem);
        this->typesMenu->AddItem(this->unusedTicketsMenuItem);
        this->Add(this->typesMenu);
    }

    ContentManagerLayout::~ContentManagerLayout()
    {
        delete this->typesMenu;
    }

    void ContentManagerLayout::sdCardMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::SdCard);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandUserMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::NANDUser);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandSystemMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::NANDSystem);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::gameCartMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::GameCart);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::unusedTicketsMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(34), "Ticket", set::GetDictionaryEntry(35));
        mainapp->GetUnusedTicketsLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetUnusedTicketsLayout());
    }
}