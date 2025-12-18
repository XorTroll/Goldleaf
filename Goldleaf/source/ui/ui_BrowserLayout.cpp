
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_BrowserLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <hos/hos_Payload.hpp>
#include <es/es_CommonCertificate.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    extern std::string g_Clipboard;

    namespace {

        std::stack<u32> g_EntryIndexStack;

        inline bool IsHiddenContent(const std::string &path) {
            return !path.empty() && (path[0] == '.');
        }

    }

    void BrowserLayout::OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) {
        if(keys_down & HidNpadButton_B) {
            if(this->GoBack()) {
                this->UpdateElements(-1);
            }
            else {
                g_MainApplication->ReturnToParentLayout();
            }
        }
        else if(keys_down & HidNpadButton_X) {
            if(!g_Clipboard.empty()) {
                auto exp = fs::GetExplorerForPath(g_Clipboard);
                const bool clipboard_is_dir = exp->IsDirectory(g_Clipboard);
                auto entry_icon = clipboard_is_dir ? GetCommonIcon(CommonIconKind::Directory) : GetCommonIconForExtension(fs::GetExtension(g_Clipboard));
                const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(222), cfg::Strings.GetString(223) + "\n(" + g_Clipboard + ")", { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true, entry_icon);
                if(option == 0) {
                    const auto item_name = fs::GetBaseName(g_Clipboard);
                    g_MainApplication->ShowLayout(g_MainApplication->GetCopyLayout());
                    g_MainApplication->GetCopyLayout()->StartCopy(g_Clipboard, this->GetExplorer()->FullPathFor(item_name));
                    this->UpdateElements();
                    g_Clipboard = "";
                }
            }
            else {
                g_MainApplication->ShowNotification(cfg::Strings.GetString(224));
            }
        }
        else if(keys_down & HidNpadButton_L) {
            const auto file_name = ShowKeyboard(cfg::Strings.GetString(225), "");
            if(!file_name.empty()) {
                const auto full_path = this->GetExplorer()->FullPathFor(file_name);
                if(this->GetExplorer()->IsFile(full_path) || this->GetExplorer()->IsDirectory(full_path)) {
                    HandleResult(rc::goldleaf::ResultEntryAlreadyPresent, cfg::Strings.GetString(255));
                }
                else {
                    this->GetExplorer()->CreateFile(full_path);
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(227) + " \'" + file_name + "\'");
                    this->UpdateElements();
                }
            }
        }
        else if(keys_down & HidNpadButton_R) {
            const auto dir_name = ShowKeyboard(cfg::Strings.GetString(250), "");
            if(!dir_name.empty()) {
                const auto full_path = this->GetExplorer()->FullPathFor(dir_name);
                if(this->GetExplorer()->IsFile(full_path) || this->GetExplorer()->IsDirectory(full_path)) {
                    HandleResult(rc::goldleaf::ResultEntryAlreadyPresent, cfg::Strings.GetString(255));
                }
                else {
                    this->GetExplorer()->CreateDirectory(full_path);
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(228) + " \'" + dir_name + "\'");
                    this->UpdateElements();
                }
            }
        }
    }

    void BrowserLayout::OnFileSelected(const std::string &item, const std::string &full_item, const std::string &pres_full_item) {
        const auto ext = LowerCaseString(fs::GetExtension(item));
        const auto item_size = this->cur_exp->GetFileSize(full_item);
        std::string icon_path = "";
        auto msg = cfg::Strings.GetString(52) + " ";
        if(ext == "nsp") {
            msg += cfg::Strings.GetString(53);
        }
        else if(ext == "nro") {
            msg += cfg::Strings.GetString(54);
        }
        else if(ext == "tik") {
            msg += cfg::Strings.GetString(55);
        }
        else if(ext == "nca") {
            msg += cfg::Strings.GetString(57);
        }
        else if(ext == "nacp") {
            msg += cfg::Strings.GetString(58);
        }
        else if((ext == "jpg") || (ext == "jpeg") || (ext == "png") || (ext == "webp")) {
            msg += cfg::Strings.GetString(59);
            icon_path = full_item;
        }
        else {
            msg += cfg::Strings.GetString(270);
        }
        msg += "\n\n" + cfg::Strings.GetString(64) + " " + fs::FormatSize(this->cur_exp->GetFileSize(full_item));
        const auto is_bin = this->cur_exp->IsFileBinary(full_item);
        std::vector<std::string> dialog_opts;
        u32 option_count = 5;
        if(ext == "nsp") {
            dialog_opts.push_back(cfg::Strings.GetString(65));
            option_count++;
        }
        else if(ext == "nro") {
            dialog_opts.push_back(cfg::Strings.GetString(66));
            option_count++;
            dialog_opts.push_back(cfg::Strings.GetString(453));
            option_count++;
        }
        else if(ext == "tik") {
            dialog_opts.push_back(cfg::Strings.GetString(67));
            option_count++;
        }
        else if(ext == "nacp") {
            dialog_opts.push_back(cfg::Strings.GetString(69));
            option_count++;
        }
        else if((ext == "jpg") || (ext == "jpeg")) {
            dialog_opts.push_back(cfg::Strings.GetString(70));
            option_count++;
        }
        else if(ext == "bin") {
            dialog_opts.push_back(cfg::Strings.GetString(66));
            option_count++;
        }
        else if(!is_bin) {
            dialog_opts.push_back(cfg::Strings.GetString(71));
            option_count++;
        }
        dialog_opts.push_back(cfg::Strings.GetString(72));
        dialog_opts.push_back(cfg::Strings.GetString(73));
        dialog_opts.push_back(cfg::Strings.GetString(74));
        dialog_opts.push_back(cfg::Strings.GetString(75));
        dialog_opts.push_back(cfg::Strings.GetString(18));
        const auto option_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(76), msg, dialog_opts, true, pu::sdl2::TextureHandle::New(pu::ui::render::LoadImageFromFile(icon_path)));
        if(option_1 < 0) {
            return;
        }

        if(ext == "nsp") {
            switch(option_1) {
                case 0: {
                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), cfg::Strings.GetString(78), { cfg::Strings.GetString(19), cfg::Strings.GetString(79), cfg::Strings.GetString(18) }, true);
                    if(option_2 < 0) {
                        return;
                    }
                    const auto dst = (option_2 == 0) ? NcmStorageId_SdCard : NcmStorageId_BuiltInUser;
                    const auto file_size = this->cur_exp->GetFileSize(full_item);
                    const auto free_space = fs::GetFreeSpaceForPartition(fs::GetPartitionFromStorageId(dst));
                    if(free_space < file_size) {
                        HandleResult(rc::goldleaf::ResultNotEnoughSize, cfg::Strings.GetString(251));
                        return;
                    }
                    g_MainApplication->ShowLayout(g_MainApplication->GetInstallLayout());
                    g_MainApplication->GetInstallLayout()->StartInstall(full_item, pres_full_item, this->cur_exp, dst);
                    this->ResetMenuHead();
                    break;
                }
            }
        }
        else if(ext == "nro") {
            switch(option_1) {
                case 0: {
                    if(GetExecutableMode() == ExecutableMode::NRO) {
                        const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(98), cfg::Strings.GetString(99), { cfg::Strings.GetString(66), cfg::Strings.GetString(18) }, true);
                        if(option_2 < 0) {
                            return;
                        }
                        envSetNextLoad(full_item.c_str(), full_item.c_str());
                        g_MainApplication->CloseWithFadeOut();
                        return;
                    }
                    else {
                        g_MainApplication->DisplayDialog(cfg::Strings.GetString(98), cfg::Strings.GetString(100), { cfg::Strings.GetString(234) }, false);
                        return;
                    }
                    break;
                }
                case 1: {
                    NroHeader nro_h = {};
                    size_t romfs_offset = 0;
                    this->cur_exp->StartFile(full_item, fs::FileMode::Read);
                    if(this->cur_exp->ReadFile(full_item, sizeof(NroStart), sizeof(nro_h), &nro_h) == sizeof(nro_h)) {
                        if(nro_h.magic == NROHEADER_MAGIC) {
                            NroAssetHeader nro_asset_h = {};
                            if(this->cur_exp->ReadFile(full_item, nro_h.size, sizeof(nro_asset_h), &nro_asset_h) == sizeof(nro_asset_h)) {
                                if(nro_asset_h.magic == NROASSETHEADER_MAGIC) {
                                    if(nro_asset_h.romfs.size > 0) {
                                        romfs_offset = nro_h.size + nro_asset_h.romfs.offset;
                                    }
                                }
                            }
                        }
                    }
                    this->cur_exp->EndFile();

                    if(romfs_offset > 0) {
                        const auto romfs_id = std::to_string(randomGet64());
                        const auto mnt_name = "nroromfs-" + romfs_id;
                        const auto rc = romfsMountFromFsdev(full_item.c_str(), romfs_offset, mnt_name.c_str());
                        if(R_SUCCEEDED(rc)) {
                            auto romfs_exp = new fs::RomFsExplorer("NroRomFs-" + romfs_id, mnt_name, true);
                            romfs_exp->SetWriteAccessBlocked(true);
                            g_MainApplication->GetExploreMenuLayout()->AddMountedExplorerWithCommonIcon(romfs_exp, "RomFs (" + fs::GetBaseName(full_item) + ")", CommonIconKind::NAND);
                            g_MainApplication->ShowNotification(cfg::Strings.GetString(454));
                        }
                        else {
                            romfs_offset = 0;
                        }
                    }
                    
                    if(romfs_offset == 0) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(455));
                    }
                    break;
                }
            }
        }
        else if(ext == "tik") {
            switch(option_1) {
                case 0: {
                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(101), cfg::Strings.GetString(102), { cfg::Strings.GetString(234), cfg::Strings.GetString(18) }, true);
                    if(option_2 == 0) {
                        const auto tik_file_size = this->cur_exp->GetFileSize(full_item);
                        auto tik_read_buf = fs::AllocateWorkBuffer(tik_file_size);
                        this->cur_exp->ReadFile(full_item, 0, tik_file_size, tik_read_buf);
                        const auto rc = esImportTicket(tik_read_buf, tik_file_size, es::CommonCertificateData, es::CommonCertificateSize);
                        fs::DeleteWorkBuffer(tik_read_buf);
                        if(R_FAILED(rc)) {
                            HandleResult(rc, cfg::Strings.GetString(103));
                        }
                    }
                    break;
                }
            }
        }
        else if(ext == "nacp")  {
            switch(option_1) {
                case 0: {
                    NacpStruct nacp_item = {};
                    const auto file_size = this->cur_exp->GetFileSize(full_item);
                    if(file_size < sizeof(NacpStruct)) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(341));
                        return;
                    }
                    this->cur_exp->ReadFile(full_item, 0, sizeof(nacp_item), &nacp_item);

                    auto name = cnt::FindApplicationNacpName(nacp_item);
                    if(name.empty()) {
                        name = cfg::Strings.GetString(106);
                    }
                    auto author = cnt::FindApplicationNacpAuthor(nacp_item);
                    if(author.empty()) {
                        author = cfg::Strings.GetString(107);
                    }

                    auto msg = cfg::Strings.GetString(108) + "\n\n";
                    msg += cfg::Strings.GetString(91) + " " + name;
                    msg += "\n" + cfg::Strings.GetString(92) + " " + author;
                    msg += "\n" + cfg::Strings.GetString(109) + " " + nacp_item.display_version;

                    msg += "\n" + cfg::Strings.GetString(110) + " ";
                    switch(nacp_item.startup_user_account) {
                        case 0: {
                            msg += cfg::Strings.GetString(112);
                            break;
                        }
                        case 1: {
                            msg += cfg::Strings.GetString(111);
                            break;
                        }
                        case 2: {
                            msg += cfg::Strings.GetString(113);
                            break;
                        }
                        default: {
                            msg += cfg::Strings.GetString(114);
                            break;
                        }
                    }

                    msg += "\n" + cfg::Strings.GetString(115) + " ";
                    switch(nacp_item.screenshot) {
                        case 0: {
                            msg += cfg::Strings.GetString(111);
                            break;
                        }
                        case 1: {
                            msg += cfg::Strings.GetString(112);
                            break;
                        }
                        default: {
                            msg += cfg::Strings.GetString(114);
                            break;
                        }
                    }

                    msg += "\n" + cfg::Strings.GetString(116) + " ";
                    switch(nacp_item.video_capture) {
                        case 0: {
                            msg += cfg::Strings.GetString(112);
                            break;
                        }
                        case 1: {
                            msg += cfg::Strings.GetString(117);
                            break;
                        }
                        case 2: {
                            msg += cfg::Strings.GetString(111);
                            break;
                        }
                        default: {
                            msg += cfg::Strings.GetString(114);
                            break;
                        }
                    }

                    msg += "\n" + cfg::Strings.GetString(118) + " ";
                    switch(nacp_item.logo_type) {
                        case 0: {
                            msg += cfg::Strings.GetString(119);
                            break;
                        }
                        case 2: {
                            msg += cfg::Strings.GetString(120);
                            break;
                        }
                        default: {
                            msg += cfg::Strings.GetString(114);
                            break;
                        }
                    }

                    g_MainApplication->DisplayDialog(cfg::Strings.GetString(58), msg, { cfg::Strings.GetString(234) }, false);
                    break;
                }
            }
        }
        else if((ext == "jpg") || (ext == "jpeg")) {
            switch(option_1) {
                case 0: {
                    if(!acc::HasSelectedUser()) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(318));
                        return;
                    }

                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(121), cfg::Strings.GetString(122), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                    if(option_2 < 0) {
                        return;
                    }

                    const auto file_size = this->cur_exp->GetFileSize(full_item);
                    auto icon_read_buf = fs::AllocateWorkBuffer(file_size);
                    this->cur_exp->ReadFile(full_item, 0, file_size, icon_read_buf);

                    const auto rc = acc::EditUserIcon(icon_read_buf, file_size);
                    if(R_SUCCEEDED(rc)) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(123));
                    }
                    else {
                        HandleResult(rc, cfg::Strings.GetString(124));
                    }

                    fs::DeleteWorkBuffer(icon_read_buf);
                    break;
                }
            }
        }
        else if(ext == "bin")  {
            switch(option_1) {
                case 0: {
                    if(item_size == 0) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(481));
                        return;
                    }

                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(125), cfg::Strings.GetString(126), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                    if(option_2 < 0) {
                        return;
                    }
                    
                    const auto ok = hos::RebootWithPayload(full_item);
                    if(!ok) {
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(460));
                    }
                    break;
                }
            }
        }
        else if(!is_bin) {
            switch(option_1) {
                case 0: {
                    g_MainApplication->ShowLayout(g_MainApplication->GetFileContentLayout());
                    g_MainApplication->GetFileContentLayout()->LoadFile(full_item, pres_full_item, this->cur_exp, false);
                    break;
                }
            }
        }

        const auto view_option = option_count - 5;
        const auto copy_option = option_count - 4;
        const auto delete_option = option_count - 3;
        const auto rename_option = option_count - 2;
        if(option_1 == static_cast<s32>(view_option)) {
            if(item_size == 0) {
                g_MainApplication->ShowNotification(cfg::Strings.GetString(481));
            }
            else {
                g_MainApplication->ShowLayout(g_MainApplication->GetFileContentLayout());
                g_MainApplication->GetFileContentLayout()->LoadFile(full_item, pres_full_item, this->cur_exp, true);
            }
        }
        else if(option_1 == static_cast<s32>(copy_option)) {
            UpdateClipboard(full_item);
        }
        else if(option_1 == static_cast<s32>(delete_option)) {
            this->PromptDeleteFile(full_item);
        }
        else if(option_1 == static_cast<s32>(rename_option)) {
            if(this->CheckWriteAccess()) {
                const auto new_name = ShowKeyboard(cfg::Strings.GetString(130), item);
                if(!new_name.empty()) {
                    if(new_name == item) {
                        return;
                    }
                    const auto new_path = this->cur_exp->FullPathFor(new_name);
                    if(this->cur_exp->IsFile(new_path) || this->cur_exp->IsDirectory(new_path)) {
                        HandleResult(rc::goldleaf::ResultEntryAlreadyPresent, cfg::Strings.GetString(254));
                    }
                    else {
                        this->cur_exp->RenameFile(full_item, new_path);
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(133));
                        this->UpdateElements(this->browse_menu->GetSelectedIndex());
                    }
                }
            }
        }
    }

    void BrowserLayout::OnDirectorySelected(const std::string &item, const std::string &full_item, const std::string &pres_full_item) {
        const auto ipc_full_item = this->cur_exp->RemoveMountName(full_item);
        AmsSuUpdateInformation update_info;
        auto rc = amssuGetUpdateInformation(ipc_full_item.c_str(), &update_info);
        const auto is_valid_update = R_SUCCEEDED(rc);

        const auto files = this->cur_exp->GetFiles(full_item);
        std::vector<std::string> nsps;
        for(const auto &file: files) {
            if(g_Settings.json_settings.fs.value().ignore_hidden_files.value() && IsHiddenContent(file)) {
                continue;
            }

            const auto path = full_item + "/" + file;
            const auto ext = LowerCaseString(fs::GetExtension(path));
            if(ext == "nsp") {
                nsps.push_back(file);
            }
        }
        std::vector<std::string> extra_options = { cfg::Strings.GetString(281) };
        if(!nsps.empty()) {
            extra_options.push_back(cfg::Strings.GetString(282));
        }
        extra_options.push_back(cfg::Strings.GetString(18));

        auto msg = cfg::Strings.GetString(134);
        if(g_Settings.json_settings.fs.value().compute_directory_sizes.value()) {
            // This can be pretty slow, so only do it if the user wants it
            msg += "\n\n" + cfg::Strings.GetString(237) + " " + fs::FormatSize(this->cur_exp->GetDirectorySize(full_item));
        }
        const auto option_1 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(135), msg, { cfg::Strings.GetString(415), cfg::Strings.GetString(73), cfg::Strings.GetString(74), cfg::Strings.GetString(75), cfg::Strings.GetString(280), cfg::Strings.GetString(18) }, true);
        switch(option_1) {
            case 0: {
                if(is_valid_update) {
                    std::string update_msg = "";
                    const auto version_str = std::to_string((update_info.version >> 26) & 0x1F) + "." + std::to_string((update_info.version >> 20) & 0x1F) + "." + std::to_string((update_info.version >> 16) & 0xF);
                    update_msg += " - " + cfg::Strings.GetString(417) + " " + version_str + "\n\n";
                    update_msg += " - " + cfg::Strings.GetString(418) + " " + (update_info.exfat_supported ? cfg::Strings.GetString(111) : cfg::Strings.GetString(112));
                    
                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(416), update_msg, { cfg::Strings.GetString(419), cfg::Strings.GetString(18) }, true);
                    if(option_2 == 0) {
                        AmsSuUpdateValidationInfo update_validation_info;
                        Result validate_rc;
                        Result validate_exfat_rc;
                        rc = amssuValidateUpdate(ipc_full_item.c_str(), &update_validation_info, &validate_rc, &validate_exfat_rc);
                        if(R_SUCCEEDED(rc) && R_SUCCEEDED(validate_rc) && R_SUCCEEDED(validate_exfat_rc)) {
                            const auto option_3 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(420), cfg::Strings.GetString(421), { cfg::Strings.GetString(423), cfg::Strings.GetString(18) }, true);
                            if(option_3 == 0) {
                                const auto option_4 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(424), cfg::Strings.GetString(425), { cfg::Strings.GetString(111), cfg::Strings.GetString(112), cfg::Strings.GetString(18) }, true);
                                if((option_4 == 0) || (option_4 == 1)) {
                                    const auto with_exfat = option_4 == 0;
                                    SetSysFirmwareVersion fw_ver;
                                    GLEAF_RC_ASSERT(setsysGetFirmwareVersion(&fw_ver));
                                    g_MainApplication->LoadCommonIconMenuData(true, cfg::Strings.GetString(424), CommonIconKind::Update, std::string(fw_ver.display_version) + " → " + version_str + "...");
                                    g_MainApplication->ShowLayout(g_MainApplication->GetUpdateInstallLayout());
                                    g_MainApplication->GetUpdateInstallLayout()->InstallUpdate(ipc_full_item, with_exfat);
                                }
                            }
                        }
                        else {
                            auto failed_rc = rc;
                            if(R_FAILED(validate_rc)) {
                                failed_rc = validate_rc;
                            }
                            if(R_FAILED(validate_exfat_rc)) {
                                failed_rc = validate_exfat_rc;
                            }
                            HandleResult(failed_rc, cfg::Strings.GetString(422));
                        }
                    }
                }
                else {
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(433));
                }
                break;
            }
            case 1: {
                UpdateClipboard(full_item);
                break;
            }
            case 2: {
                if(this->CheckWriteAccess()) {
                    const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(325), cfg::Strings.GetString(326), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
                    if(option_2 < 0) {
                        return;
                    }
                    this->cur_exp->DeleteDirectory(full_item);
                    g_MainApplication->ShowNotification(cfg::Strings.GetString(327));
                    this->UpdateElements();
                }
                break;
            }
            case 3: {
                if(this->CheckWriteAccess())  {
                    const auto new_name = ShowKeyboard(cfg::Strings.GetString(238), item);
                    if(!new_name.empty()) {
                        if(new_name == item) {
                            // TODO: special handling?
                            return;
                        }
                        const auto new_path = this->cur_exp->FullPathFor(new_name);
                        if(this->cur_exp->IsFile(new_path) || this->cur_exp->IsDirectory(new_path)) {
                            HandleResult(rc::goldleaf::ResultEntryAlreadyPresent, cfg::Strings.GetString(254));
                        }
                        else {
                            this->cur_exp->RenameDirectory(full_item, new_path);
                            g_MainApplication->ShowNotification(cfg::Strings.GetString(139));
                            this->UpdateElements();
                        }
                    }
                }
                break;
            }
            case 4: {
                const auto option_2 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(280), cfg::Strings.GetString(134), extra_options, true);
                switch(option_2) {
                    case 0: {
                        this->cur_exp->SetArchiveBit(full_item);
                        this->UpdateElements(this->browse_menu->GetSelectedIndex());
                        g_MainApplication->ShowNotification(cfg::Strings.GetString(303));
                        break;
                    }
                    case 1: {
                        const auto option_3 = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), cfg::Strings.GetString(78), { cfg::Strings.GetString(19), cfg::Strings.GetString(79), cfg::Strings.GetString(18) }, true);
                        if(option_3 < 0) {
                            return;
                        }
                        const auto dst = (option_3 == 0) ? NcmStorageId_SdCard : NcmStorageId_BuiltInUser;

                        const auto option_skip = g_MainApplication->DisplayDialog(cfg::Strings.GetString(77), cfg::Strings.GetString(529), { cfg::Strings.GetString(111), cfg::Strings.GetString(112), cfg::Strings.GetString(18) }, true);
                        if(option_skip < 0) {
                            return;
                        }
                        const bool skip_if_installed = (option_skip == 0);

                        // const auto scan_subdirs = g_MainApplication->DisplayDialog("Install", "Scan subdirectories for NSP files?", { "Yes", "No", "Cancel" }, true);
                        // if(scan_subdirs < 0) {
                        //     return;
                        // }
                        // const bool scan_subdirectories = (scan_subdirs == 0);

                        bool any_installed = false;
                        for(const auto &nsp_name: nsps) {
                            const auto nsp_path = full_item + "/" + nsp_name;
                            const auto pres_nsp_path = pres_full_item + "/" + nsp_name;
                            
                            const auto file_size = this->cur_exp->GetFileSize(nsp_path);
                            const auto free_space = fs::GetFreeSpaceForPartition(fs::GetPartitionFromStorageId(dst));
                            if(free_space < file_size) {
                                HandleResult(rc::goldleaf::ResultNotEnoughSize, cfg::Strings.GetString(251));
                                return;
                            }
                            g_MainApplication->ShowLayout(g_MainApplication->GetInstallLayout());
                            const auto installed = g_MainApplication->GetInstallLayout()->StartInstall(nsp_path, pres_nsp_path, this->cur_exp, dst, true, skip_if_installed);
                            if(installed) {
                                any_installed = true;
                            }
                        }

                        if(!any_installed) {
                            g_MainApplication->ShowNotification(cfg::Strings.GetString(530));
                        }

                        this->ResetMenuHead();
                        break;
                    }
                }
                break;
            }
        }
    }

    BrowserLayout::BrowserLayout() : pu::ui::Layout() {
        this->cur_exp = fs::GetSdCardExplorer();
        this->browse_menu = pu::ui::elm::Menu::New(0, 280, pu::ui::render::ScreenWidth, g_Settings.GetColorScheme().menu_base, g_Settings.GetColorScheme().menu_base_focus, g_Settings.json_settings.ui.value().menu_item_size.value(), ComputeDefaultMenuItemCount(g_Settings.json_settings.ui.value().menu_item_size.value()));
        g_Settings.ApplyToMenu(this->browse_menu);
        this->empty_dir_text = pu::ui::elm::TextBlock::New(30, 630, cfg::Strings.GetString(49));
        this->empty_dir_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->empty_dir_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->empty_dir_text->SetColor(g_Settings.GetColorScheme().text);
        this->Add(this->browse_menu);
        this->Add(this->empty_dir_text);

        this->SetOnInput(std::bind(&BrowserLayout::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void BrowserLayout::ChangePartitionExplorer(fs::Explorer *exp, const bool update_contents) {
        this->cur_exp = exp;
        if(update_contents) {
            this->UpdateElements();
        }
    }

    void BrowserLayout::ChangePartitionSdCard(const bool update_contents) {
        this->ChangePartitionExplorer(fs::GetSdCardExplorer(), update_contents);
    }

    void BrowserLayout::ChangePartitionNAND(const fs::Partition partition, const bool update_contents) {
        fs::Explorer *exp = nullptr;
        switch(partition) {
            case fs::Partition::PRODINFOF: {
                exp = fs::GetPRODINFOFExplorer();
                break;
            }
            case fs::Partition::NANDSafe: {
                exp = fs::GetNANDSafeExplorer();
                break;
            }
            case fs::Partition::NANDUser: {
                exp = fs::GetNANDUserExplorer();
                break;
            }
            case fs::Partition::NANDSystem: {
                exp = fs::GetNANDSystemExplorer();
                break;
            }
            default: {
                return;
            }
        }
        this->ChangePartitionExplorer(exp, update_contents);
    }
    
    void BrowserLayout::ChangePartitionRemotePcDrive(const std::string &mount_name, const bool update_contents) {
        this->ChangePartitionExplorer(fs::GetRemotePCExplorer(mount_name), update_contents);
    }

    void BrowserLayout::ChangePartitionDrive(const UsbHsFsDevice &drv, const bool update_contents) {
        this->ChangePartitionExplorer(fs::GetDriveExplorer(drv), update_contents);
    }

    void BrowserLayout::UpdateElements(const int idx) {
        g_Settings.ApplyToMenu(this->browse_menu);
        const auto contents = this->cur_exp->GetContents();
        this->browse_menu->ClearItems();
        this->ResetMenuHead();
        this->browse_menu->SetVisible(!contents.empty());
        this->empty_dir_text->SetVisible(contents.empty());
        if(!contents.empty()) {
            for(const auto &item: contents) {
                if(g_Settings.json_settings.fs.value().ignore_hidden_files.value() && IsHiddenContent(item)) {
                    continue;
                }

                auto menu_item = pu::ui::elm::MenuItem::New(item);
                menu_item->SetColor(g_Settings.GetColorScheme().text);
                if(this->cur_exp->IsDirectory(item)) {
                    menu_item->SetIcon(this->cur_exp->IsDirectoryEmpty(item) ? GetCommonIcon(CommonIconKind::DirectoryEmpty) : GetCommonIcon(CommonIconKind::Directory));
                }
                else {
                    const auto ext = LowerCaseString(fs::GetExtension(item));
                    menu_item->SetIcon(GetCommonIconForExtension(ext));
                }
                menu_item->AddOnKey(std::bind(&BrowserLayout::fsItems_DefaultKey, this, item));
                menu_item->AddOnKey(std::bind(&BrowserLayout::fsItems_Y, this, item), HidNpadButton_Y);
                this->browse_menu->AddItem(menu_item);
            }
            u32 tmp_idx = 0;
            if(idx < 0) {
                if(!g_EntryIndexStack.empty()) {
                    tmp_idx = g_EntryIndexStack.top();
                    g_EntryIndexStack.pop();
                }
            }
            else {
                tmp_idx = static_cast<u32>(idx);
                if(tmp_idx >= contents.size()) {
                    tmp_idx = 0;
                }
            }
            this->browse_menu->SetSelectedIndex(tmp_idx);
        }
    }

    void BrowserLayout::ResetMenuHead() {
        g_MainApplication->LoadMenuHead(this->cur_exp->GetPresentableCwd());
    }

    void BrowserLayout::HandleFileDirectly(const std::string &path) {
        const auto dir = fs::GetBaseDirectory(path);
        const auto file_name = fs::GetBaseName(path);
        this->ChangePartitionRemotePcDrive(dir);

        auto &items = this->browse_menu->GetItems();
        const auto it = std::find_if(items.begin(), items.end(), [&](pu::ui::elm::MenuItem::Ref &item) -> bool {
            return item->GetName() == file_name;
        });
        if(it == items.end()) {
            return;
        }

        const auto idx = std::distance(items.begin(), it);
        this->browse_menu->SetSelectedIndex(idx);
        this->fsItems_DefaultKey(file_name);
    }

    void BrowserLayout::PromptDeleteFile(const std::string &path) {
        if(this->CheckWriteAccess()) {
            const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(127), cfg::Strings.GetString(128), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            if(option < 0) {
                return;
            }

            this->cur_exp->DeleteFile(path);
            g_MainApplication->ShowNotification(cfg::Strings.GetString(129));
            auto tmp_idx = this->browse_menu->GetSelectedIndex();
            if(tmp_idx > 0) {
                tmp_idx--;
            }
            this->UpdateElements(tmp_idx);
        }
    }

    bool BrowserLayout::GoBack() {
        return this->cur_exp->NavigateBack();
    }

    bool BrowserLayout::CheckWriteAccess() {
        if(this->cur_exp->IsWriteAccessBlocked()) {
            g_MainApplication->ShowNotification(cfg::Strings.GetString(476));
            return false;
        }
        else if(this->cur_exp->ShouldWarnOnWriteAccess()) {
            const auto option = g_MainApplication->DisplayDialog(cfg::Strings.GetString(50), cfg::Strings.GetString(51), { cfg::Strings.GetString(111), cfg::Strings.GetString(18) }, true);
            return option == 0;
        }
        else {
            return true;
        }
    }

    void BrowserLayout::fsItems_DefaultKey(const std::string &item) {
        const auto full_item = this->cur_exp->FullPathFor(item);
        const auto pres_full_item = this->cur_exp->FullPresentablePathFor(item);
        if(this->cur_exp->NavigateForward(full_item)) {
            g_EntryIndexStack.push(this->browse_menu->GetSelectedIndex());
            this->UpdateElements();
        }
        else {
            this->OnFileSelected(item, full_item, pres_full_item);
        }
    }

    void BrowserLayout::fsItems_Y(const std::string &item) {
        const auto full_item = this->cur_exp->FullPathFor(item);
        const auto pres_full_item = this->cur_exp->FullPresentablePathFor(item);

        if(this->cur_exp->IsDirectory(full_item)) {
            this->OnDirectorySelected(item, full_item, pres_full_item);
        }
        else {
            this->OnFileSelected(item, full_item, pres_full_item);
        }
    }

}
