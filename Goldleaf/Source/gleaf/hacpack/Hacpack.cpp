#include "Hacpack.hpp"
#include <iostream>
#include <unistd.h>
#include <fstream>
using namespace std;

namespace gleaf::hacpack
{
    Build Build::MakeProgramNCA(u64 ApplicationId, string ExeFs, string RomFs, string Logo)
    {
        Build b;
        b.NSP = false;
        b.NCA = NCAType::Program;
        b.ApplicationId = ApplicationId;
        b.ExeFs = ExeFs;
        b.RomFs = RomFs;
        b.Logo = Logo;
        return b;
    }

    Build Build::MakeControlNCA(u64 ApplicationId, string ControlFs)
    {
        Build b;
        b.NSP = false;
        b.NCA = NCAType::Control;
        b.ApplicationId = ApplicationId;
        b.RomFs = ControlFs;
        return b;
    }

    Build Build::MakeManualNCA(u64 ApplicationId, string HtmlFs)
    {
        Build b;
        b.NSP = false;
        b.NCA = NCAType::Manual;
        b.ApplicationId = ApplicationId;
        b.RomFs = HtmlFs;
        return b;
    }

    Build Build::MakeMetaNCA(u64 ApplicationId, string Program, string Control, string LegalInfo, string OfflineHtml)
    {
        Build b;
        b.NSP = false;
        b.NCA = NCAType::Meta;
        b.ApplicationId = ApplicationId;
        b.MProgramNCA = Program;
        b.MControlNCA = Control;
        b.MLegalInfoNCA = LegalInfo;
        b.MOfflineHtmlNCA = OfflineHtml;
        return b;
    }

    Build Build::MakeNSP(u64 ApplicationId, string NCADir)
    {
        Build b;
        b.ApplicationId = ApplicationId;
        b.NSP = true;
        b.NCADir = NCADir;
        return b;
    }

