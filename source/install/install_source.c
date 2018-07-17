#include "install/install_source.h"

#include "install/nsp.h"
#include "install/nsp_extracted.h"

Result openInstallFile(InstallFile *out, InstallContext *context, char *name)
{
    switch (context->sourceType)
    {
        case InstallSourceType_Extracted:
            return openExtractedInstallFile(out, context, name);

        case InstallSourceType_Nsp:
            return openNspInstallFile(out, context, name);

        case InstallSourceType_None:
        default:
            return -1;
    }
}

Result openInstallFileWithExt(InstallFile *out, InstallContext *context, char *ext)
{
    switch (context->sourceType)
    {
        case InstallSourceType_Extracted:
            return openExtractedInstallFileWithExt(out, context, ext);

        case InstallSourceType_Nsp:
            return openNspInstallFileWithExt(out, context, ext);

        case InstallSourceType_None:
        default:
            return -1;
    }
}

Result readInstallFile(InstallContext *context, InstallFile *file, size_t off, void *buff, size_t len)
{
    switch (context->sourceType)
    {
        case InstallSourceType_Extracted:
            return readExtractedInstallFile(context, file, off, buff, len);

        case InstallSourceType_Nsp:
            return readNspInstallFile(context, file, off, buff, len);

        case InstallSourceType_None:
        default:
            return -1;
    }
}

void closeInstallFile(InstallContext *context, InstallFile *file)
{
    switch (context->sourceType)
    {
        case InstallSourceType_Extracted:
            closeExtractedInstallFile(context, file);
            break;

        case InstallSourceType_Nsp:
            closeNspInstallFile(context, file);
            break;

        case InstallSourceType_None:
            ;
    }
}