
/*

    Goldleaf - Nintendo Switch title manager homebrew (open-source)

    Copyright 2018 - Goldleaf project, developed and managed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

*/

#pragma once
#include <switch.h>

namespace gleaf::ncm
{
    enum class ContentMetaType : u8
    {
        SystemProgram = 0x1,
        SystemData = 0x2,
        SystemUpdate = 0x3,
        BootImagePackage = 0x4,
        BootImagePackageSafe = 0x5,
        Application = 0x80,
        Patch = 0x81,
        AddOnContent = 0x82,
        Delta = 0x83,
    };

    enum class ContentType : u8
    {
        Meta = 0x0, 
        Program = 0x1, 
        Data = 0x2, 
        Control = 0x3, 
        OfflineHTML = 0x4, 
        LegalInformation = 0x5, 
        DeltaFragment = 0x6,
    };

    struct ContentMetaHeader
    {
        u64 ApplicationId;
        u32 TitleVersion;
        ContentMetaType Type;
        u8 Pad;
        u16 ExtendedHeaderSize;
        u16 ContentCount;
        u16 ContentMetaCount;
        u8 Attributes;
        u8 Pad1[0x3];
        u32 RequiredDownloadSystemVersion;
        u8 Pad2[0x4];
    } PACKED;

    struct ApplicationMetaExtendedHeader
    {
        u64 PatchApplicationId;
        u32 RequiredSystemVersion;
        u32 Pad;
    } PACKED;

    struct PatchMetaExtendedHeader
    {
        u64 ApplicationApplicationId;
        u32 RequiredSystemVersion;
        u32 ExtendedDataSize;
        u8 Pad[0x8];
    } PACKED;

    struct AddOnContentMetaExtendedHeader
    {
        u64 ApplicationApplicationId;
        u32 RequiredApplicationVersion;
        u32 Pad;
    } PACKED;

    struct ContentRecord
    {
        NcmNcaId NCAId;
        u8 Size[0x6];
        ContentType Type;
        u8 Unknown;
    } PACKED;

    struct HashedContentRecord
    {
        u8 Hash[0x20];
        ContentRecord Record;
    } PACKED;

    struct InstallContentMetaHeader
    {
        u16 ExtendedHeaderSize;
        u16 ContentCount;
        u16 ContentMetaCount;
        u16 Pad;
    } PACKED;
}