    bool Process(std::string Output, Build Mode, PackageFormat Format, std::string KeyFile)
    {
        bool ret = true;
        hp_settings_t settings;
        memset(&settings, 0, sizeof(settings));

        printf("hacPack %s by The-4n\n\n", HACPACK_VERSION);

        filepath_init(&settings.out_dir);
        filepath_init(&settings.exefs_dir);
        filepath_init(&settings.romfs_dir);
        filepath_init(&settings.logo_dir);
        filepath_init(&settings.programnca);
        filepath_init(&settings.controlnca);
        filepath_init(&settings.legalnca);
        filepath_init(&settings.htmldocnca);
        filepath_init(&settings.datanca);
        filepath_init(&settings.publicdatanca);
        filepath_init(&settings.metanca);
        filepath_init(&settings.ncadir);
        filepath_init(&settings.cnmt);

        // Hardcode default temp directory
        filepath_init(&settings.temp_dir);
        filepath_set(&settings.temp_dir, "sdmc:/goldleaf/dump/temp/hacpack_temp");

        // Hardcode default backup directory
        filepath_init(&settings.backup_dir);
        filepath_set(&settings.backup_dir, "sdmc:/goldleaf/dump/temp/hacpack_backup");

        filepath_t keypath;
        filepath_init(&keypath);
        pki_initialize_keyset(&settings.keyset);
        // Default keyset filepath
        filepath_set(&keypath, KeyFile.c_str());

        // Default Settings
        settings.keygeneration = 1;
        settings.sdk_version = 0x000C1100;
        settings.keyareakey = (unsigned char *)calloc(1, 0x10);
        memset(settings.keyareakey, 4, 0x10);

        settings.title_id = Mode.ApplicationId;
        filepath_set(&settings.out_dir, Output.c_str());
        if(Mode.NSP)
        {
            settings.file_type = FILE_TYPE_NSP;
            filepath_set(&settings.ncadir, Mode.NCADir.c_str());
        }
        else
        {
            settings.file_type = FILE_TYPE_NCA;
            switch(Mode.NCA)
            {
                case NCAType::Program:
                    settings.nca_type = NCA_TYPE_PROGRAM;
                    filepath_set(&settings.exefs_dir, Mode.ExeFs.c_str());
                    if(Mode.RomFs != "") filepath_set(&settings.romfs_dir, Mode.RomFs.c_str());
                    if(Mode.Logo != "") filepath_set(&settings.logo_dir, Mode.Logo.c_str());
                    break;
                case NCAType::Control:
                    settings.nca_type = NCA_TYPE_CONTROL;
                    filepath_set(&settings.romfs_dir, Mode.RomFs.c_str());
                    break;
                case NCAType::Manual:
                    settings.nca_type = NCA_TYPE_MANUAL;
                    filepath_set(&settings.romfs_dir, Mode.RomFs.c_str());
                    break;
                case NCAType::Meta:
                    settings.nca_type = NCA_TYPE_META;
                    settings.title_type = TITLE_TYPE_APPLICATION;
                    filepath_set(&settings.programnca, Mode.MProgramNCA.c_str());
                    filepath_set(&settings.controlnca, Mode.MControlNCA.c_str());
                    if(Mode.MLegalInfoNCA != "") filepath_set(&settings.legalnca, Mode.MLegalInfoNCA.c_str());
                    if(Mode.MOfflineHtmlNCA != "") filepath_set(&settings.htmldocnca, Mode.MOfflineHtmlNCA.c_str());
                    break;
            }
        }

        printf("----> Preparing:\n");

        // Try to populate default keyfile.
        FILE *keyfile = NULL;
        keyfile = os_fopen(keypath.os_path, OS_MODE_READ);

        if (keyfile != NULL)
        {
            extkeys_initialize_keyset(&settings.keyset, keyfile);
            pki_derive_keys(&settings.keyset);
            fclose(keyfile);
        }
        else
        {
            fprintf(stderr, "Unable to open keyset '%s'\n"
                            "Use -k or --keyset to specify your keyset path or place your keyset in ." OS_PATH_SEPARATOR "keys.dat\n",
                    keypath.char_path);
            ret = false;
        }

        // Make sure that header_key exists
        uint8_t has_header_Key = 0;
        for (unsigned int i = 0; i < 0x10; i++)
        {
            if (settings.keyset.header_key[i] != 0)
            {
                has_header_Key = 1;
                break;
            }
        }
        if (has_header_Key == 0)
        {
            fprintf(stderr, "Error: header_key is not present in keyset file\n");
            ret = false;
        }

        // Make sure that key_area_key_application_keygen exists
        uint8_t has_kek = 0;
        for (unsigned int kekc = 0; kekc < 0x10; kekc++)
        {
            if (settings.keyset.key_area_keys[settings.keygeneration - 1][0][kekc] != 0)
            {
                has_kek = 1;
                break;
            }
        }
        if (has_kek == 0)
        {
            fprintf(stderr, "Error: key_area_key_application for keygeneration %i is not present in keyset file\n", settings.keygeneration);
            ret = false;
        }

        // Make sure that titlekek_keygen exists if titlekey is specified
        if (settings.has_title_key == 1)
        {
            uint8_t has_titlekek = 0;
            for (unsigned int tkekc = 0; tkekc < 0x10; tkekc++)
            {
                if (settings.keyset.titlekeks[settings.keygeneration - 1][tkekc] != 0)
                {
                    has_titlekek = 1;
                    break;
                }
            }
            if (has_titlekek == 0)
            {
                fprintf(stderr, "Error: titlekek for keygeneration %i is not present in keyset file\n", settings.keygeneration);
                ret = false;
            }
        }

        // Make sure that titleid is within valid range
        if (settings.title_id < 0x0100000000000000 || settings.title_id > 0x0fffffffffffffff)
        {
            fprintf(stderr, "Error: Bad TitleID: %016" PRIx64 "\n"
                            "Valid TitleID range: 0100000000000000 - 0fffffffffffffff\n",
                    settings.title_id);
            ret = false;
        }
        if (settings.title_id > 0x01ffffffffffffff)
            printf("Warning: TitleID %" PRIx64 " is greater than 01ffffffffffffff and it's not suggested\n", settings.title_id);

        // Make sure that outout directory is set
        if (settings.out_dir.valid == VALIDITY_INVALID)
            ret = false;

        // Remove existing temp directory and create new one + out
        printf("Removing existing temp directory\n");
        filepath_remove_directory(&settings.temp_dir);
        printf("Creating temp directory\n");
        os_makedir(settings.temp_dir.os_path);
        printf("Creating out directory\n");
        os_makedir(settings.out_dir.os_path);

        // Create backup directory
        printf("Creating backup directory\n");
        os_makedir(settings.backup_dir.os_path);
        // Add titleid to backup folder path
        filepath_append(&settings.backup_dir, "%016" PRIx64, settings.title_id);
        os_makedir(settings.backup_dir.os_path);

        printf("\n");

        if (settings.file_type == FILE_TYPE_NCA)
        {
            switch (settings.nca_type)
            {
            case NCA_TYPE_PROGRAM:
                if (settings.exefs_dir.valid == VALIDITY_INVALID)
                    ret = false;
                printf("----> Processing NPDM\n");
                npdm_process(&settings);
                printf("\n");
                nca_create_program(&settings);
                break;
            case NCA_TYPE_CONTROL:
                if (settings.romfs_dir.valid == VALIDITY_INVALID)
                    ret = false;
                else if (settings.has_title_key)
                {
                    fprintf(stderr, "Titlekey is not supported for control nca\n");
                    ret = false;
                }
                printf("----> Processing NACP\n");
                nacp_process(&settings);
                printf("\n");
                nca_create_romfs_type(&settings, nca_romfs_get_type(settings.nca_type));
                break;
            case NCA_TYPE_DATA:
                if (settings.romfs_dir.valid == VALIDITY_INVALID)
                    ret = false;
                else if (settings.has_title_key)
                {
                    fprintf(stderr, "Titlekey is not supported for data nca\n");
                    ret = false;
                }
                nca_create_romfs_type(&settings, nca_romfs_get_type(settings.nca_type));
                break;
            case NCA_TYPE_MANUAL:
                if (settings.romfs_dir.valid == VALIDITY_INVALID)
                    ret = false;
                nca_create_romfs_type(&settings, nca_romfs_get_type(settings.nca_type));
                break;
            case NCA_TYPE_PUBLICDATA:
                if (settings.romfs_dir.valid == VALIDITY_INVALID)
                    ret = false;
                nca_create_romfs_type(&settings, nca_romfs_get_type(settings.nca_type));
                break;
            case NCA_TYPE_META:
                if (settings.title_type == 0)
                {
                    fprintf(stderr, "Error: invalid titletype\n");
                    ret = false;
                }
                else if (settings.cnmt.valid == VALIDITY_VALID)
                    nca_create_meta(&settings);
                else if (settings.has_title_key)
                {
                    fprintf(stderr, "Titlekey is not supported for metadata nca\n");
                    ret = false;
                }
                else if ((settings.programnca.valid == VALIDITY_INVALID || settings.controlnca.valid == VALIDITY_INVALID) && settings.title_type == TITLE_TYPE_APPLICATION)
                {
                    fprintf(stderr, "--programnca and/or --controlnca is not set\n");
                    ret = false;
                }
                else if (settings.title_type == TITLE_TYPE_ADDON && settings.publicdatanca.valid == VALIDITY_INVALID)
                {
                    fprintf(stderr, "--publicdatanca is not set\n");
                    ret = false;
                }
                else if (settings.title_type == TITLE_TYPE_SYSTEMPROGRAM && settings.programnca.valid == VALIDITY_INVALID)
                {
                    fprintf(stderr, "--programnca is not set\n");
                    ret = false;
                }
                else if (settings.title_type == TITLE_TYPE_SYSTEMDATA && settings.datanca.valid == VALIDITY_INVALID)
                {
                    fprintf(stderr, "--datanca is not set\n");
                    ret = false;
                }
                else
                    nca_create_meta(&settings);
                break;
            default:
                ret = false;
            }
        }
        else if (settings.file_type == FILE_TYPE_NSP)
        {
            if (settings.ncadir.valid != VALIDITY_INVALID)
            {
                // Create NSP
                printf("----> Creating NSP:\n");
                filepath_t nsp_file_path;
                filepath_init(&nsp_file_path);
                filepath_copy(&nsp_file_path, &settings.out_dir);
                filepath_append(&nsp_file_path, "%016" PRIx64 ".nsp", settings.title_id);
                uint64_t pfs0_size;
                pfs0_build(&settings.ncadir, &nsp_file_path, &pfs0_size);
                printf("\n----> Created NSP: %s\n", nsp_file_path.char_path);
            }
            else
                ret = false;
        }
        else
            ret = false;

        // Remove temp directory
        printf("\n");
        printf("Removing created temp directory\n");
        filepath_remove_directory(&settings.temp_dir);

        printf("\nDone.\n");

        free(settings.keyareakey);
        if(p_htrc() == 1) ret = false;
        return ret;
    }
}