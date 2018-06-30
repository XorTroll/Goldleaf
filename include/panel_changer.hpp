#pragma once

#include <memory>
#include <switch.h>
#include "title.hpp"

namespace menu
{
    namespace main_menu
    {
        void titleInfoSelected();
        void exitSelected();
    }

    namespace tinfo_menu
    {
        void storageSelected(FsStorageId storageId);
        void titleSelected(std::shared_ptr<Title> title);
        void listContentRecordsSelected(std::shared_ptr<Title> title, bool isUpdate);
    }
}