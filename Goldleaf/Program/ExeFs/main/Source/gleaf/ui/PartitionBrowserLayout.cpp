#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    PartitionBrowserLayout::PartitionBrowserLayout() : pu::Layout()
    {
        this->gexp = fs::GetSdCardExplorer();
        this->browseMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, gsets.BrowserItemSize, (500 / gsets.BrowserItemSize));
        this->browseMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->dirEmptyText = new pu::element::TextBlock(30, 630, set::GetDictionaryEntry(49));
        this->dirEmptyText->SetTextAlign(pu::element::TextAlign::CenterAll);
        this->dirEmptyText->SetColor(gsets.CustomScheme.Text);
        this->AddChild(this->browseMenu);
        this->AddChild(this->dirEmptyText);
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

    void PartitionBrowserLayout::UpdateElements()
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
                bool isdir = this->gexp->IsDirectory(this->gexp->FullPathFor(itm));
                pu::element::MenuItem *mitm = new pu::element::MenuItem(itm);
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
            this->browseMenu->SetSelectedIndex(0);
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
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        std::string fullitm = this->gexp->FullPathFor(itm);
        std::string pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->NavigateForward(itm)) this->UpdateElements();
        else if(this->gexp->IsFile(fullitm))
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
            std::vector<std::string> vopts;
            u32 copt = 5;
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
            else if(!this->gexp->IsFileBinary(fullitm))
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
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), set::GetDictionaryEntry(80), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0) return;
                        bool ignorev = (sopt == 0);
                        u64 fsize = this->gexp->GetFileSize(fullitm);
                        u64 rsize = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                        if(rsize < fsize)
                        {
                            HandleResult(err::Make(err::ErrorDescription::NotEnoughSize), set::GetDictionaryEntry(251));
                            return;
                        }
                        std::string nspipt = "@Sdcard:" + pfullitm.substr(7);
                        nsp::Installer *inst = new nsp::Installer(dst, nspipt, ignorev);
                        Result rc = inst->GetLatestResult();
                        if(rc != 0)
                        {
                            HandleResult(rc, set::GetDictionaryEntry(251));
                            delete inst;
                            return;
                        }
                        bool hascontrol = inst->HasContent(ncm::ContentType::Control);
                        std::string info = set::GetDictionaryEntry(82) + "\n\n";
                        switch(inst->GetContentType())
                        {
                            case ncm::ContentMetaType::Application:
                                info += set::GetDictionaryEntry(83);
                                break;
                            case ncm::ContentMetaType::Patch:
                                info += set::GetDictionaryEntry(84);
                                break;
                            case ncm::ContentMetaType::AddOnContent:
                                info += set::GetDictionaryEntry(85);
                                break;
                            default:
                                info += set::GetDictionaryEntry(86);
                                break;
                        }
                        info += "\n";
                        horizon::ApplicationIdMask idmask = horizon::IsValidApplicationId(inst->GetApplicationId());
                        switch(idmask)
                        {
                            case horizon::ApplicationIdMask::Official:
                                info += set::GetDictionaryEntry(87);
                                break;
                            case horizon::ApplicationIdMask::Homebrew:
                                info += set::GetDictionaryEntry(88);
                                break;
                            case horizon::ApplicationIdMask::Invalid:
                                info += set::GetDictionaryEntry(89);
                                break;
                        }
                        info += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(inst->GetApplicationId());
                        info += "\n\n";
                        if(hascontrol)
                        {
                            NacpStruct *nacp = inst->GetNACP();
                            if(nacp != NULL)
                            {
                                NacpLanguageEntry lent;
                                for(u32 i = 0; i < 16; i++)
                                {
                                    lent = nacp->lang[i];
                                    if((strlen(lent.name) != 0) && (strlen(lent.author) != 0)) break;
                                }
                                info += set::GetDictionaryEntry(91) + " ";
                                info += lent.name;
                                info += "\n" + set::GetDictionaryEntry(92) + " ";
                                info += lent.author;
                                info += "\n" + set::GetDictionaryEntry(109) + " ";
                                info += nacp->version;
                                info += "\n\n" + set::GetDictionaryEntry(93) + " ";
                            }
                        }
                        std::vector<ncm::ContentRecord> ncas = inst->GetRecords();
                        for(u32 i = 0; i < ncas.size(); i++)
                        {
                            ncm::ContentType t = ncas[i].Type;
                            switch(t)
                            {
                                case ncm::ContentType::Control:
                                    info += set::GetDictionaryEntry(166);
                                    break;
                                case ncm::ContentType::Data:
                                    info += set::GetDictionaryEntry(165);
                                    break;
                                case ncm::ContentType::LegalInformation:
                                    info += set::GetDictionaryEntry(168);
                                    break;
                                case ncm::ContentType::Meta:
                                    info += set::GetDictionaryEntry(163);
                                    break;
                                case ncm::ContentType::OfflineHTML:
                                    info += set::GetDictionaryEntry(167);
                                    break;
                                case ncm::ContentType::Program:
                                    info += set::GetDictionaryEntry(164);
                                    break;
                                default:
                                    break;
                            }
                            if(i != (ncas.size() - 1)) info += ", ";
                        }
                        if(inst->HasTicket())
                        {
                            info += "\n\n" + set::GetDictionaryEntry(94) + "\n\n";
                            horizon::TicketData tik = inst->GetTicketData();
                            info += set::GetDictionaryEntry(235) + " " + tik.TitleKey;
                            info += "\n" + set::GetDictionaryEntry(236) + " ";
                            switch(tik.Signature)
                            {
                                case horizon::TicketSignature::RSA_4096_SHA1:
                                    info += "RSA 4096 (SHA1)";
                                    break;
                                case horizon::TicketSignature::RSA_2048_SHA1:
                                    info += "RSA 2048 (SHA1)";
                                    break;
                                case horizon::TicketSignature::ECDSA_SHA1:
                                    info += "ECDSA (SHA256)";
                                    break;
                                case horizon::TicketSignature::RSA_4096_SHA256:
                                    info += "RSA 4096 (SHA256)";
                                    break;
                                case horizon::TicketSignature::RSA_2048_SHA256:
                                    info += "RSA 2048 (SHA256)";
                                    break;
                                case horizon::TicketSignature::ECDSA_SHA256:
                                    info += "ECDSA (SHA256)";
                                    break;
                            }
                            info += "\n" + set::GetDictionaryEntry(95) + " " + std::to_string(tik.KeyGeneration + 1) + " ";
                            switch(tik.KeyGeneration)
                            {
                                case 0:
                                    info += "(1.0.0 - 2.3.0)";
                                    break;
                                case 1:
                                    info += "(3.0.0)";
                                    break;
                                case 2:
                                    info += "(3.0.1 - 3.0.2)";
                                    break;
                                case 3:
                                    info += "(4.0.0 - 4.1.0)";
                                    break;
                                case 4:
                                    info += "(5.0.0 - 5.1.0)";
                                    break;
                                case 5:
                                    info += "(6.0.0 - 6.1.0)";
                                    break;
                                case 6:
                                    info += "(6.2.0)";
                                    break;
                                case 7:
                                    info += "(7.0.0 - 7.1.0)";
                                    break;
                                default:
                                    info += set::GetDictionaryEntry(96);
                                    break;
                            }
                        }
                        else info += "\n\n" + set::GetDictionaryEntry(97);
                        std::string ncicon;
                        if(hascontrol) ncicon = inst->GetExportedIconPath();
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), info, { set::GetDictionaryEntry(65), set::GetDictionaryEntry(18) }, true, ncicon);
                        if(sopt < 0)
                        {
                            delete inst;
                            return;
                        }
                        mainapp->LoadLayout(mainapp->GetInstallLayout());
                        mainapp->GetInstallLayout()->StartInstall(inst, this, false, fullitm, pfullitm);
                        this->UpdateElements();
                        break;
                }
            }
            else if(ext == "nro")
            {
                switch(sopt)
                {
                    case 0:
                        if(IsNRO())
                        {
                            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(98), set::GetDictionaryEntry(99), { set::GetDictionaryEntry(66), set::GetDictionaryEntry(18) }, true);
                            if(sopt < 0) return;
                            envSetNextLoad(fullitm.c_str(), "sdmc:/hbmenu.nro");
                            mainapp->Close();
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
                            std::ifstream ifs(fullitm, std::ios::binary);
                            ifs.seekg(0, ifs.end);
                            int sztik = ifs.tellg();
                            ifs.seekg(0, ifs.beg);
                            auto btik = std::make_unique<u8[]>(sztik);
                            ifs.read((char*)btik.get(), sztik);
                            ifs.close();
                            Result rc = es::ImportTicket(btik.get(), sztik, es::CertData, 1792);
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
                        mainapp->Close();
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
                        auto res = acc::GetProfileEditor(uid);
                        rc = std::get<0>(res);
                        acc::ProfileEditor *pedit = std::get<1>(res);
                        std::vector<u8> vdata = this->gexp->ReadFile(fullitm);
                        pu::render::NativeTexture icon = pu::render::LoadImage(fullitm);
                        if(!icon)
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(121), set::GetDictionaryEntry(259), { set::GetDictionaryEntry(234) }, true);
                            return;
                        }
                        u32 icw = pu::render::GetTextureWidth(icon);
                        u32 ich = pu::render::GetTextureHeight(icon);
                        if((icw != 256) || (ich != 256))
                        {
                            mainapp->CreateShowDialog(set::GetDictionaryEntry(121), set::GetDictionaryEntry(260), { set::GetDictionaryEntry(234) }, true);
                            return;
                        }
                        rc = pedit->StoreWithImage(&pbase, &udata, vdata.data(), vdata.size());
                        if(rc == 0) mainapp->UpdateFooter(set::GetDictionaryEntry(123));
                        else HandleResult(rc, set::GetDictionaryEntry(124));
                        delete pedit;
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
                        horizon::PayloadProcess(fullitm);
                        break;
                }
            }
            else if(!this->gexp->IsFileBinary(fullitm))
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
                    if(rc == 0) mainapp->UpdateFooter(set::GetDictionaryEntry(129));
                    else HandleResult(rc, set::GetDictionaryEntry(253));
                    this->UpdateElements();
                }
            }
            else if(sopt == renopt)
            {
                std::string kbdt = AskForText(set::GetDictionaryEntry(130), itm);
                if(kbdt != "")
                {
                    if(kbdt == itm) return;
                    std::string newren = kbdt;
                    bool isdir = this->gexp->IsDirectory(fullitm);
                    if(this->gexp->IsFile(newren) || this->gexp->IsDirectory(newren)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(254));
                    else if(this->WarnNANDWriteAccess())
                    {
                        int rc = 0;
                        if(isdir) this->gexp->RenameDirectory(fullitm, newren);
                        else this->gexp->RenameFile(fullitm, newren);
                        if(rc) HandleResult(err::MakeErrno(rc), set::GetDictionaryEntry(254));
                        else
                        {
                            mainapp->UpdateFooter(set::GetDictionaryEntry(133));
                            this->UpdateElements();
                        }
                    }
                }
            }
        }
    }

    void PartitionBrowserLayout::fsItems_Click_Y()
    {
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        std::string fullitm = this->gexp->FullPathFor(itm);
        std::string pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->IsDirectory(fullitm))
        {
            std::string msg = set::GetDictionaryEntry(134);
            msg += "\n\n" + set::GetDictionaryEntry(237) + " " + fs::FormatSize(this->gexp->GetDirectorySize(fullitm));
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(135), msg, { set::GetDictionaryEntry(136), set::GetDictionaryEntry(73), set::GetDictionaryEntry(74), set::GetDictionaryEntry(75), set::GetDictionaryEntry(18) }, true);
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
                        /*
                        if(rc != 0)
                        {
                            HandleResult(rc, set::GetDictionaryEntry(253));
                            return;
                        }
                        */
                        this->UpdateElements();
                    }
                    break;
                case 3:
                    std::string kbdt = AskForText(set::GetDictionaryEntry(238), itm);
                    if(kbdt != "")
                    {
                        if(kbdt == itm) return;
                        std::string newren = this->gexp->FullPathFor(kbdt);
                        if(this->gexp->IsFile(newren) || this->gexp->IsDirectory(newren)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(254));
                        else if(this->WarnNANDWriteAccess())
                        {
                            int rc = rename(fullitm.c_str(), newren.c_str());
                            if(rc) HandleResult(rc, set::GetDictionaryEntry(254));
                            else mainapp->UpdateFooter(set::GetDictionaryEntry(139));
                            this->UpdateElements();
                        }
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