#include "ui/ui_installnsp_mode.hpp"

#include <cstring>
#include "install/install_nsp.hpp"
#include "nx/fs.hpp"
#include "ui/console_options_view.hpp"
#include "error.hpp"

namespace tin::ui
{
    InstallNSPMode::InstallNSPMode() :
        IMode("Install NSP")
    {

    }

    void InstallNSPMode::GetNSPList()
    {
        nx::fs::IFileSystem fileSystem;
        fileSystem.OpenSdFileSystem();
        nx::fs::IDirectory dir = fileSystem.OpenDirectory("/tinfoil/nsp/", FS_DIROPEN_FILE);

        u64 entryCount = dir.GetEntryCount();

        auto dirEntries = std::make_unique<FsDirectoryEntry[]>(entryCount);

        dir.Read(0, dirEntries.get(), entryCount);

        for (unsigned int i = 0; i < entryCount; i++)
        {
            FsDirectoryEntry dirEntry = dirEntries[i];
            std::string dirEntryName(dirEntry.name);
            std::string ext = ".nsp";

            if (dirEntry.type != ENTRYTYPE_FILE || dirEntryName.compare(dirEntryName.size() - ext.size(), ext.size(), ext) != 0)
                continue;

            m_nspList.push_back(dirEntry.name);
        }
    }

    void InstallNSPMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleOptionsView>();
        view->AddEntry("Select NSP", tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);

        m_nspList.clear();
        this->GetNSPList();

        if (m_nspList.size() > 0)
        {
            view->AddEntry("Install All", ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnNSPSelected, this));

            for (unsigned int i = 0; i < m_nspList.size(); i++)
            {
                view->AddEntry(m_nspList[i], ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnNSPSelected, this));
            }
        }

        manager.PushView(std::move(view));
    }

    void InstallNSPMode::OnNSPSelected()
    {
        // Retrieve previous selection
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        ConsoleOptionsView* prevView;

        if (!(prevView = dynamic_cast<ConsoleOptionsView*>(manager.GetCurrentView())))
        {
            throw std::runtime_error("Previous view must be a ConsoleOptionsView!");
        }

        m_name = prevView->GetSelectedOptionValue()->GetText();

        // Prepare the next view
        auto view = std::make_unique<tin::ui::ConsoleOptionsView>();
        view->AddEntry("Select Destination", tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);
        view->AddEntry("SD Card", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnDestinationSelected, this));
        view->AddEntry("NAND", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnDestinationSelected, this));
        manager.PushView(std::move(view));
    }

    void InstallNSPMode::OnDestinationSelected()
    {
        // Retrieve previous selection
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        ConsoleOptionsView* prevView;

        if (!(prevView = dynamic_cast<ConsoleOptionsView*>(manager.GetCurrentView())))
        {
            throw std::runtime_error("Previous view must be a ConsoleOptionsView!");
        }

        auto destStr = prevView->GetSelectedOptionValue()->GetText();
        m_destStorageId = FsStorageId_SdCard;

        if (destStr == "NAND")
        {
            m_destStorageId = FsStorageId_NandUser;
        }

        auto view = std::make_unique<tin::ui::ConsoleOptionsView>();
        view->AddEntry("Ignore Required Firmware Version", tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);
        view->AddEntry("No", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnIgnoreReqFirmVersionSelected, this));
        view->AddEntry("Yes", tin::ui::ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnIgnoreReqFirmVersionSelected, this));
        manager.PushView(std::move(view));
    }

    void InstallNSPMode::OnIgnoreReqFirmVersionSelected()
    {
        // Retrieve previous selection
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        ConsoleOptionsView* prevView;

        if (!(prevView = dynamic_cast<ConsoleOptionsView*>(manager.GetCurrentView())))
        {
            throw std::runtime_error("Previous view must be a ConsoleOptionsView!");
        }

        auto optStr = prevView->GetSelectedOptionValue()->GetText();
        m_ignoreReqFirmVersion = (optStr == "Yes");
        std::vector<std::string> installList;

        if (m_name == "Install All")
        {
            installList = m_nspList;
        }
        else
        {
            installList.push_back(m_name);
        }

        // Push a blank view ready for installation
        auto view = std::make_unique<tin::ui::ConsoleView>(3);
        manager.PushView(std::move(view));

        for (unsigned int i = 0; i < installList.size(); i++)
        {
            printf("Installing %i/%ld\n", (i + 1), installList.size());
            std::string path = "@Sdcard://tinfoil/nsp/" + installList[i];

            try
            {
                nx::fs::IFileSystem fileSystem;
                ASSERT_OK(fileSystem.OpenFileSystemWithId(path, FsFileSystemType_ApplicationPackage, 0), "Failed to open application package file system");
                tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, "/", path + "/");
                tin::install::nsp::NSPInstallTask task(simpleFS, m_destStorageId, m_ignoreReqFirmVersion);

                task.PrepareForInstall();
                LOG_DEBUG("Pre Install Records: \n");
                task.DebugPrintInstallData();
                task.Install();
                LOG_DEBUG("Post Install Records: \n");
                task.DebugPrintInstallData();
            }
            catch (std::exception& e)
            {
                printf("Failed to install NSP!\n");
                LOG_DEBUG("Failed to install NSP");
                LOG_DEBUG("%s", e.what());
                fprintf(stdout, "%s", e.what());
                break;
            }
        }

        printf("Done!\n\nPress (B) to return.\n");
    }
}
