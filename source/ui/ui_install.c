#include "ui/ui_install.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "install/install.h"
#include "ui/ui.h"

void showTitleManagementOptionsView(void)
{
    View *view = calloc(1, sizeof(View));
    unsigned char numEntries = 7;
    ViewEntry viewEntries[] = 
    {
        (ViewEntry) 
        {
            .type = ViewEntryType_Heading,
            .text = "Title Management",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_None,
        },
        (ViewEntry)
        {
            .type = ViewEntryType_Select,
            .text = "Install NSP",
            .onSelected = showNspInstallOptions,
        },
        (ViewEntry)
        {
            .type = ViewEntryType_Select,
            .text = "Install Extracted NSP",
            .onSelected = showExtractedInstallOptions,
        },
        (ViewEntry)
        {
            .type = ViewEntryType_SelectInactive,
            .text = "Uninstall",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_SelectInactive,
            .text = "Dump NSP",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_Select,
            .text = "Back",
            .onSelected = unwind,
        },
    };

    memcpy(view->viewEntries, viewEntries, sizeof(ViewEntry) * numEntries);
    view->numEntries = numEntries;

    pushView(view);
}

void showExtractedInstallOptions(void)
{
    Result rc;
    FsFileSystem sdFs;
    FsDir dir;

    if (R_FAILED(rc = fsMountSdcard(&sdFs)))
    {
        printf("showExtractedInstallOptions: Failed to mount sd card. Error code: 0x%08x\n", rc);
        return;
    }

    if (R_FAILED(rc = fsFsOpenDirectory(&sdFs, "/tinfoil/extracted/",  FS_DIROPEN_DIRECTORY, &dir)))
    {
        printf("showExtractedInstallOptions: Failed to open extracted dir. Error code: 0x%08x\n", rc);
        fsFsClose(&sdFs);
        return;
    }

    size_t numEntriesRead;
    FsDirectoryEntry *dirEntries = calloc(42, sizeof(FsDirectoryEntry)); // 41 = Max num entries (44) -3 for the heading, the space under it and the back option

    if (R_FAILED(rc = fsDirRead(&dir, 0, &numEntriesRead, 42, dirEntries)))
    {
        printf("showExtractedInstallOptions: Failed to read extracted dir. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    View *view = calloc(1, sizeof(View));

    view->viewEntries[0] = (ViewEntry)
    {
        .type = ViewEntryType_Heading,
        .text = "Install Extracted NSP",
    };

    view->viewEntries[1] = (ViewEntry)
    {
        .type = ViewEntryType_None,
    };

    view->viewEntries[2] = (ViewEntry)
    {
        .type = ViewEntryType_Select,
        .text = "Back",
        .onSelected = unwind
    };

    for (int i = 0; i < numEntriesRead; i++)
    {
        FsDirectoryEntry dirEntry = dirEntries[i];
        ViewEntryType type = ViewEntryType_Select;

        if (dirEntry.type != ENTRYTYPE_DIR)
            continue;

        view->viewEntries[i + 3] = (ViewEntry)
        {
            .type = type,
            .onSelected = onTitleExtractedDirSelected,
        };

        memcpy(view->viewEntries[i + 3].text, dirEntry.name, 256-1);
    }

    view->numEntries = 3 + numEntriesRead;
    pushView(view);

    CLEANUP:
    free(dirEntries);
    fsDirClose(&dir);
    fsFsClose(&sdFs);
}

void onTitleExtractedDirSelected(void)
{
    InstallContext context;
    context.sourceType = InstallSourceType_Extracted;

    // Get the previously selected directory. Note we haven't changed views to this one yet, 
    // so the current view is still the dir selection view.
    snprintf(context.path, FS_MAX_PATH, "/tinfoil/extracted/%s/", g_currentView->viewEntries[g_currentView->cursorPos].text);

    View *view = calloc(1, sizeof(View));
    pushView(view);

    installTitle(&context);
}

void showNspInstallOptions(void)
{
    Result rc;
    FsFileSystem sdFs;
    FsDir dir;

    if (R_FAILED(rc = fsMountSdcard(&sdFs)))
    {
        printf("showNspInstallOptions: Failed to mount sd card. Error code: 0x%08x\n", rc);
        return;
    }

    if (R_FAILED(rc = fsFsOpenDirectory(&sdFs, "/tinfoil/nsp/", FS_DIROPEN_DIRECTORY | FS_DIROPEN_FILE, &dir)))
    {
        printf("showNspInstallOptions: Failed to open nsp dir. Error code: 0x%08x\n", rc);
        fsFsClose(&sdFs);
        return;
    }

    size_t numEntriesRead;
    FsDirectoryEntry *dirEntries = calloc(42, sizeof(FsDirectoryEntry)); // 41 = Max num entries (44) -3 for the heading, the space under it and the back option

    if (R_FAILED(rc = fsDirRead(&dir, 0, &numEntriesRead, 42, dirEntries)))
    {
        printf("showNspInstallOptions: Failed to read extracted dir. Error code: 0x%08x\n", rc);
        goto CLEANUP;
    }

    View *view = calloc(1, sizeof(View));

    view->viewEntries[0] = (ViewEntry)
    {
        .type = ViewEntryType_Heading,
        .text = "Install NSP",
    };

    view->viewEntries[1] = (ViewEntry)
    {
        .type = ViewEntryType_None,
    };

    view->viewEntries[2] = (ViewEntry)
    {
        .type = ViewEntryType_Select,
        .text = "Back",
        .onSelected = unwind
    };

    for (int i = 0; i < numEntriesRead; i++)
    {
        FsDirectoryEntry dirEntry = dirEntries[i];
        ViewEntryType type = ViewEntryType_Select;

        if (dirEntry.type != ENTRYTYPE_FILE || strcmp(strchr(dirEntry.name, '.') + 1, "nsp") != 0)
            continue;

        view->viewEntries[i + 3] = (ViewEntry)
        {
            .type = type,
            .onSelected = onTitleNspSelected,
        };

        memcpy(view->viewEntries[i + 3].text, dirEntry.name, 256-1);
    }

    view->numEntries = 3 + numEntriesRead;
    pushView(view);

    CLEANUP:
    free(dirEntries);
    fsDirClose(&dir);
    fsFsClose(&sdFs);
}

void onTitleNspSelected(void)
{
    InstallContext context;
    context.sourceType = InstallSourceType_Nsp;

    // Get the previously selected directory. Note we haven't changed views to this one yet, 
    // so the current view is still the dir selection view.
    snprintf(context.path, FS_MAX_PATH, "/tinfoil/nsp/%s", g_currentView->viewEntries[g_currentView->cursorPos].text);

    View *view = calloc(1, sizeof(View));
    pushView(view);

    installTitle(&context);
}