
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

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

extern ui::MainApplication::Ref mainapp;
extern set::Settings gsets;

namespace ui
{
    std::vector<u32> expidxstack;

    PartitionBrowserLayout::PartitionBrowserLayout() : pu::ui::Layout()
    {
        this->gexp = fs::GetSdCardExplorer();
        this->browseMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->browseMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->browseMenu);
        this->dirEmptyText = pu::ui::elm::TextBlock::New(30, 630, set::GetDictionaryEntry(49));
        this->dirEmptyText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->dirEmptyText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->dirEmptyText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->browseMenu);
        this->Add(this->dirEmptyText);
    }

    void PartitionBrowserLayout::ChangePartitionSdCard(bool Update)
    {
        this->gexp = fs::GetSdCardExplorer();
        if(Update) this->UpdateElements();
    }

    void PartitionBrowserLayout::ChangePartitionNAND(fs::Partition Partition, bool Update)
    {
        switch(Partition)
        {
            case fs::Partition::PRODINFOF:
                this->gexp = fs::GetPRODINFOFExplorer();
                break;
            case fs::Partition::NANDSafe:
                this->gexp = fs::GetNANDSafeExplorer();
                break;
            case fs::Partition::NANDUser:
                this->gexp = fs::GetNANDUserExplorer();
                break;
            case fs::Partition::NANDSystem:
                this->gexp = fs::GetNANDSystemExplorer();
                break;
            default:
                break;
        }
        if(Update) this->UpdateElements();
    }
    
    void PartitionBrowserLayout::ChangePartitionPCDrive(String Mount, bool Update)
    {
        this->gexp = fs::GetUSBPCDriveExplorer(Mount);
        if(Update) this->UpdateElements();
    }

    void PartitionBrowserLayout::UpdateElements(int Idx)
    {
        if(!this->elems.empty()) this->elems.clear();
        this->elems = this->gexp->GetContents();
        this->browseMenu->ClearItems();
        mainapp->LoadMenuHead(this->gexp->GetPresentableCwd());
        if(this->elems.empty())
        {
            this->browseMenu->SetVisible(false);
            this->dirEmptyText->SetVisible(true);
        }
        else
        {
            this->browseMenu->SetVisible(true);
            this->dirEmptyText->SetVisible(false);
            for(u32 i = 0; i < this->elems.size(); i++)
            {
                String itm = this->elems[i];
                bool isdir = this->gexp->IsDirectory(itm);
                auto mitm = pu::ui::elm::MenuItem::New(itm);
                mitm->SetColor(gsets.CustomScheme.Text);
                if(isdir) mitm->SetIcon(gsets.PathForResource("/FileSystem/Directory.png"));
                else
                {
                    String ext = fs::GetExtension(itm);
                    if(ext == "nsp") mitm->SetIcon(gsets.PathForResource("/FileSystem/NSP.png"));
                    else if(ext == "nro") mitm->SetIcon(gsets.PathForResource("/FileSystem/NRO.png"));
                    else if(ext == "tik") mitm->SetIcon(gsets.PathForResource("/FileSystem/TIK.png"));
                    else if(ext == "cert") mitm->SetIcon(gsets.PathForResource("/FileSystem/CERT.png"));
                    else if(ext == "nxtheme") mitm->SetIcon(gsets.PathForResource("/FileSystem/NXTheme.png"));
                    else if(ext == "nca") mitm->SetIcon(gsets.PathForResource("/FileSystem/NCA.png"));
                    else if(ext == "nacp") mitm->SetIcon(gsets.PathForResource("/FileSystem/NACP.png"));
                    else if((ext == "jpg") || (ext == "jpeg")) mitm->SetIcon(gsets.PathForResource("/FileSystem/JPEG.png"));
                    else mitm->SetIcon(gsets.PathForResource("/FileSystem/File.png"));
                }
                mitm->AddOnClick(std::bind(&PartitionBrowserLayout::fsItems_Click, this));
                mitm->AddOnClick(std::bind(&PartitionBrowserLayout::fsItems_Click_Y, this), KEY_Y);
                this->browseMenu->AddItem(mitm);
            }
            u32 tmpidx = 0;
            if(Idx < 0)
            {
                if(!expidxstack.empty())
                {
                    tmpidx = expidxstack[expidxstack.size() - 1];
                    expidxstack.pop_back();
                }
            }
            else
            {
                if(this->elems.size() > (u32)Idx) tmpidx = Idx;
            }
            this->browseMenu->SetSelectedIndex(tmpidx);
        }
    }

    void PartitionBrowserLayout::HandleFileDirectly(String Path)
    {
        auto dir = fs::GetBaseDirectory(Path);
        auto fname = fs::GetFileName(Path);
        this->ChangePartitionPCDrive(dir);

        auto items = this->browseMenu->GetItems();
        auto it = std::find_if(items.begin(), items.end(), [&](pu::ui::elm::MenuItem::Ref &item) -> bool
        {
            return (item->GetName() == fname);
        });
        if(it == items.end()) return;

        u32 idx = std::distance(items.begin(), it);
        this->browseMenu->SetSelectedIndex(idx);
        fsItems_Click();
    }

    bool PartitionBrowserLayout::GoBack()
    {
        return this->gexp->NavigateBack();
    }

    bool PartitionBrowserLayout::WarnNANDWriteAccess()
    {
        if(!this->gexp->ShouldWarnOnWriteAccess()) return true;
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(50), set::GetDictionaryEntry(51), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        return (sopt == 0);
    }

    void PartitionBrowserLayout::fsItems_Click()
    {
        if(this->elems.empty()) return;
        String itm = this->browseMenu->GetSelectedItem()->GetName();
        String fullitm = this->gexp->FullPathFor(itm);
        String pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->NavigateForward(fullitm))
        {
            expidxstack.push_back(this->browseMenu->GetSelectedIndex());
            this->UpdateElements();
        }
        else
        {
            String ext = fs::GetExtension(itm);
            String msg = set::GetDictionaryEntry(52) + " ";
            if(ext == "nsp") msg += set::GetDictionaryEntry(53);
            else if(ext == "nro") msg += set::GetDictionaryEntry(54);
            else if(ext == "tik") msg += set::GetDictionaryEntry(55);
            else if(ext == "nxtheme") msg += set::GetDictionaryEntry(56);
            else if(ext == "nca") msg += set::GetDictionaryEntry(57);
            else if(ext == "nacp") msg += set::GetDictionaryEntry(58);
            else if((ext == "jpg") || (ext == "jpeg")) msg += set::GetDictionaryEntry(59);
            else msg += set::GetDictionaryEntry(270);
            msg += "\n\n" + set::GetDictionaryEntry(64) + " " + fs::FormatSize(this->gexp->GetFileSize(fullitm));
            std::vector<String> vopts;
            u32 copt = 5;
            bool ibin = this->gexp->IsFileBinary(fullitm);
            if(ext == "nsp")
            {
                vopts.push_back(set::GetDictionaryEntry(65));
                copt = 6;
            }
            else if(ext == "nro")
            {
                vopts.push_back(set::GetDictionaryEntry(66));
                copt = 6;
            }
            else if(ext == "tik")
            {
                vopts.push_back(set::GetDictionaryEntry(67));
                copt = 6;
            }
            else if(ext == "nxtheme")
            {
                vopts.push_back(set::GetDictionaryEntry(65));
                copt = 6;
            }
            else if(ext == "nacp")
            {
                vopts.push_back(set::GetDictionaryEntry(69));
                copt = 6;
            }
            else if((ext == "jpg") || (ext == "jpeg"))
            {
                vopts.push_back(set::GetDictionaryEntry(70));
                copt = 6;
            }
            else if(ext == "bin")
            {
                if(IsAtmosphere())
                {
                    vopts.push_back(set::GetDictionaryEntry(66));
                    copt = 6;
                }
            }
            else if(!ibin)
            {
                vopts.push_back(set::GetDictionaryEntry(71));
                copt = 6;
            }
            vopts.push_back(set::GetDictionaryEntry(72));
            vopts.push_back(set::GetDictionaryEntry(73));
            vopts.push_back(set::GetDictionaryEntry(74));
            vopts.push_back(set::GetDictionaryEntry(75));
            vopts.push_back(set::GetDictionaryEntry(18));
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(76), msg, vopts, true);
            if(sopt < 0) return;
            int osopt = sopt;
            if(ext == "nsp")
            {
                switch(sopt)
                {
                    case 0:
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(78), { set::GetDictionaryEntry(19), set::GetDictionaryEntry(79), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0) return;
                        Storage dst = Storage::SdCard;
                        if(sopt == 0) dst = Storage::SdCard;
                        else if(sopt == 1) dst = Storage::NANDUser;
                        u64 fsize = this->gexp->GetFileSize(fullitm);
                        u64 rsize = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                        if(rsize < fsize)
                        {
                            HandleResult(err::Make(err::ErrorDescription::NotEnoughSize), set::GetDictionaryEntry(251));
                            return;
                        }
                        mainapp->LoadMenuHead(set::GetDictionaryEntry(145) + " " + pfullitm);
                        mainapp->LoadLayout(mainapp->GetInstallLayout());
                        mainapp->GetInstallLayout()->StartInstall(fullitm, this->gexp, dst);
                        mainapp->LoadLayout(mainapp->GetBrowserLayout());
                        mainapp->LoadMenuHead(this->gexp->GetPresentableCwd());
                        break;
                }
            }
            else if(ext == "nro")
            {
                switch(sopt)
                {
                    case 0:
                        if(GetExecutableMode() == ExecutableMode::NRO)
                        {
                            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(98), set::GetDictionaryEntry(99), { set::GetDictionaryEntry(66), set::GetDictionaryEntry(18) }, true);
                            if(sopt < 0) return;
                            envSetNextLoad(fullitm.AsUTF8().c_str(), fullitm.AsUTF8().c_str());
                            mainapp->CloseWithFadeOut();
                            return;
                        }
                        else
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(98), set::GetDictionaryEntry(100), { set::GetDictionaryEntry(234) }, false);
                            return;
                        }
                        break;
                }
            }
            else if(ext == "tik")
            {
                switch(sopt)
                {
                    case 0:
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(101), set::GetDictionaryEntry(102), { set::GetDictionaryEntry(234), set::GetDictionaryEntry(18) }, true);
                        if(sopt == 0)
                        {
                            auto btik = this->gexp->ReadFile(fullitm);
                            Result rc = es::ImportTicket(btik.data(), btik.size(), es::CertData, es::CertSize);
                            if(rc != 0) HandleResult(rc, set::GetDictionaryEntry(103));
                        }
                        break;
                }
            }
            else if(ext == "nxtheme")
            {
                switch(sopt)
                {
                    case 0:
                        std::string ntnro = "sdmc:/switch/nxthemes_installer/nxthemesinstaller.nro";
                        if(!fs::IsFile(ntnro))
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(104), set::GetDictionaryEntry(105), { set::GetDictionaryEntry(234) }, false);
                            return;
                        }
                        std::string arg = ntnro + " installtheme=" + fullitm.AsUTF8();
                        size_t index = 0;
                        while(true)
                        {
                            index = arg.find(" ", index);
                            if(index == String::npos) break;
                            arg.replace(index, 1, "(_)");
                        }
                        envSetNextLoad(ntnro.c_str(), arg.c_str());
                        mainapp->CloseWithFadeOut();
                        return;
                        break;
                }
            }
            else if(ext == "nacp") 
            {
                switch(sopt)
                {
                    case 0:
                        NacpStruct nacp = {};
                        auto fsize = this->gexp->GetFileSize(fullitm);
                        if(fsize < sizeof(NacpStruct))
                        {
                            mainapp->ShowNotification(set::GetDictionaryEntry(341));
                            return;
                        }
                        this->gexp->ReadFileBlock(fullitm, 0, sizeof(NacpStruct), (u8*)&nacp);
                        NacpStruct *snacp = &nacp;
                        u8 *rnacp = (u8*)snacp;
                        NacpLanguageEntry *lent = NULL;
                        nacpGetLanguageEntry(snacp, &lent);
                        String name = set::GetDictionaryEntry(106);
                        String author = set::GetDictionaryEntry(107);
                        String version = String(snacp->display_version);
                        if(lent != NULL)
                        {
                            name = String(lent->name);
                            author = String(lent->author);
                        }
                        String msg = set::GetDictionaryEntry(108) + "\n\n";
                        msg += set::GetDictionaryEntry(91) + " " + name;
                        msg += "\n" + set::GetDictionaryEntry(92) + " " + author;
                        msg += "\n" + set::GetDictionaryEntry(109) + " " + version;
                        msg += "\n" + set::GetDictionaryEntry(110) + " ";
                        u8 uacc = rnacp[0x3025];
                        if(uacc == 0) msg += set::GetDictionaryEntry(112);
                        else if(uacc == 1) msg += set::GetDictionaryEntry(111);
                        else if(uacc == 2) msg += set::GetDictionaryEntry(113);
                        else msg += set::GetDictionaryEntry(114);
                        u8 scrc = rnacp[0x3034];
                        msg += "\n" + set::GetDictionaryEntry(115) + " ";
                        if(scrc == 0) msg += set::GetDictionaryEntry(111);
                        else if(scrc == 1) msg += set::GetDictionaryEntry(112);
                        else msg += set::GetDictionaryEntry(114);
                        u8 vidc = rnacp[0x3035];
                        msg += "\n" + set::GetDictionaryEntry(116) + " ";
                        if(vidc == 0) msg += set::GetDictionaryEntry(112);
                        else if(vidc == 1) msg += set::GetDictionaryEntry(117);
                        else if(vidc == 2) msg += set::GetDictionaryEntry(111);
                        else msg += set::GetDictionaryEntry(114);
                        u8 logom = rnacp[0x30f0];
                        msg += "\n" + set::GetDictionaryEntry(118) + " ";
                        if(logom == 0) msg += set::GetDictionaryEntry(119);
                        else if(logom == 2) msg += set::GetDictionaryEntry(120);
                        else msg += set::GetDictionaryEntry(114);
                        mainapp->CreateShowDialog(set::GetDictionaryEntry(58), msg, { set::GetDictionaryEntry(234) }, false);
                        break;
                }
            }
            else if((ext == "jpg") || (ext == "jpeg"))
            {
                switch(sopt)
                {
                    case 0:
                        if(!acc::HasUser()) return;
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(121), set::GetDictionaryEntry(122), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0) return;

                        size_t fsize = this->gexp->GetFileSize(fullitm);
                        u8 *iconbuf = new u8[fsize]();
                        this->gexp->ReadFileBlock(fullitm, 0, fsize, iconbuf);

                        auto rc = acc::EditUserIcon(iconbuf, fsize);
                        if(rc == 0) mainapp->ShowNotification(set::GetDictionaryEntry(123));
                        else HandleResult(rc, set::GetDictionaryEntry(124));
                        delete[] iconbuf;
                        break;
                }
            }
            else if(ext == "bin") 
            {
                if(IsAtmosphere()) switch(sopt)
                {
                    case 0:
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(125), set::GetDictionaryEntry(126), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0) return;
                        hos::PayloadProcess(fullitm);
                        break;
                }
            }
            else if(!ibin)
            {
                switch(sopt)
                {
                    case 0:
                        mainapp->LoadLayout(mainapp->GetFileContentLayout());
                        mainapp->GetFileContentLayout()->LoadFile(pfullitm, fullitm, this->gexp, false);
                        break;
                }
            }
            int viewopt = copt - 5;
            int copyopt = copt - 4;
            int delopt = copt - 3;
            int renopt = copt - 2;
            if((osopt == viewopt) && (this->gexp->GetFileSize(fullitm) > 0))
            {
                mainapp->LoadLayout(mainapp->GetFileContentLayout());
                mainapp->GetFileContentLayout()->LoadFile(pfullitm, fullitm, this->gexp, true);
            }
            else if(osopt == copyopt) UpdateClipboard(fullitm);
            else if(osopt == delopt)
            {
                if(this->WarnNANDWriteAccess())
                {
                    sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(127), set::GetDictionaryEntry(128), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                    if(sopt < 0) return;
                    Result rc = 0;
                    this->gexp->DeleteFile(fullitm);
                    if(rc == 0) mainapp->ShowNotification(set::GetDictionaryEntry(129));
                    else HandleResult(rc, set::GetDictionaryEntry(253));
                    u32 tmpidx = this->browseMenu->GetSelectedIndex();
                    if(tmpidx > 0) tmpidx--;
                    this->UpdateElements(tmpidx);
                }
            }
            else if(osopt == renopt)
            {
                String kbdt = AskForText(set::GetDictionaryEntry(130), itm);
                if(kbdt != "")
                {
                    if(kbdt == itm) return;
                    String newren = kbdt;
                    if(this->gexp->IsFile(newren) || this->gexp->IsDirectory(newren)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(254));
                    else if(this->WarnNANDWriteAccess())
                    {
                        int rc = 0;
                        this->gexp->RenameFile(fullitm, newren);
                        if(rc) HandleResult(err::MakeErrno(rc), set::GetDictionaryEntry(254));
                        else
                        {
                            mainapp->ShowNotification(set::GetDictionaryEntry(133));
                            this->UpdateElements(this->browseMenu->GetSelectedIndex());
                        }
                    }
                }
            }
        }
    }

    void PartitionBrowserLayout::fsItems_Click_Y()
    {
        String itm = this->browseMenu->GetSelectedItem()->GetName().AsUTF8().c_str();
        String fullitm = this->gexp->FullPathFor(itm);
        String pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->IsDirectory(fullitm))
        {
            auto files = this->gexp->GetFiles(fullitm);
            std::vector<String> nsps;
            for(u32 i = 0; i < files.size(); i++)
            {
                auto path = fullitm + "/" + files[i];
                if(fs::GetExtension(path) == "nsp") nsps.push_back(files[i]);
            }
            std::vector<String> extraopts = { set::GetDictionaryEntry(281) };
            if(!nsps.empty()) extraopts.push_back(set::GetDictionaryEntry(282));
            extraopts.push_back(set::GetDictionaryEntry(18));
            String msg = set::GetDictionaryEntry(134);
            msg += "\n\n" + set::GetDictionaryEntry(237) + " " + fs::FormatSize(this->gexp->GetDirectorySize(fullitm));
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(135), msg, { set::GetDictionaryEntry(73), set::GetDictionaryEntry(74), set::GetDictionaryEntry(75), set::GetDictionaryEntry(280), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            switch(sopt)
            {
                case 0:
                    UpdateClipboard(fullitm);
                    break;
                case 1:
                    if(this->WarnNANDWriteAccess())
                    {
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(325), set::GetDictionaryEntry(326), {set::GetDictionaryEntry(111), set::GetDictionaryEntry(18)}, true);
                        if(sopt < 0) return;
                        this->gexp->DeleteDirectory(fullitm);
                        mainapp->ShowNotification(set::GetDictionaryEntry(327));
                        this->UpdateElements();
                    }
                    break;
                case 2:
                    {
                        String kbdt = AskForText(set::GetDictionaryEntry(238), itm);
                        if(kbdt != "")
                        {
                            if(kbdt == itm) return;
                            String newren = this->gexp->FullPathFor(kbdt);
                            if(this->gexp->IsFile(newren) || this->gexp->IsDirectory(newren)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(254));
                            else if(this->WarnNANDWriteAccess())
                            {
                                int rc = 0;
                                this->gexp->RenameDirectory(fullitm, newren);
                                if(rc) HandleResult(rc, set::GetDictionaryEntry(254));
                                else mainapp->ShowNotification(set::GetDictionaryEntry(139));
                                this->UpdateElements();
                            }
                        }
                    }
                    break;
                case 3:
                    int sopt2 = mainapp->CreateShowDialog(set::GetDictionaryEntry(280), set::GetDictionaryEntry(134), extraopts, true);
                    switch(sopt2)
                    {
                        case 0:
                            this->gexp->SetArchiveBit(fullitm);
                            this->UpdateElements(this->browseMenu->GetSelectedIndex());
                            mainapp->ShowNotification(set::GetDictionaryEntry(303));
                            break;
                        case 1:
                            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(78), { set::GetDictionaryEntry(19), set::GetDictionaryEntry(79), set::GetDictionaryEntry(18) }, true);
                            if(sopt < 0) return;
                            Storage dst = Storage::SdCard;
                            if(sopt == 0) dst = Storage::SdCard;
                            else if(sopt == 1) dst = Storage::NANDUser;
                            for(u32 i = 0; i < nsps.size(); i++)
                            {
                                auto nsp = fullitm + "/" + nsps[i];
                                auto pnsp = pfullitm + "/" + nsps[i];
                                
                                u64 fsize = this->gexp->GetFileSize(nsp);
                                u64 rsize = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                                if(rsize < fsize)
                                {
                                    HandleResult(err::Make(err::ErrorDescription::NotEnoughSize), set::GetDictionaryEntry(251));
                                    return;
                                }
                                mainapp->LoadMenuHead(set::GetDictionaryEntry(145) + " " + pnsp);
                                mainapp->LoadLayout(mainapp->GetInstallLayout());
                                mainapp->GetInstallLayout()->StartInstall(nsp, this->gexp, dst, true);
                                mainapp->LoadLayout(mainapp->GetBrowserLayout());
                            }
                            mainapp->LoadMenuHead(this->gexp->GetPresentableCwd());
                            break;
                    }
                    break;
            }
        }
    }

    fs::Explorer *PartitionBrowserLayout::GetExplorer()
    {
        return this->gexp;
    }
}