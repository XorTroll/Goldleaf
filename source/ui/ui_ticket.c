#include "ui/ui_ticket.h"

#include <inttypes.h>
#include <machine/endian.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "nx/ipc/es.h"
#include "ui/ui.h"
#include "ui/ui_utils.h"

void showTicketManagementOptionsView(void)
{
    View *view = calloc(1, sizeof(View));
    unsigned char numEntries = 8;
    ViewEntry viewEntries[] = 
    {
        (ViewEntry) 
        {
            .type = ViewEntryType_Heading,
            .text = "Ticket Management",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_None,
        },
        (ViewEntry)
        {
            .type = ViewEntryType_SelectInactive,
            .text = "Install Tik & Cert",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_SelectInactive,
            .text = "Delete Personalized Ticket",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_Select,
            .text = "Delete Common Ticket",
            .onSelected = showSelectDeleteCommonTicket
        },
        (ViewEntry)
        {
            .type = ViewEntryType_SelectInactive,
            .text = "View Title Keys",
        },
        (ViewEntry)
        {
            .type = ViewEntryType_SelectInactive,
            .text = "Dump ETicket Services Data",
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

void showSelectDeleteCommonTicket(void)
{
    Result rc = 0;
    u32 numInstalledTickets = 0;

    if (R_FAILED(rc = esCountCommonTicket(&numInstalledTickets)))
    {
        printf("showSelectDeletePersonalizedTicket: Failed to count common tickets. Error code: 0x%08x\n", rc);
        return;
    }

    RightsId *rightsIds = calloc(numInstalledTickets, sizeof(RightsId));
    u32 numRightsIdsWritten = 0;

    if (R_FAILED(rc = esListCommonTicket(&numRightsIdsWritten, rightsIds, numInstalledTickets * sizeof(RightsId))))
    {
        printf("showSelectDeletePersonalizedTicket: Failed to list common tickets. Error code: 0x%08x\n", rc);
        free(rightsIds);
        return;
    }

    if (numInstalledTickets != numRightsIdsWritten)
    {
        printf("showSelectDeletePersonalizedTicket: Mismatch between num installed tickets and num rights ids written. Error code: 0x%08x\n", rc);
        free(rightsIds);
        return;
    }

    // We currently don't support scrolling, so no more than this will fit
    if (numRightsIdsWritten > 41)
        numRightsIdsWritten = 41;

    View *view = calloc(1, sizeof(View));

    view->viewEntries[0] = (ViewEntry)
    {
        .type = ViewEntryType_Heading,
        .text = "Delete Common Ticket",
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

    for (int i = 0; i < numRightsIdsWritten; i++)
    {
        RightsId rightsId = rightsIds[i];
        u64 titleId = __bswap64(*(u64 *)rightsId.c);
        u64 baseTitleId = titleId & ~0x800;
        u64 keyGeneration = __bswap64(*(u64 *)(rightsId.c + 8));
        char title[256-1-32-1] = {0};

        view->viewEntries[i + 3] = (ViewEntry)
        {
            .type = ViewEntryType_Select,
            .onSelected = onDeleteCommonTicket,
        };

        getTitleName(baseTitleId, title, 256-1-32-1);

        snprintf(view->viewEntries[i + 3].text, 256-1, "%016lx%016lx %s", titleId, keyGeneration, title);
    }
    view->numEntries = 3 + numRightsIdsWritten;

    pushView(view);
    free(rightsIds);
}

void onDeleteCommonTicket(void)
{
    Result rc = 0;
    RightsId rightsId;
    ViewEntry *selectedViewEntry = &g_currentView->viewEntries[g_currentView->cursorPos];
    char *entryText = selectedViewEntry->text;
    char lowerU64[17] = {0};
    char upperU64[17] = {0};

    memcpy(lowerU64, entryText, 16);
    memcpy(upperU64, entryText + 16, 16);

    *(u64 *)rightsId.c = __bswap64(strtoul(lowerU64, NULL, 16));
    *(u64 *)(rightsId.c + 8) = __bswap64(strtoul(upperU64, NULL, 16));

    View *view = calloc(1, sizeof(View));
    pushView(view);

    printBytes(nxlinkout, rightsId.c, sizeof(RightsId), true);

    if (R_FAILED(rc = esDeleteTicket(&rightsId, sizeof(RightsId))))
    {
        printf("onDeleteCommonTicket: Failed to delete common ticket. Error code: 0x%08x\n", rc);
        return;
    }

    printf("Deleted common ticket successfully!\n");
    strcpy(selectedViewEntry->text, "<DELETED>\x00");
    selectedViewEntry->type = ViewEntryType_SelectInactive;
    selectedViewEntry->onSelected = NULL;
}