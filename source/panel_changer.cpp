#include "panel_changer.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include "console_panel.hpp"
#include "console_select_panel.hpp"
#include "ncm.h"
#include "error.hpp"
#include "menu.hpp"
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
        NCMContentStorage g_contentStorage;
        NCMContentMetaDatabase g_contentMetaDatabase;

        void storageSelected(FsStorageId storageId)
        {
            Result rc = 0;

            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu::g_menu->m_console, "Title Info");
            auto section = panel->addSection("Title Selection");

            if (R_FAILED(rc = ncmOpenContentStorage(storageId, &g_contentStorage)))
            {
                error::critical("menu::tinfo_menu::storageSelected", " Failed to open content storage", rc);
                return;
            }

            if (R_FAILED(rc = ncmOpenContentMetaDatabase(storageId, &g_contentMetaDatabase)))
            {
                error::critical("menu::tinfo_menu::storageSelected", " Failed to open content meta database", rc);
                return;
            }

            u32 numEntriesTotal;

            if (R_FAILED(rc = ncmContentMetaDatabaseListApplication(&g_contentMetaDatabase, 0, NULL, 0, &numEntriesTotal, NULL)))
            {
                error::critical("menu::tinfo_menu::storageSelected", "Failed to get number of application meta entries", rc);
                return;
            }
            
            auto applicationContentMetaKeys = std::make_unique<NCMApplicationContentMetaKey[]>(numEntriesTotal);
            u32 numEntriesWritten;

            if (R_FAILED(rc = ncmContentMetaDatabaseListApplication(&g_contentMetaDatabase, 0, applicationContentMetaKeys.get(), sizeof(NCMApplicationContentMetaKey) * numEntriesTotal, NULL, &numEntriesWritten)))
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

            std::map<u64, NCMMetaRecord> baseMetaRecords;
            std::map<u64, NCMMetaRecord> updateMetaRecords;

            for (u32 i = 0; i < numEntriesTotal; i++)
            {
                NCMApplicationContentMetaKey applicationContentMetaKey = applicationContentMetaKeys.get()[i];

                switch (applicationContentMetaKey.metaRecord.type)
                {
                    // Base games
                    case 0x80:
                        baseMetaRecords.insert(std::make_pair(applicationContentMetaKey.baseTitleID, applicationContentMetaKey.metaRecord));
                        break;

                    // Updates
                    case 0x81:
                        updateMetaRecords.insert(std::make_pair(applicationContentMetaKey.baseTitleID, applicationContentMetaKey.metaRecord));
                        break;
                }
            }

            for (std::pair<u64, NCMMetaRecord> baseElement : baseMetaRecords)
            {
                NCMMetaRecord updateMetaRecord = updateMetaRecords[baseElement.first];
                auto title = std::make_shared<Title>(baseElement.second, updateMetaRecord, storageId);
                
                std::stringstream sectionName;
                sectionName << title->getName() << " (" << utils::toHexString(reinterpret_cast<u8 *>(&baseElement.first), sizeof(u64)) << ")";
                section->addEntry(sectionName.str(), std::bind(menu::tinfo_menu::titleSelected, title));
            }

            menu::g_menu->pushPanel(panel);
        }

        void titleSelected(std::shared_ptr<Title> title)
        {
            auto panel = std::make_shared<menu::ConsoleSelectPanel>(menu::g_menu->m_console, "Title Info");
            
            std::stringstream sectionNameSS;
            sectionNameSS << "Title Options - " << title->getName();

            auto section = panel->addSection(sectionNameSS.str());
            section->addEntry("Display Meta Records", std::bind(menu::tinfo_menu::displayMetaRecordsSelected, title));
            section->addEntry("List Base Content Records", std::bind(menu::tinfo_menu::listContentRecordsSelected, title, false));
            //section->addEntry("List Update Content Records", std::bind(menu::tinfo_menu::listContentRecordsSelected, title, true));
            menu::g_menu->pushPanel(panel);
        }

        void displayMetaRecordsSelected(std::shared_ptr<Title> title)
        {
            auto panel = std::make_shared<menu::ConsolePanel>();

            std::stringstream ss;
            ss << title->getName() << " Meta Records: ";
            panel->addLine(ss.str());
            panel->addLine("");
            ss.str("");

            panel->addLine("Base Game: ");
            panel->addLine("");
            ss << " Title Id: " << utils::toHexString(reinterpret_cast<u8 *>(&title->m_baseMetaRecord.titleID), sizeof(u64));
            panel->addLine(ss.str());
            ss.str("");
            ss << "  Version: " << utils::toHexString(reinterpret_cast<u8 *>(&title->m_baseMetaRecord.version), sizeof(u32));
            panel->addLine(ss.str());
            ss.str("");
            ss << "     Type: " << getMetaRecordTypeName(title->m_baseMetaRecord.type) << " (" << (int)title->m_baseMetaRecord.type << ")";
            panel->addLine(ss.str());
            ss.str("");
            ss << "    Flags: " << (int)title->m_baseMetaRecord.flags;
            panel->addLine(ss.str());
            ss.str("");
            panel->addLine("");

            if (title->m_updateMetaRecord.titleID != 0)
            {
                panel->addLine("Update: ");
                panel->addLine("");
                ss << " Title Id: " << utils::toHexString(reinterpret_cast<u8 *>(&title->m_updateMetaRecord.titleID), sizeof(u64));
                panel->addLine(ss.str());
                ss.str("");
                ss << "  Version: " << utils::toHexString(reinterpret_cast<u8 *>(&title->m_updateMetaRecord.version), sizeof(u32));
                panel->addLine(ss.str());
                ss.str("");
                ss << "     Type: " << getMetaRecordTypeName(title->m_updateMetaRecord.type) << " (" << (int)title->m_updateMetaRecord.type << ")";
                panel->addLine(ss.str());
                ss.str("");
                ss << "    Flags: " << (int)title->m_updateMetaRecord.flags;
                panel->addLine(ss.str());
                ss.str("");
            }

            menu::g_menu->pushPanel(panel);
        }

        void listContentRecordsSelected(std::shared_ptr<Title> title, bool isUpdate)
        {
            Result rc = 0;
            auto panel = std::make_shared<menu::ConsolePanel>();

            // TODO: It appears update content records use a different format from base game content records (?)
            const NCMMetaRecord *metaRecord;
            std::array<NCMContentRecord, 256> contentRecords;
            size_t sizeRead;
            size_t entriesRead;

            if (isUpdate)
                metaRecord = &title->m_updateMetaRecord;
            else
                metaRecord = &title->m_baseMetaRecord;

            if (R_FAILED(rc = ncmContentMetaDatabaseGet(&g_contentMetaDatabase, metaRecord, contentRecords.size() * sizeof(NCMContentRecord), contentRecords.data(), &sizeRead)))
            {
                if (isUpdate && rc == 0xe05)
                {
                    panel->addLine("No updates found.");
                    menu::g_menu->pushPanel(panel);
                    return;
                }
                else
                {
                    error::critical("menu::tinfo_menu::listContentRecordsSelected", "Failed to get content records", rc);
                    return;
                }
            }

            entriesRead = sizeRead / sizeof(NCMContentRecord);

            std::stringstream ss;
            ss << title->getName() << " Content Records: ";
            panel->addLine(ss.str());
            panel->addLine("");
            ss.str("");

            for (size_t i = 0; i < entriesRead; i++)
            {
                auto contentRecord = contentRecords[i];
                char contentStoragePath[FS_MAX_PATH];

                ss << "NcaId: " << utils::toHexString(reinterpret_cast<u8 *>(&contentRecord.ncaID), sizeof(contentRecord.ncaID));
                panel->addLine(ss.str());
                ss.str("");
                ss << " Size: " << (*(reinterpret_cast<u64 *>(contentRecord.size)) & 0xFFFFFFFFFFFF);
                panel->addLine(ss.str());
                ss.str("");
                ss << " Type: " << getContentRecordTypeName(contentRecord.type) << " (" << static_cast<int>(contentRecord.type) << ")";
                panel->addLine(ss.str());
                ss.str("");

                if (R_SUCCEEDED(ncmContentStorageGetPath(&g_contentStorage, &contentRecord.ncaID, contentStoragePath)))
                {
                    ss << " Path: " << std::string(contentStoragePath);
                    panel->addLine(ss.str());
                    ss.str("");
                }

                panel->addLine("");
            }

            ss.str("");
            ss << "Total content records read: " << entriesRead;
            panel->addLine(ss.str());

            menu::g_menu->pushPanel(panel);
        }
    }
}