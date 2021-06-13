
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2021 XorTroll

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

#include <ui/ui_PartitionBrowserLayout.hpp>
#include <ui/ui_MainApplication.hpp>
#include <hos/hos_Payload.hpp>
#include <amssu/amssu_Service.hpp>

extern ui::MainApplication::Ref g_MainApplication;
extern cfg::Settings g_Settings;

namespace ui {

    namespace {

        std::vector<u32> g_EntryIndexStack;

    }

    PartitionBrowserLayout::PartitionBrowserLayout() : pu::ui::Layout() {
        this->cur_exp = fs::GetSdCardExplorer();
        this->browse_menu = pu::ui::elm::Menu::New(0, 160, 1280, g_Settings.custom_scheme.Base, g_Settings.menu_item_size, (560 / g_Settings.menu_item_size));
        this->browse_menu->SetOnFocusColor(g_Settings.custom_scheme.BaseFocus);
        g_Settings.ApplyScrollBarColor(this->browse_menu);
        this->empty_dir_text = pu::ui::elm::TextBlock::New(30, 630, cfg::strings::Main.GetString(49));
        this->empty_dir_text->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->empty_dir_text->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->empty_dir_text->SetColor(g_Settings.custom_scheme.Text);
        this->Add(this->browse_menu);
        this->Add(this->empty_dir_text);
    }

    void PartitionBrowserLayout::ChangePartitionExplorer(fs::Explorer *exp, bool update_contents) {
        this->cur_exp = exp;
        if(update_contents) {
            this->UpdateElements();
        }
    }

    void PartitionBrowserLayout::ChangePartitionSdCard(bool update_contents) {
        this->ChangePartitionExplorer(fs::GetSdCardExplorer(), update_contents);
    }

