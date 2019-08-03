#include <ui/ui_PCExploreLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    PCExploreLayout::PCExploreLayout() : pu::ui::Layout()
    {
        this->pathsMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
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
        Result rc = usb::ProcessCommand<usb::CommandId::GetDriveCount>(usb::Out32(drivecount));
        if(R_SUCCEEDED(rc))
        {
            for(u32 i = 0; i < drivecount; i++)
            {
                std::string label;
                std::string path;
                u32 sztmp = 0;
                rc = usb::ProcessCommand<usb::CommandId::GetDriveInfo>(usb::In32(i), usb::OutString(label), usb::OutString(path), usb::Out32(sztmp), usb::Out32(sztmp));
                if(R_SUCCEEDED(rc))
                {
                    this->names.push_back(label + " (" + path + ":\\)");
                    this->paths.push_back(path);
                }
            }
        }
        rc = usb::ProcessCommand<usb::CommandId::GetSpecialPathCount>(usb::Out32(pathcount));
        if(R_SUCCEEDED(rc))
        {
            for(u32 i = 0; i < pathcount; i++)
            {
                std::string name;
                std::string path;
                rc = usb::ProcessCommand<usb::CommandId::GetSpecialPath>(usb::In32(i), usb::OutString(name), usb::OutString(path));
                if(R_SUCCEEDED(rc))
                {
                    this->names.push_back(name);
                    this->paths.push_back(path);    
                }
            }
        }
        for(u32 i = 0; i < this->names.size(); i++)
        {
            pu::ui::elm::MenuItem *itm = new pu::ui::elm::MenuItem(this->names[i]);
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