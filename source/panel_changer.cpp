#include "panel_changer.hpp"

#include <iostream>
#include <sstream>
#include "console_select_panel.hpp"
#include "ncm.h"
#include "error.hpp"
#include "menu.hpp"
#include "title.hpp"
#include "utils.hpp"

namespace menu
{
    namespace main_menu
    {
        void titleInfoSelected()
        {
            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu::g_menu->m_console, "Title Info");
            auto section = panel->addSection("Install Location");
            section->addEntry("NAND", std::bind(menu::tinfo_menu::storageSelected, FsStorageId_NandUser));
            section->addEntry("SD Card", std::bind(menu::tinfo_menu::storageSelected, FsStorageId_SdCard));
            menu::g_menu->pushPanel(panel);
        }

        void exitSelected()
        {
            menu::g_menu->m_exitRequested = true;
        }
    }

    namespace tinfo_menu
    {
        void storageSelected(FsStorageId storageId)
        {
            Result rc = 0;
            NCMContentMetaDatabase contentMetaDatabase;
            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu::g_menu->m_console, "Title Info");
            auto section = panel->addSection("Title Selection");

            if (R_FAILED(rc = ncmOpenContentMetaDatabase(storageId, &contentMetaDatabase)))
            {
                error::critical("menu::tinfo_menu::storageSelected", " Failed to open content meta database", rc);
                return;
            }

            u32 numEntriesTotal;

            if (R_FAILED(rc = ncmContentMetaDatabaseListApplication(&contentMetaDatabase, 0, NULL, 0, &numEntriesTotal, NULL)))
            {
                error::critical("menu::tinfo_menu::storageSelected", "Failed to get number of application meta entries", rc);
                return;
            }
            
            auto applicationContentMetaKeys = std::make_unique<NCMApplicationContentMetaKey[]>(numEntriesTotal);
            u32 numEntriesWritten;

            if (R_FAILED(rc = ncmContentMetaDatabaseListApplication(&contentMetaDatabase, 0, applicationContentMetaKeys.get(), sizeof(NCMApplicationContentMetaKey) * numEntriesTotal, NULL, &numEntriesWritten)))
            {
                error::critical("menu::tinfo_menu::storageSelected", "Failed to list application meta entries", rc);
                return;
            }

            if (numEntriesWritten != numEntriesTotal)
            {
                std::stringstream ss;
                ss << "Number of entries written " << numEntriesWritten << " doesn't match total " << numEntriesTotal;
                error::critical("menu::tinfo_menu::storageSelected", ss.str(), -1);
                return;
            }

            for (u32 i = 0; i < numEntriesTotal; i++)
            {
                NCMApplicationContentMetaKey applicationContentMetaKey = applicationContentMetaKeys.get()[i];

                // We only want base titles, not updates
                if (applicationContentMetaKey.baseTitleID == applicationContentMetaKey.metaRecord.titleID)
                {
                    Title title(applicationContentMetaKey.metaRecord.titleID, storageId);
                    
                    std::stringstream sectionName;
                    sectionName << title.getName() << " (" << utils::toHexString(reinterpret_cast<u8 *>(&applicationContentMetaKey.baseTitleID), sizeof(u64)) << ")";
                    section->addEntry(sectionName.str(), nullptr);
                }
            }

            menu::g_menu->pushPanel(panel);
        }
    }
}