    void PartitionBrowserLayout::ChangePartitionNAND(fs::Partition partition, bool update_contents) {
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
    
    void PartitionBrowserLayout::ChangePartitionPCDrive(String mount_name, bool update_contents) {
        this->ChangePartitionExplorer(fs::GetRemotePCExplorer(mount_name), update_contents);
    }

    void PartitionBrowserLayout::ChangePartitionDrive(UsbHsFsDevice &drv, bool update_contents) {
        this->ChangePartitionExplorer(fs::GetDriveExplorer(drv), update_contents);
    }

    void PartitionBrowserLayout::UpdateElements(int idx) {
        auto contents = this->cur_exp->GetContents();
        this->browse_menu->ClearItems();
        g_MainApplication->LoadMenuHead(this->cur_exp->GetPresentableCwd());
        this->browse_menu->SetVisible(!contents.empty());
        this->empty_dir_text->SetVisible(contents.empty());
        if(!contents.empty()) {
            for(auto &itm: contents) {
                auto mitm = pu::ui::elm::MenuItem::New(itm);
                mitm->SetColor(g_Settings.custom_scheme.Text);
                if(this->cur_exp->IsDirectory(itm)) {
                    mitm->SetIcon(g_Settings.PathForResource("/FileSystem/Directory.png"));
                }
                else {
                    auto ext = LowerCaseString(fs::GetExtension(itm));
                    if(ext == "nsp") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/NSP.png"));
                    }
                    else if(ext == "nro") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/NRO.png"));
                    }
                    else if(ext == "tik") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/TIK.png"));
                    }
                    else if(ext == "cert") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/CERT.png"));
                    }
                    else if(ext == "nxtheme") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/NXTheme.png"));
                    }
                    else if(ext == "nca") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/NCA.png"));
                    }
                    else if(ext == "nacp") {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/NACP.png"));
                    }
                    else if((ext == "jpg") || (ext == "jpeg")) {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/JPEG.png"));
                    }
                    else {
                        mitm->SetIcon(g_Settings.PathForResource("/FileSystem/File.png"));
                    }
                }
                mitm->AddOnClick(std::bind(&PartitionBrowserLayout::fsItems_Click, this, itm));
                mitm->AddOnClick(std::bind(&PartitionBrowserLayout::fsItems_Click_Y, this, itm), KEY_Y);
                this->browse_menu->AddItem(mitm);
            }
            u32 tmp_idx = 0;
            if(idx < 0) {
                if(!g_EntryIndexStack.empty()) {
                    tmp_idx = g_EntryIndexStack.back();
                    g_EntryIndexStack.pop_back();
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

    void PartitionBrowserLayout::HandleFileDirectly(String path) {
        auto dir = fs::GetBaseDirectory(path);
        auto file_name = fs::GetFileName(path);
        this->ChangePartitionPCDrive(dir);

        auto items = this->browse_menu->GetItems();
        const auto it = std::find_if(items.begin(), items.end(), [&](pu::ui::elm::MenuItem::Ref &item) -> bool {
            return item->GetName() == file_name;
        });
        if(it == items.end()) {
            return;
        }

        const auto idx = std::distance(items.begin(), it);
        this->browse_menu->SetSelectedIndex(idx);
        fsItems_Click(file_name);
    }

    bool PartitionBrowserLayout::GoBack() {
        return this->cur_exp->NavigateBack();
    }

    bool PartitionBrowserLayout::WarnWriteAccess() {
        if(!this->cur_exp->ShouldWarnOnWriteAccess()) {
            return true;
        }
        const auto option = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(50), cfg::strings::Main.GetString(51), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
        return option == 0;
    }

    void PartitionBrowserLayout::fsItems_Click(String item) {
        auto full_item = this->cur_exp->FullPathFor(item);
        auto pres_full_item = this->cur_exp->FullPresentablePathFor(item);
        if(this->cur_exp->NavigateForward(full_item)) {
            g_EntryIndexStack.push_back(this->browse_menu->GetSelectedIndex());
            this->UpdateElements();
        }
        else {
            auto ext = LowerCaseString(fs::GetExtension(item));
            auto msg = cfg::strings::Main.GetString(52) + " ";
            if(ext == "nsp") {
                msg += cfg::strings::Main.GetString(53);
            }
            else if(ext == "nro") {
                msg += cfg::strings::Main.GetString(54);
            }
            else if(ext == "tik") {
                msg += cfg::strings::Main.GetString(55);
            }
            else if(ext == "nxtheme") {
                msg += cfg::strings::Main.GetString(56);
            }
            else if(ext == "nca") {
                msg += cfg::strings::Main.GetString(57);
            }
            else if(ext == "nacp") {
                msg += cfg::strings::Main.GetString(58);
            }
            else if((ext == "jpg") || (ext == "jpeg")) {
                msg += cfg::strings::Main.GetString(59);
            }
            else {
                msg += cfg::strings::Main.GetString(270);
            }
            msg += "\n\n" + cfg::strings::Main.GetString(64) + " " + fs::FormatSize(this->cur_exp->GetFileSize(full_item));
            const auto is_bin = this->cur_exp->IsFileBinary(full_item);
            std::vector<String> vopts;
            u32 option_count = 5;
            if(ext == "nsp") {
                vopts.push_back(cfg::strings::Main.GetString(65));
                option_count++;
            }
            else if(ext == "nro") {
                vopts.push_back(cfg::strings::Main.GetString(66));
                option_count++;
            }
            else if(ext == "tik") {
                vopts.push_back(cfg::strings::Main.GetString(67));
                option_count++;
            }
            else if(ext == "nxtheme") {
                vopts.push_back(cfg::strings::Main.GetString(65));
                option_count++;
            }
            else if(ext == "nacp") {
                vopts.push_back(cfg::strings::Main.GetString(69));
                option_count++;
            }
            else if((ext == "jpg") || (ext == "jpeg")) {
                vopts.push_back(cfg::strings::Main.GetString(70));
                option_count++;
            }
            else if(ext == "bin") {
                vopts.push_back(cfg::strings::Main.GetString(66));
                option_count++;
            }
            else if(!is_bin) {
                vopts.push_back(cfg::strings::Main.GetString(71));
                option_count++;
            }
            vopts.push_back(cfg::strings::Main.GetString(72));
            vopts.push_back(cfg::strings::Main.GetString(73));
            vopts.push_back(cfg::strings::Main.GetString(74));
            vopts.push_back(cfg::strings::Main.GetString(75));
            vopts.push_back(cfg::strings::Main.GetString(18));
            const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(76), msg, vopts, true);
            if(option_1 < 0) {
                return;
            }

            if(ext == "nsp") {
                switch(option_1) {
                    case 0: {
                        const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(77), cfg::strings::Main.GetString(78), { cfg::strings::Main.GetString(19), cfg::strings::Main.GetString(79), cfg::strings::Main.GetString(18) }, true);
                        if(option_2 < 0) {
                            return;
                        }
                        const auto dst = (option_2 == 0) ? NcmStorageId_SdCard : NcmStorageId_BuiltInUser;
                        const auto file_size = this->cur_exp->GetFileSize(full_item);
                        const auto free_space = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                        if(free_space < file_size) {
                            HandleResult(err::result::ResultNotEnoughSize, cfg::strings::Main.GetString(251));
                            return;
                        }
                        g_MainApplication->LoadMenuHead(cfg::strings::Main.GetString(145) + " " + pres_full_item);
                        g_MainApplication->LoadLayout(g_MainApplication->GetInstallLayout());
                        g_MainApplication->GetInstallLayout()->StartInstall(full_item, this->cur_exp, dst);
                        g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
                        g_MainApplication->LoadMenuHead(this->cur_exp->GetPresentableCwd());
                        break;
                    }
                }
            }
            else if(ext == "nro") {
                switch(option_1) {
                    case 0: {
                        if(GetExecutableMode() == ExecutableMode::NRO) {
                            const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(98), cfg::strings::Main.GetString(99), { cfg::strings::Main.GetString(66), cfg::strings::Main.GetString(18) }, true);
                            if(option_2 < 0) {
                                return;
                            }
                            envSetNextLoad(full_item.AsUTF8().c_str(), full_item.AsUTF8().c_str());
                            g_MainApplication->CloseWithFadeOut();
                            return;
                        }
                        else {
                            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(98), cfg::strings::Main.GetString(100), { cfg::strings::Main.GetString(234) }, false);
                            return;
                        }
                        break;
                    }
                }
            }
            else if(ext == "tik") {
                switch(option_1) {
                    case 0: {
                        const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(101), cfg::strings::Main.GetString(102), { cfg::strings::Main.GetString(234), cfg::strings::Main.GetString(18) }, true);
                        if(option_2 == 0) {
                            auto read_buf = fs::GetWorkBuffer();
                            const auto tik_file_size = this->cur_exp->GetFileSize(full_item);
                            this->cur_exp->ReadFile(full_item, 0, tik_file_size, read_buf);
                            const auto rc = es::ImportTicket(read_buf, tik_file_size, es::CommonCertificateData, es::CommonCertificateSize);
                            if(R_FAILED(rc)) {
                                HandleResult(rc, cfg::strings::Main.GetString(103));
                            }
                        }
                        break;
                    }
                }
            }
            else if(ext == "nxtheme") {
                // TODO (to consider): shall we continue supporting this?
                // This implementation is really shitty, and only works for SD files...
                switch(option_1) {
                    case 0: {
                        std::string nxthemes_nro_path = "sdmc:/switch/nxthemes_installer/nxthemesinstaller.nro";
                        auto sd_exp = fs::GetSdCardExplorer();
                        if(!sd_exp->IsFile(nxthemes_nro_path)) {
                            g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(104), cfg::strings::Main.GetString(105), { cfg::strings::Main.GetString(234) }, false);
                            return;
                        }
                        auto arg = nxthemes_nro_path + " installtheme=" + full_item.AsUTF8();
                        size_t index = 0;
                        while(true) {
                            index = arg.find(" ", index);
                            if(index == std::string::npos) {
                                break;
                            }
                            arg.replace(index, 1, "(_)");
                        }
                        envSetNextLoad(nxthemes_nro_path.c_str(), arg.c_str());
                        g_MainApplication->CloseWithFadeOut();
                        return;
                    }
                }
            }
            else if(ext == "nacp")  {
                switch(option_1) {
                    case 0: {
                        NacpStruct nacp_item = {};
                        const auto file_size = this->cur_exp->GetFileSize(full_item);
                        if(file_size < sizeof(NacpStruct)) {
                            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(341));
                            return;
                        }
                        this->cur_exp->ReadFile(full_item, 0, sizeof(nacp_item), &nacp_item);

                        auto name = hos::FindNacpName(nacp_item);
                        if(name.empty()) {
                            name = cfg::strings::Main.GetString(106);
                        }
                        auto author = hos::FindNacpAuthor(nacp_item);
                        if(author.empty()) {
                            author = cfg::strings::Main.GetString(107);
                        }

                        auto msg = cfg::strings::Main.GetString(108) + "\n\n";
                        msg += cfg::strings::Main.GetString(91) + " " + name;
                        msg += "\n" + cfg::strings::Main.GetString(92) + " " + author;
                        msg += "\n" + cfg::strings::Main.GetString(109) + " " + nacp_item.display_version;

                        msg += "\n" + cfg::strings::Main.GetString(110) + " ";
                        switch(nacp_item.startup_user_account) {
                            case 0: {
                                msg += cfg::strings::Main.GetString(112);
                                break;
                            }
                            case 1: {
                                msg += cfg::strings::Main.GetString(111);
                                break;
                            }
                            case 2: {
                                msg += cfg::strings::Main.GetString(113);
                                break;
                            }
                            default: {
                                msg += cfg::strings::Main.GetString(114);
                                break;
                            }
                        }

                        msg += "\n" + cfg::strings::Main.GetString(115) + " ";
                        switch(nacp_item.screenshot) {
                            case 0: {
                                msg += cfg::strings::Main.GetString(111);
                                break;
                            }
                            case 1: {
                                msg += cfg::strings::Main.GetString(112);
                                break;
                            }
                            default: {
                                msg += cfg::strings::Main.GetString(114);
                                break;
                            }
                        }

                        msg += "\n" + cfg::strings::Main.GetString(116) + " ";
                        switch(nacp_item.video_capture) {
                            case 0: {
                                msg += cfg::strings::Main.GetString(112);
                                break;
                            }
                            case 1: {
                                msg += cfg::strings::Main.GetString(117);
                                break;
                            }
                            case 2: {
                                msg += cfg::strings::Main.GetString(111);
                                break;
                            }
                            default: {
                                msg += cfg::strings::Main.GetString(114);
                                break;
                            }
                        }

                        msg += "\n" + cfg::strings::Main.GetString(118) + " ";
                        switch(nacp_item.logo_type) {
                            case 0: {
                                msg += cfg::strings::Main.GetString(119);
                                break;
                            }
                            case 2: {
                                msg += cfg::strings::Main.GetString(120);
                                break;
                            }
                            default: {
                                msg += cfg::strings::Main.GetString(114);
                                break;
                            }
                        }

                        g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(58), msg, { cfg::strings::Main.GetString(234) }, false);
                        break;
                    }
                }
            }
            else if((ext == "jpg") || (ext == "jpeg")) {
                switch(option_1) {
                    case 0: {
                        if(!acc::HasSelectedUser()) {
                            return;
                        }
                        const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(121), cfg::strings::Main.GetString(122), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                        if(option_2 < 0) {
                            return;
                        }

                        const auto file_size = this->cur_exp->GetFileSize(full_item);
                        auto icon_buf = fs::GetWorkBuffer();
                        this->cur_exp->ReadFile(full_item, 0, file_size, icon_buf);

                        const auto rc = acc::EditUserIcon(icon_buf, file_size);
                        if(R_SUCCEEDED(rc)) {
                            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(123));
                        }
                        else {
                            HandleResult(rc, cfg::strings::Main.GetString(124));
                        }
                        break;
                    }
                }
            }
            else if(ext == "bin")  {
                switch(option_1) {
                    case 0: {
                        const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(125), cfg::strings::Main.GetString(126), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                        if(option_2 < 0) {
                            return;
                        }
                        
                        const auto ok = hos::RebootWithPayload(full_item);
                        if(!ok) {
                            // TODO: translation?
                            g_MainApplication->ShowNotification("Something failed...");
                        }
                        break;
                    }
                }
            }
            else if(!is_bin) {
                switch(option_1) {
                    case 0: {
                        g_MainApplication->LoadLayout(g_MainApplication->GetFileContentLayout());
                        g_MainApplication->GetFileContentLayout()->LoadFile(full_item, this->cur_exp, false);
                        break;
                    }
                }
            }

            const auto view_option = option_count - 5;
            const auto copy_option = option_count - 4;
            const auto delete_option = option_count - 3;
            const auto rename_option = option_count - 2;
            if((option_1 == view_option) && (this->cur_exp->GetFileSize(full_item) > 0)) {
                g_MainApplication->LoadLayout(g_MainApplication->GetFileContentLayout());
                g_MainApplication->GetFileContentLayout()->LoadFile(full_item, this->cur_exp, true);
            }
            else if(option_1 == copy_option) {
                UpdateClipboard(full_item);
            }
            else if(option_1 == delete_option) {
                if(this->WarnWriteAccess()) {
                    const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(127), cfg::strings::Main.GetString(128), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                    if(option_2 < 0) {
                        return;
                    }

                    this->cur_exp->DeleteFile(full_item);
                    g_MainApplication->ShowNotification(cfg::strings::Main.GetString(129));
                    auto tmp_idx = this->browse_menu->GetSelectedIndex();
                    if(tmp_idx > 0) {
                        tmp_idx--;
                    }
                    this->UpdateElements(tmp_idx);
                }
            }
            else if(option_1 == rename_option) {
                auto new_name = AskForText(cfg::strings::Main.GetString(130), item);
                if(!new_name.empty()) {
                    if(new_name == item) {
                        return;
                    }
                    auto new_path = this->cur_exp->FullPathFor(new_name);
                    if(this->cur_exp->IsFile(new_path) || this->cur_exp->IsDirectory(new_path)) {
                        HandleResult(err::result::ResultEntryAlreadyPresent, cfg::strings::Main.GetString(254));
                    }
                    else if(this->WarnWriteAccess()) {
                        this->cur_exp->RenameFile(full_item, new_path);
                        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(133));
                        this->UpdateElements(this->browse_menu->GetSelectedIndex());
                    }
                }
            }
        }
    }

    void PartitionBrowserLayout::fsItems_Click_Y(String item) {
        auto full_item = this->cur_exp->FullPathFor(item);
        auto pres_full_item = this->cur_exp->FullPresentablePathFor(item);

        auto ipc_full_item = this->cur_exp->RemoveMountName(full_item);
        amssu::UpdateInformation update_info = {};
        auto rc = amssu::GetUpdateInformation(ipc_full_item.AsUTF8().c_str(), &update_info);
        const auto is_valid_update = R_SUCCEEDED(rc);

        if(this->cur_exp->IsDirectory(full_item)) {
            auto files = this->cur_exp->GetFiles(full_item);
            std::vector<String> nsps;
            for(auto &file: files) {
                auto path = full_item + "/" + file;
                auto ext = LowerCaseString(fs::GetExtension(path));
                if(ext == "nsp") {
                    nsps.push_back(file);
                }
            }
            std::vector<String> extra_options = { cfg::strings::Main.GetString(281) };
            if(!nsps.empty()) {
                extra_options.push_back(cfg::strings::Main.GetString(282));
            }
            extra_options.push_back(cfg::strings::Main.GetString(18));

            auto msg = cfg::strings::Main.GetString(134);
            msg += "\n\n" + cfg::strings::Main.GetString(237) + " " + fs::FormatSize(this->cur_exp->GetDirectorySize(full_item));
            const auto option_1 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(135), msg, { cfg::strings::Main.GetString(415), cfg::strings::Main.GetString(73), cfg::strings::Main.GetString(74), cfg::strings::Main.GetString(75), cfg::strings::Main.GetString(280), cfg::strings::Main.GetString(18) }, true);
            switch(option_1) {
                case 0: {
                    if(is_valid_update) {
                        String update_msg = "";
                        const auto version_str = std::to_string((update_info.version >> 26) & 0x1F) + "." + std::to_string((update_info.version >> 20) & 0x1F) + "." + std::to_string((update_info.version >> 16) & 0xF);
                        update_msg += " - " + cfg::strings::Main.GetString(417) + " " + version_str + "\n\n";
                        update_msg += " - " + cfg::strings::Main.GetString(418) + " " + (update_info.exfat_supported ? cfg::strings::Main.GetString(111) : cfg::strings::Main.GetString(112));
                        
                        const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(416), update_msg, { cfg::strings::Main.GetString(419), cfg::strings::Main.GetString(18) }, true);
                        if(option_2 == 0) {
                            amssu::UpdateValidationInfo update_validation_info = {};
                            rc = amssu::ValidateUpdate(ipc_full_item.AsUTF8().c_str(), &update_validation_info);
                            if(R_SUCCEEDED(rc) && R_SUCCEEDED(update_validation_info.result) && R_SUCCEEDED(update_validation_info.exfat_result)) {
                                const auto option_3 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(420), cfg::strings::Main.GetString(421), { cfg::strings::Main.GetString(423), cfg::strings::Main.GetString(18) }, true);
                                if(option_3 == 0) {
                                    const auto option_4 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(424), cfg::strings::Main.GetString(425), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(112), cfg::strings::Main.GetString(18) }, true);
                                    if((option_4 == 0) || (option_4 == 1)) {
                                        const auto with_exfat = option_4 == 0;
                                        SetSysFirmwareVersion fw_ver = {};
                                        setsysGetFirmwareVersion(&fw_ver);
                                        g_MainApplication->LoadMenuData(cfg::strings::Main.GetString(424), "Update", String(fw_ver.display_version) + " → " + version_str + "...");
                                        g_MainApplication->LoadLayout(g_MainApplication->GetUpdateInstallLayout());
                                        g_MainApplication->GetUpdateInstallLayout()->InstallUpdate(ipc_full_item, with_exfat);
                                    }
                                }
                            }
                            else {
                                auto failed_rc = rc;
                                if(R_FAILED(update_validation_info.result)) {
                                    failed_rc = update_validation_info.result;
                                }
                                if(R_FAILED(update_validation_info.exfat_result)) {
                                    failed_rc = update_validation_info.exfat_result;
                                }
                                HandleResult(failed_rc, cfg::strings::Main.GetString(422));
                            }
                        }
                    }
                    else {
                        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(433));
                    }
                    break;
                }
                case 1: {
                    UpdateClipboard(full_item);
                    break;
                }
                case 2: {
                    if(this->WarnWriteAccess()) {
                        const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(325), cfg::strings::Main.GetString(326), { cfg::strings::Main.GetString(111), cfg::strings::Main.GetString(18) }, true);
                        if(option_2 < 0) {
                            return;
                        }
                        this->cur_exp->DeleteDirectory(full_item);
                        g_MainApplication->ShowNotification(cfg::strings::Main.GetString(327));
                        this->UpdateElements();
                    }
                    break;
                }
                case 3: {
                    auto new_name = AskForText(cfg::strings::Main.GetString(238), item);
                    if(!new_name.empty()) {
                        if(new_name == item) {
                            // TODO: special handling?
                            return;
                        }
                        auto new_path = this->cur_exp->FullPathFor(new_name);
                        if(this->cur_exp->IsFile(new_path) || this->cur_exp->IsDirectory(new_path)) {
                            HandleResult(err::result::ResultEntryAlreadyPresent, cfg::strings::Main.GetString(254));
                        }
                        else if(this->WarnWriteAccess()) {
                            this->cur_exp->RenameDirectory(full_item, new_path);
                            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(139));
                            this->UpdateElements();
                        }
                    }
                    break;
                }
                case 4: {
                    const auto option_2 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(280), cfg::strings::Main.GetString(134), extra_options, true);
                    switch(option_2) {
                        case 0: {
                            this->cur_exp->SetArchiveBit(full_item);
                            this->UpdateElements(this->browse_menu->GetSelectedIndex());
                            g_MainApplication->ShowNotification(cfg::strings::Main.GetString(303));
                            break;
                        }
                        case 1: {
                            const auto option_3 = g_MainApplication->CreateShowDialog(cfg::strings::Main.GetString(77), cfg::strings::Main.GetString(78), { cfg::strings::Main.GetString(19), cfg::strings::Main.GetString(79), cfg::strings::Main.GetString(18) }, true);
                            if(option_3 < 0) {
                                return;
                            }
                            const auto dst = (option_2 == 0) ? NcmStorageId_SdCard : NcmStorageId_BuiltInUser;
                            for(auto &nsp_name: nsps) {
                                auto nsp_path = full_item + "/" + nsp_name;
                                auto pres_nsp_path = pres_full_item + "/" + nsp_name;
                                
                                const auto file_size = this->cur_exp->GetFileSize(nsp_path);
                                const auto free_space = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                                if(free_space < file_size) {
                                    HandleResult(err::result::ResultNotEnoughSize, cfg::strings::Main.GetString(251));
                                    return;
                                }
                                g_MainApplication->LoadMenuHead(cfg::strings::Main.GetString(145) + " " + pres_nsp_path);
                                g_MainApplication->LoadLayout(g_MainApplication->GetInstallLayout());
                                g_MainApplication->GetInstallLayout()->StartInstall(nsp_path, this->cur_exp, dst, true);
                                g_MainApplication->LoadLayout(g_MainApplication->GetBrowserLayout());
                            }
                            g_MainApplication->LoadMenuHead(this->cur_exp->GetPresentableCwd());
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

}