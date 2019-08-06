#include <ui/ui_PartitionBrowserLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;
    std::vector<u32> expidxstack;

    PartitionBrowserLayout::PartitionBrowserLayout() : pu::ui::Layout()
    {
        this->gexp = fs::GetSdCardExplorer();
        this->browseMenu = new pu::ui::elm::Menu(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->browseMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->dirEmptyText = new pu::ui::elm::TextBlock(30, 630, set::GetDictionaryEntry(49));
        this->dirEmptyText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->dirEmptyText->SetVerticalAlign(pu::ui::elm::VerticalAlign::Center);
        this->dirEmptyText->SetColor(gsets.CustomScheme.Text);
        this->Add(this->browseMenu);
        this->Add(this->dirEmptyText);
    }

    PartitionBrowserLayout::~PartitionBrowserLayout()
    {
        delete this->dirEmptyText;
        delete this->browseMenu;
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
    
    void PartitionBrowserLayout::ChangePartitionPCDrive(std::string Mount, bool Update)
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
                std::string itm = this->elems[i];
                bool isdir = this->gexp->IsDirectory(itm);
                pu::ui::elm::MenuItem *mitm = new pu::ui::elm::MenuItem(itm);
                mitm->SetColor(gsets.CustomScheme.Text);
                if(isdir) mitm->SetIcon(gsets.PathForResource("/FileSystem/Directory.png"));
                else
                {
                    std::string ext = fs::GetExtension(itm);
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
                if(this->elems.size() > Idx) tmpidx = Idx;
            }
            this->browseMenu->SetSelectedIndex(tmpidx);
        }
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
        std::string itm = this->browseMenu->GetSelectedItem()->GetName().AsUTF8();
        std::string fullitm = this->gexp->FullPathFor(itm);
        std::string pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->NavigateForward(fullitm))
        {
            expidxstack.push_back(this->browseMenu->GetSelectedIndex());
            this->UpdateElements();
        }
        else
        {
            std::string ext = fs::GetExtension(itm);
            std::string msg = set::GetDictionaryEntry(52) + " ";
            if(ext == "nsp") msg += set::GetDictionaryEntry(53);
            else if(ext == "nro") msg += set::GetDictionaryEntry(54);
            else if(ext == "tik") msg += set::GetDictionaryEntry(55);
            else if(ext == "nxtheme") msg += set::GetDictionaryEntry(56);
            else if(ext == "nca") msg += set::GetDictionaryEntry(57);
            else if(ext == "nacp") msg += set::GetDictionaryEntry(58);
            else if((ext == "jpg") || (ext == "jpeg")) msg += set::GetDictionaryEntry(59);
            else msg += set::GetDictionaryEntry(270);
            msg += "\n\n" + set::GetDictionaryEntry(64) + " " + fs::FormatSize(this->gexp->GetFileSize(fullitm));
            std::vector<pu::String> vopts;
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
                        mainapp->LoadLayout(mainapp->GetInstallLayout());
                        mainapp->GetInstallLayout()->StartInstall(fullitm, this->gexp, dst, this);
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
                            envSetNextLoad(fullitm.c_str(), fullitm.c_str());
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
                            Result rc = es::ImportTicket(btik.data(), btik.size(), es::CertData, 1792);
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
                        if(!this->gexp->IsFile(ntnro))
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(104), set::GetDictionaryEntry(105), { set::GetDictionaryEntry(234) }, false);
                            return;
                        }
                        std::string arg = "installtheme=" + fullitm;
                        size_t index = 0;
                        while(true)
                        {
                            index = arg.find(" ", index);
                            if(index == std::string::npos) break;
                            arg.replace(index, 1, "(_)");
                        }
                        char args[ntnro.size() + arg.size() + 8];
                        sprintf(args, "%s %s", ntnro.c_str(), arg.c_str());
                        envSetNextLoad(ntnro.c_str(), args);
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
                        u8 *rnacp = this->gexp->ReadFile(fullitm).data();
                        NacpStruct *snacp = (NacpStruct*)rnacp;
                        NacpLanguageEntry *lent = NULL;
                        nacpGetLanguageEntry(snacp, &lent);
                        std::string name = set::GetDictionaryEntry(106);
                        std::string author = set::GetDictionaryEntry(107);
                        std::string version = std::string(snacp->version);
                        if(lent != NULL)
                        {
                            name = std::string(lent->name);
                            author = std::string(lent->author);
                        }
                        std::string msg = set::GetDictionaryEntry(108) + "\n\n";
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
                        u128 uid = AskForUser();
                        if(uid == 0) return;
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(121), set::GetDictionaryEntry(122), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0) return;
                        AccountProfile prf;
                        AccountProfileBase pbase;
                        AccountUserData udata;
                        Result rc = accountGetProfile(&prf, uid);
                        rc = accountProfileGet(&prf, &udata, &pbase);
                        acc::ProfileEditor pedit;
                        rc = acc::GetProfileEditor(uid, &pedit);
                        std::vector<u8> vdata = this->gexp->ReadFile(fullitm);
                        pu::ui::render::NativeTexture icon = pu::ui::render::LoadImage(fullitm);
                        if(!icon)
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(121), set::GetDictionaryEntry(259), { set::GetDictionaryEntry(234) }, true);
                            return;
                        }
                        u32 icw = pu::ui::render::GetTextureWidth(icon);
                        u32 ich = pu::ui::render::GetTextureHeight(icon);
                        if((icw != 256) || (ich != 256))
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(121), set::GetDictionaryEntry(260), { set::GetDictionaryEntry(234) }, true);
                            return;
                        }
                        rc = pedit.StoreWithImage(&pbase, &udata, vdata.data(), vdata.size());
                        if(rc == 0) mainapp->ShowNotification(set::GetDictionaryEntry(123));
                        else HandleResult(rc, set::GetDictionaryEntry(124));
                        pedit.Close();
                        serviceClose(&prf.s);
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
                        mainapp->GetFileContentLayout()->LoadFile(fullitm, this, this->gexp, false);
                        break;
                }
            }
            int viewopt = copt - 5;
            int copyopt = copt - 4;
            int delopt = copt - 3;
            int renopt = copt - 2;
            if((sopt == viewopt) && (this->gexp->GetFileSize(fullitm) > 0))
            {
                mainapp->LoadLayout(mainapp->GetFileContentLayout());
                mainapp->GetFileContentLayout()->LoadFile(fullitm, this, this->gexp, true);
            }
            else if(sopt == copyopt) UpdateClipboard(fullitm);
            else if(sopt == delopt)
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
            else if(sopt == renopt)
            {
                std::string kbdt = AskForText(set::GetDictionaryEntry(130), itm);
                if(kbdt != "")
                {
                    if(kbdt == itm) return;
                    std::string newren = kbdt;
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
        std::string itm = this->browseMenu->GetSelectedItem()->GetName().AsUTF8();
        std::string fullitm = this->gexp->FullPathFor(itm);
        std::string pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->IsDirectory(fullitm))
        {
            auto files = this->gexp->GetFiles(fullitm);
            std::vector<std::string> nsps;
            for(u32 i = 0; i < files.size(); i++)
            {
                auto path = fullitm + "/" + files[i];
                if(fs::GetExtension(path) == "nsp") nsps.push_back(path);
            }
            std::vector<pu::String> extraopts = { set::GetDictionaryEntry(281) };
            if(!nsps.empty()) extraopts.push_back(set::GetDictionaryEntry(282));
            extraopts.push_back(set::GetDictionaryEntry(18));
            std::string msg = set::GetDictionaryEntry(134);
            msg += "\n\n" + set::GetDictionaryEntry(237) + " " + fs::FormatSize(this->gexp->GetDirectorySize(fullitm));
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(135), msg, { set::GetDictionaryEntry(136), set::GetDictionaryEntry(73), set::GetDictionaryEntry(74), set::GetDictionaryEntry(75), set::GetDictionaryEntry(280), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            switch(sopt)
            {
                case 0:
                    if(this->gexp->NavigateForward(itm)) this->UpdateElements();
                    break;
                case 1:
                    UpdateClipboard(fullitm);
                    break;
                case 2:
                    if(this->WarnNANDWriteAccess())
                    {
                        this->gexp->DeleteDirectory(fullitm);
                        this->UpdateElements();
                    }
                    break;
                case 3:
                    {
                        std::string kbdt = AskForText(set::GetDictionaryEntry(238), itm);
                        if(kbdt != "")
                        {
                            if(kbdt == itm) return;
                            std::string newren = this->gexp->FullPathFor(kbdt);
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
                case 4:
                    int sopt2 = mainapp->CreateShowDialog(set::GetDictionaryEntry(280), set::GetDictionaryEntry(134), extraopts, true);
                    switch(sopt2)
                    {
                        case 0:
                            this->gexp->SetArchiveBit(fullitm);
                            this->UpdateElements(this->browseMenu->GetSelectedIndex());
                            mainapp->ShowNotification(set::GetDictionaryEntry(303));
                            break;
                        case 1:
                            for(u32 i = 0; i < nsps.size(); i++)
                            {
                                auto nsp = nsps[i];
                                sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(78), { set::GetDictionaryEntry(19), set::GetDictionaryEntry(79), set::GetDictionaryEntry(18) }, true);
                                if(sopt < 0) return;
                                Storage dst = Storage::SdCard;
                                if(sopt == 0) dst = Storage::SdCard;
                                else if(sopt == 1) dst = Storage::NANDUser;
                                u64 fsize = this->gexp->GetFileSize(nsp);
                                u64 rsize = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                                if(rsize < fsize)
                                {
                                    HandleResult(err::Make(err::ErrorDescription::NotEnoughSize), set::GetDictionaryEntry(251));
                                    return;
                                }
                                mainapp->LoadLayout(mainapp->GetInstallLayout());
                                mainapp->GetInstallLayout()->StartInstall(nsp, this->gexp, dst, this);
                            }
                            this->UpdateElements(this->browseMenu->GetSelectedIndex());
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