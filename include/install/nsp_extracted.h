#pragma once

#include "install/install_source.h"

Result openExtractedInstallFile(InstallFile *out, InstallContext *context, char *name);
Result openExtractedInstallFileWithExt(InstallFile *out, InstallContext *context, char *ext);
Result readExtractedInstallFile(InstallContext *context, InstallFile *file, size_t off, void *buff, size_t bufSize);
void closeExtractedInstallFile(InstallContext *context, InstallFile *file);