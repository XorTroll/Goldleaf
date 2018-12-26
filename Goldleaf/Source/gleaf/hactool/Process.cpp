#include <gleaf/hactool/Process.hpp>
#include <gleaf/fs.hpp>

namespace gleaf::hactool
{
    Extraction Extraction::MakeExeFs(std::string OutExeFs)
    {
        Extraction ext;
        ext.DoExeFs = true;
        ext.DoRomFs = false;
        ext.DoLogo = false;
        ext.ExeFs = OutExeFs;
        return ext;
    }

    Extraction Extraction::MakeRomFs(std::string OutRomFs)
    {
        Extraction ext;
        ext.DoExeFs = false;
        ext.DoRomFs = true;
        ext.DoLogo = false;
        ext.RomFs = OutRomFs;
        return ext;
    }

    Extraction Extraction::MakeLogo(std::string OutLogo)
    {
        Extraction ext;
        ext.DoExeFs = false;
        ext.DoRomFs = false;
        ext.DoLogo = true;
        ext.Logo = OutLogo;
        return ext;
    }

    bool Process(std::string Input, Extraction Mode, ExtractionFormat Format, std::string KeyFile)
    {
        if(!gleaf::fs::IsFile(Input)) return false;
        if(!gleaf::fs::IsFile(KeyFile)) return false;
        hactool_ctx_t tool_ctx;
        hactool_ctx_t base_ctx; /* Context for base NCA, if used. */
        nca_ctx_t nca_ctx;
        filepath_t keypath;
        nca_init(&nca_ctx);
        memset(&tool_ctx, 0, sizeof(tool_ctx));
        memset(&base_ctx, 0, sizeof(base_ctx));
        filepath_init(&keypath);
        nca_ctx.tool_ctx = &tool_ctx;
        nca_ctx.is_cli_target = false;
        nca_ctx.tool_ctx->file_type = FILETYPE_NCA;
        base_ctx.file_type = FILETYPE_NCA; 
        nca_ctx.tool_ctx->action = ACTION_INFO | ACTION_EXTRACT;
        pki_initialize_keyset(&tool_ctx.settings.keyset, KEYSET_RETAIL);
        switch(Format)
        {
            case ExtractionFormat::XCI:
                nca_ctx.tool_ctx->file_type = FILETYPE_XCI;
                break;
            case ExtractionFormat::NCA:
                nca_ctx.tool_ctx->file_type = FILETYPE_NCA;
                break;
            case ExtractionFormat::PFS0:
                nca_ctx.tool_ctx->file_type = FILETYPE_PFS0;
                break;
        }
        if(Mode.DoExeFs)
        {
            if(Mode.ExeFs == "") return false;
            nca_ctx.tool_ctx->settings.exefs_dir_path.enabled = 1;
            filepath_set(&nca_ctx.tool_ctx->settings.exefs_dir_path.path, Mode.ExeFs.c_str());
        }
        if(Mode.DoRomFs)
        {
            if(Mode.RomFs == "") return false;
            nca_ctx.tool_ctx->settings.romfs_dir_path.enabled = 1;
            filepath_set(&nca_ctx.tool_ctx->settings.romfs_dir_path.path, Mode.RomFs.c_str());
        }
        if(Mode.DoLogo)
        {
            if(Mode.Logo == "") return false;
            filepath_set(&nca_ctx.tool_ctx->settings.section_dir_paths[2], Mode.Logo.c_str());
        }
        filepath_set(&keypath, KeyFile.c_str());
        FILE *keyfile = NULL;
        if(keypath.valid == VALIDITY_VALID) keyfile = os_fopen(keypath.os_path, OS_MODE_READ);

        if(keyfile != NULL)
        {
            extkeys_initialize_keyset(&tool_ctx.settings.keyset, keyfile);
            if (tool_ctx.settings.has_sdseed) {
                for (unsigned int key = 0; key < 2; key++) {
                    for (unsigned int i = 0; i < 0x20; i++) {
                        tool_ctx.settings.keyset.sd_card_key_sources[key][i] ^= tool_ctx.settings.sdseed[i & 0xF];
                    }
                }
            }
            pki_derive_keys(&tool_ctx.settings.keyset);
            fclose(keyfile);
        }

        if ((tool_ctx.file = fopen(Input.c_str(), "rb")) == NULL && tool_ctx.file_type != FILETYPE_BOOT0) {
            fprintf(stderr, "unable to open: %s\n", strerror(errno));
            return false;
        }
        
        switch (tool_ctx.file_type) {
            case FILETYPE_NCA: {
                if (nca_ctx.tool_ctx->base_nca_ctx != NULL) {
                    memcpy(&base_ctx.settings.keyset, &tool_ctx.settings.keyset, sizeof(nca_keyset_t));
                    base_ctx.settings.known_titlekeys = tool_ctx.settings.known_titlekeys;
                    nca_ctx.tool_ctx->base_nca_ctx->tool_ctx = &base_ctx;
                    nca_process(nca_ctx.tool_ctx->base_nca_ctx);
                    int found_romfs = 0;
                    for (unsigned int i = 0; i < 4; i++) {
                        if (nca_ctx.tool_ctx->base_nca_ctx->section_contexts[i].is_present && nca_ctx.tool_ctx->base_nca_ctx->section_contexts[i].type == ROMFS) {
                            found_romfs = 1;
                            break;
                        }
                    }
                    if (found_romfs == 0) {
                        fprintf(stderr, "Unable to locate RomFS in base NCA!\n");
                        return false;
                    }
                }

                nca_ctx.file = tool_ctx.file;
                nca_process(&nca_ctx);
                nca_free_section_contexts(&nca_ctx);
                
                if (nca_ctx.tool_ctx->base_file_type == BASEFILE_FAKE) {
                    nca_ctx.tool_ctx->base_file = NULL;
                }
                
                if (nca_ctx.tool_ctx->base_file != NULL) {
                    fclose(nca_ctx.tool_ctx->base_file);
                    if (nca_ctx.tool_ctx->base_file_type == BASEFILE_NCA) {
                        nca_free_section_contexts(nca_ctx.tool_ctx->base_nca_ctx);
                        free(nca_ctx.tool_ctx->base_nca_ctx);
                    }
                }     
                break;
            }
            case FILETYPE_PFS0: {
                pfs0_ctx_t pfs0_ctx;
                memset(&pfs0_ctx, 0, sizeof(pfs0_ctx));
                pfs0_ctx.file = tool_ctx.file;
                pfs0_ctx.tool_ctx = &tool_ctx;
                pfs0_process(&pfs0_ctx);
                if (pfs0_ctx.header) {
                    free(pfs0_ctx.header);
                }
                if (pfs0_ctx.npdm) {
                    free(pfs0_ctx.npdm);
                }
                break;
            }
            case FILETYPE_ROMFS: {
                romfs_ctx_t romfs_ctx;
                memset(&romfs_ctx, 0, sizeof(romfs_ctx));
                romfs_ctx.file = tool_ctx.file;
                romfs_ctx.tool_ctx = &tool_ctx;
                romfs_process(&romfs_ctx);
                if (romfs_ctx.files) {
                    free(romfs_ctx.files);
                }
                if (romfs_ctx.directories) {
                    free(romfs_ctx.directories);
                }
                break;
            }
            case FILETYPE_NCA0_ROMFS: {
                nca0_romfs_ctx_t romfs_ctx;
                memset(&romfs_ctx, 0, sizeof(romfs_ctx));
                romfs_ctx.file = tool_ctx.file;
                romfs_ctx.tool_ctx = &tool_ctx;
                nca0_romfs_process(&romfs_ctx);
                if (romfs_ctx.files) {
                    free(romfs_ctx.files);
                }
                if (romfs_ctx.directories) {
                    free(romfs_ctx.directories);
                }
                break;
            }
            case FILETYPE_NPDM: {
                npdm_t raw_hdr;
                memset(&raw_hdr, 0, sizeof(raw_hdr));
                if (fread(&raw_hdr, 1, sizeof(raw_hdr), tool_ctx.file) != sizeof(raw_hdr)) {
                    fprintf(stderr, "Failed to read NPDM header!\n");
                    return false;
                }
                if (raw_hdr.magic != MAGIC_META) {
                    fprintf(stderr, "NPDM seems corrupt!\n");
                    return false;
                }
                uint64_t npdm_size = raw_hdr.aci0_size + raw_hdr.aci0_offset;
                if (raw_hdr.acid_offset + raw_hdr.acid_size > npdm_size) {
                    npdm_size = raw_hdr.acid_offset + raw_hdr.acid_size;
                }
                fseeko64(tool_ctx.file, 0, SEEK_SET);
                npdm_t *npdm = (npdm_t*)malloc(npdm_size);
                if (npdm == NULL) {
                    fprintf(stderr, "Failed to allocate NPDM!\n");
                    return false;
                }
                if (fread(npdm, 1, npdm_size, tool_ctx.file) != npdm_size) {
                    fprintf(stderr, "Failed to read NPDM!\n");
                    return false;
                }
                npdm_process(npdm, &tool_ctx);
                break;
            }
            case FILETYPE_HFS0: {
                hfs0_ctx_t hfs0_ctx;
                memset(&hfs0_ctx, 0, sizeof(hfs0_ctx));
                hfs0_ctx.file = tool_ctx.file;
                hfs0_ctx.tool_ctx = &tool_ctx;
                hfs0_process(&hfs0_ctx);
                if (hfs0_ctx.header) {
                    free(hfs0_ctx.header);
                }
                break;
            }
            case FILETYPE_PACKAGE1: {
                pk11_ctx_t pk11_ctx;
                memset(&pk11_ctx, 0, sizeof(pk11_ctx));
                pk11_ctx.file = tool_ctx.file;
                pk11_ctx.tool_ctx = &tool_ctx;
                pk11_process(&pk11_ctx);
                if (pk11_ctx.pk11) {
                    free(pk11_ctx.pk11);
                }
                break;
            }
            case FILETYPE_PACKAGE2: {
                pk21_ctx_t pk21_ctx;
                memset(&pk21_ctx, 0, sizeof(pk21_ctx));
                pk21_ctx.file = tool_ctx.file;
                pk21_ctx.tool_ctx = &tool_ctx;
                pk21_process(&pk21_ctx);
                if (pk21_ctx.sections) {
                    free(pk21_ctx.sections);
                }
                break;
            }
            case FILETYPE_INI1: {
                ini1_ctx_t ini1_ctx;
                memset(&ini1_ctx, 0, sizeof(ini1_ctx));
                ini1_ctx.file = tool_ctx.file;
                ini1_ctx.tool_ctx = &tool_ctx;
                ini1_process(&ini1_ctx);
                if (ini1_ctx.header) {
                    free(ini1_ctx.header);
                }
                break;
            }
            case FILETYPE_KIP1: {
                kip1_ctx_t kip1_ctx;
                memset(&kip1_ctx, 0, sizeof(kip1_ctx));
                kip1_ctx.file = tool_ctx.file;
                kip1_ctx.tool_ctx = &tool_ctx;
                kip1_process(&kip1_ctx);
                if (kip1_ctx.header) {
                    free(kip1_ctx.header);
                }
                break;
            }
            case FILETYPE_NSO0: {
                nso0_ctx_t nso0_ctx;
                memset(&nso0_ctx, 0, sizeof(nso0_ctx));
                nso0_ctx.file = tool_ctx.file;
                nso0_ctx.tool_ctx = &tool_ctx;
                nso0_process(&nso0_ctx);
                if (nso0_ctx.header) {
                    free(nso0_ctx.header);
                }
                if (nso0_ctx.uncompressed_header) {
                    free(nso0_ctx.uncompressed_header);
                }
                break;
            }
            case FILETYPE_XCI: {
                xci_ctx_t xci_ctx;
                memset(&xci_ctx, 0, sizeof(xci_ctx));
                xci_ctx.file = tool_ctx.file;
                xci_ctx.tool_ctx = &tool_ctx;
                xci_process(&xci_ctx);
                break;
            }
            case FILETYPE_BOOT0: {
                nca_keyset_t new_keyset;
                memcpy(&new_keyset, &tool_ctx.settings.keyset, sizeof(new_keyset));
                for (unsigned int i = 0; i < 0x10; i++) {
                    if (tool_ctx.settings.keygen_sbk[i] != 0) {
                        memcpy(new_keyset.secure_boot_key, tool_ctx.settings.keygen_sbk, 0x10);
                    }
                }
                for (unsigned int i = 0; i < 0x10; i++) {
                    if (tool_ctx.settings.keygen_tsec[i] != 0) {
                        memcpy(new_keyset.tsec_key, tool_ctx.settings.keygen_tsec, 0x10);
                    }
                }
                for (unsigned int i = 0; tool_ctx.file != NULL && i < 0x20; i++) {
                    fseek(tool_ctx.file, 0x180000 + 0x200 * i, SEEK_SET);
                    if (fread(&new_keyset.encrypted_keyblobs[i], sizeof(new_keyset.encrypted_keyblobs[i]), 1, tool_ctx.file) != 1) {
                        fprintf(stderr, "Error: Failed to read encrypted_keyblob_%02x from boot0!\n", i);
                        return false;
                    }
                }
                printf("Deriving keys...\n");
                pki_derive_keys(&new_keyset);
                printf("--\n");
                printf("All derivable keys (using loaded sources):\n\n");
                pki_print_keys(&new_keyset);
                break;
            }
            default: {
                fprintf(stderr, "Unknown File Type!\n\n");
                return false;
            }
        }
        
        if (tool_ctx.settings.known_titlekeys.titlekeys != NULL) {
            free(tool_ctx.settings.known_titlekeys.titlekeys);
        }

        if (tool_ctx.file != NULL) {
            fclose(tool_ctx.file);
        }
        printf("Done!\n");

        return true;
    }
}