#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    PCExploreLayout::PCExploreLayout() : pu::Layout()
    {
        this->pathsMenu = new pu::element::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->pathsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->Add(this->pathsMenu);
    }

    PCExploreLayout::~PCExploreLayout()
    {
        delete this->pathsMenu;
    }

    void PCExploreLayout::UpdatePaths()
    {
        this->names.clear();
        this->paths.clear();
        this->pathsMenu->ClearItems();
        u32 drivecount = 0;
        u32 pathcount = 0;
        if(usb::WriteCommandInput(usb::CommandId::ListSystemDrives))
        {
            drivecount = usb::Read32();
            for(u32 i = 0; i < drivecount; i++)
            {
                std::string name = usb::ReadString();
                std::string path = usb::ReadString();
                this->names.push_back(name + " (" + path + ":\\)");
                this->paths.push_back(path);
            }
        }
        if(usb::WriteCommandInput(usb::CommandId::GetEnvironmentPaths))
        {
            pathcount = usb::Read32();
            for(u32 i = 0; i < pathcount; i++)
            {
                std::string name = usb::ReadString();
                std::string path = usb::ReadString();
                this->names.push_back(name);
                this->paths.push_back(path);
            }
        }
        for(u32 i = 0; i < this->names.size(); i++)
        {
            pu::element::MenuItem *itm = new pu::element::MenuItem(this->names[i]);
            itm->SetColor(gsets.CustomScheme.Text);
            if(i < drivecount) itm->SetIcon(gsets.PathForResource("/Common/Drive.png"));
            else itm->SetIcon(gsets.PathForResource("/FileSystem/Directory.png"));
            itm->AddOnClick(std::bind(&PCExploreLayout::path_Click, this));
            this->pathsMenu->AddItem(itm);
        }
        this->pathsMenu->SetSelectedIndex(0);
    }

    void PCExploreLayout::path_Click()
    {
        u32 idx = this->pathsMenu->GetSelectedIndex();
        mainapp->GetBrowserLayout()->ChangePartitionPCDrive(this->paths[idx]);
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }
}