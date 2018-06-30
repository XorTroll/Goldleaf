#pragma once

#include <switch.h>

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
    }
}