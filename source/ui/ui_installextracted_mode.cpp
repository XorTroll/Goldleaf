#include "ui/ui_installextracted_mode.hpp"

#include <cstring>
#include "install/install_nsp.hpp"
#include "nx/fs.hpp"
#include "ui/console_options_view.hpp"
#include "error.hpp"

namespace tin::ui
{
    InstallExtractedNSPMode::InstallExtractedNSPMode() :
        IMode("Install Extracted NSP")
    {

    }

    void InstallExtractedNSPMode::OnSelected() try
    {
        /*tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleOptionsView>();
        view->AddEntry(m_name, tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);*/

        nx::fs::IFileSystem fileSystem;
        fileSystem.OpenSdFileSystem();
        nx::fs::IDirectory dir = fileSystem.OpenDirectory("/tinfoil/extracted/", FS_DIROPEN_DIRECTORY | FS_DIROPEN_FILE);
        /*u64 entryCount = dir.GetEntryCount();
        LOG_DEBUG("Entry count: 0x%lx\n", entryCount);

        if (entryCount > 0)
        {
            auto dirEntries = std::make_unique<FsDirectoryEntry[]>(entryCount);
            dir.Read(0, dirEntries.get(), entryCount);

            for (unsigned int i = 0; i < entryCount; i++)
            {
                FsDirectoryEntry dirEntry = dirEntries[i];

                if (dirEntry.type != ENTRYTYPE_DIR)
                    continue;

                view->AddEntry(dirEntry.name, ConsoleEntrySelectType::SELECT, std::bind(&InstallExtractedNSPMode::OnExtractedNSPSelected, this));
            }
        }

        manager.PushView(std::move(view));*/
    }
    catch (std::runtime_error& e)
    {
        LOG_DEBUG("%s\n%s", "Failed to list extracted NSP entries", e.what());
    }

    void InstallExtractedNSPMode::OnExtractedNSPSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        ConsoleOptionsView* prevView;

        if (!(prevView = dynamic_cast<ConsoleOptionsView*>(manager.GetCurrentView())))
        {
            throw std::runtime_error("View must be a ConsoleOptionsView!");
        }

        std::string path = "/tinfoil/extracted/" + prevView->GetSelectedOptionValue()->GetText();
        std::string fullPath = "@Sdcard:/" + path;

        // Push a blank view ready for installation
        auto view = std::make_unique<tin::ui::ConsoleView>();
        manager.PushView(std::move(view));

        try
        {
            nx::fs::IFileSystem fileSystem;
            ASSERT_OK(fileSystem.OpenSdFileSystem(), "Failed to open SD file system");
            tin::install::nsp::SimpleFileSystem simpleFS(fileSystem, path, fullPath);
            tin::install::nsp::NSPInstallTask task(simpleFS, FsStorageId_SdCard);

            task.PrepareForInstall();
            task.Install();
        }
        catch (std::exception& e)
        {
            printf("Failed to install extracted NSP!\n");
            LOG_DEBUG("Failed to install extracted NSP");
            LOG_DEBUG("%s", e.what());
            fprintf(stdout, "%s", e.what());
        }
    }
}