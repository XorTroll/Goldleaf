#include "ui/ui_utils.h"

#include <stdio.h>
#include <string.h>

#include "debug.h"

Result getTitleName(u64 titleId, char *outBuf, size_t bufSize)
{
    Result rc = 0;
    NsApplicationControlData appControlData;
    size_t sizeRead;

    fprintf(nxlinkout, "%lx\n", titleId);

    if (R_FAILED(rc = nsGetApplicationControlData(0x1, titleId, &appControlData, sizeof(NsApplicationControlData), &sizeRead)))
    {
        fprintf(nxlinkout, "getTitleName: Failed to get application control data. Error code: 0x%08x\n", rc);
        return rc;
    }
    else if (sizeRead < sizeof(appControlData.nacp))
    {
        fprintf(nxlinkout, "getTitleName: Incorrect size for nacp\n");
        return -1;
    }

    NacpLanguageEntry *languageEntry;

    if (R_FAILED(rc = nacpGetLanguageEntry(&appControlData.nacp, &languageEntry)))
    {
        fprintf(nxlinkout, "getTitleName: Failed to get language entry. Error code: 0x%08x\n", rc);
        return rc;
    }

    if (languageEntry == NULL)
    {
        fprintf(nxlinkout, "getTitleName: Language entry is null\n");
        return -1;
    }

    strncpy(outBuf, languageEntry->name, bufSize);
    return rc;
}