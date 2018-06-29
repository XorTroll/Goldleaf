#include "panel_changer.hpp"

#include <iostream>
#include <sstream>
#include "console_select_panel.hpp"
#include "ncm.h"
#include "title.hpp"
#include "utils.hpp"

namespace menu
{
    namespace main_menu
    {
        void titleInfoSelected(menu::Menu *menu)
        {
            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu->m_console, "Title Info");
            auto section = panel->addSection("Install Location");
            section->addEntry("NAND", nullptr);
            section->addEntry("SD Card", std::bind(tinfo_menu::storageSelected, menu, FsStorageId_SdCard));
            menu->pushPanel(panel);
        }

        void exitSelected(menu::Menu *menu)
        {
            menu->m_exitRequested = true;
        }
    }

    namespace tinfo_menu
    {
        void storageSelected(menu::Menu *menu, FsStorageId storageId)
        {
            Result rc = 0;
            NCMContentMetaDatabase contentMetaDatabase;
            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu->m_console, "Title Info");
            auto section = panel->addSection("Title Selection");

            if (R_FAILED(rc = ncmOpenContentMetaDatabase(storageId, &contentMetaDatabase)))
            {
                printf("storageSelected: Failed to open content meta database. Error code: 0x%08x\n", rc);
                return;
            }

            u32 numEntriesTotal;

            if (R_FAILED(rc = ncmContentMetaDatabaseListApplication(&contentMetaDatabase, 0, NULL, 0, &numEntriesTotal, NULL)))
            {
                printf("storageSelected: Failed to get number of application meta entries. Error code: 0x%08x\n", rc);
                return;
            }
            
            auto applicationContentMetaKeys = std::make_unique<NCMApplicationContentMetaKey[]>(numEntriesTotal);
            u32 numEntriesWritten;

            if (R_FAILED(rc = ncmContentMetaDatabaseListApplication(&contentMetaDatabase, 0, applicationContentMetaKeys.get(), sizeof(NCMApplicationContentMetaKey) * numEntriesTotal, NULL, &numEntriesWritten)))
            {
                printf("storageSelected: Failed to list application meta entries. Error code: 0x%08x\n", rc);
                return;
            }

            if (numEntriesWritten != numEntriesTotal)
            {
                printf("Number of entries written %u doesn't match total %u\n", numEntriesWritten, numEntriesTotal);
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

            menu->pushPanel(panel);
        }
    }
}