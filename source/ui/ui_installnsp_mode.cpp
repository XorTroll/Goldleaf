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

    void InstallNSPMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleOptionsView>();
        view->AddEntry(m_name, tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);

        nx::fs::IFileSystem fileSystem;
        fileSystem.OpenSdFileSystem();
        nx::fs::IDirectory dir = fileSystem.OpenDirectory("/tinfoil/nsp/", FS_DIROPEN_DIRECTORY | FS_DIROPEN_FILE);

        u64 entryCount = dir.GetEntryCount();

        if (entryCount > 0)
        {
            auto dirEntries = std::make_unique<FsDirectoryEntry[]>(entryCount);

            dir.Read(0, dirEntries.get(), entryCount);

            for (unsigned int i = 0; i < entryCount; i++)
            {
                FsDirectoryEntry dirEntry = dirEntries[i];

                if (dirEntry.type != ENTRYTYPE_FILE || strcmp(strchr(dirEntry.name, '.') + 1, "nsp") != 0)
                    continue;

                view->AddEntry(dirEntry.name, ConsoleEntrySelectType::SELECT, std::bind(&InstallNSPMode::OnNSPSelected, this));
            }
        }

        manager.PushView(std::move(view));
    }

    void InstallNSPMode::OnNSPSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        ConsoleOptionsView* prevView;

        if (!(prevView = dynamic_cast<ConsoleOptionsView*>(manager.GetCurrentView())))
        {
            throw std::runtime_error("View must be a ConsoleOptionsView!");
        }

        std::string path = "@Sdcard://tinfoil/nsp/" + prevView->GetSelectedOptionValue()->GetText();

        // Push a blank view ready for installation
        auto view = std::make_unique<tin::ui::ConsoleView>();
        manager.PushView(std::move(view));

        try
        {
            nx::fs::IFileSystem fileSystem;
            ASSERT_OK(fileSystem.OpenFileSystemWithId(path, FsFileSystemType_ApplicationPackage, 0), "Failed to open application package file system");
            tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, "/", path + "/");
            tin::install::nsp::NSPInstallTask task(simpleFS, FsStorageId_SdCard);

            task.PrepareForInstall();
            LOG_DEBUG("Pre Install Records: \n");
            task.DebugPrintInstallData();
            task.Install();
        }
        catch (std::exception& e)
        {
            printf("Failed to install NSP!\n");
            LOG_DEBUG("Failed to install NSP");
            LOG_DEBUG("%s", e.what());
            fprintf(stdout, "%s", e.what());
        }
    }
}