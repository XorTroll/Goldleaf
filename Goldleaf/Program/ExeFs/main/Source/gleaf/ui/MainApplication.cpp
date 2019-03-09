#include <gleaf/ui/MainApplication.hpp>
#include <gleaf/ui/Utils.hpp>
#include <threads.h>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    MainApplication *mainapp;
    extern std::string clipboard;
    ApplicationHolder launchapp;

    MainMenuLayout::MainMenuLayout() : pu::Layout()
    {
        this->optionMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->optionMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->optionMenu->SetOnSelectionChanged(std::bind(&MainMenuLayout::optionMenu_SelectionChanged, this));
        this->sdcardMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(0));
        this->sdcardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdcardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdcardMenuItem->AddOnClick(std::bind(&MainMenuLayout::sdcardMenuItem_Click, this));
        this->nandMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(1));
        this->nandMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandMenuItem->AddOnClick(std::bind(&MainMenuLayout::nandMenuItem_Click, this));
        this->usbMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(2));
        this->usbMenuItem->SetIcon(gsets.PathForResource("/Common/USB.png"));
        this->usbMenuItem->SetColor(gsets.CustomScheme.Text);
        this->usbMenuItem->AddOnClick(std::bind(&MainMenuLayout::usbMenuItem_Click, this));
        this->titleMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(3));
        this->titleMenuItem->SetIcon(gsets.PathForResource("/Common/Storage.png"));
        this->titleMenuItem->SetColor(gsets.CustomScheme.Text);
        this->titleMenuItem->AddOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->ticketMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(4));
        this->ticketMenuItem->SetIcon(gsets.PathForResource("/Common/Ticket.png"));
        this->ticketMenuItem->SetColor(gsets.CustomScheme.Text);
        this->ticketMenuItem->AddOnClick(std::bind(&MainMenuLayout::ticketMenuItem_Click, this));
        this->webMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(5));
        this->webMenuItem->SetIcon(gsets.PathForResource("/Common/Browser.png"));
        this->webMenuItem->SetColor(gsets.CustomScheme.Text);
        this->webMenuItem->AddOnClick(std::bind(&MainMenuLayout::webMenuItem_Click, this));
        this->accountMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(6));
        this->accountMenuItem->SetIcon(gsets.PathForResource("/Common/Accounts.png"));
        this->accountMenuItem->SetColor(gsets.CustomScheme.Text);
        this->accountMenuItem->AddOnClick(std::bind(&MainMenuLayout::accountMenuItem_Click, this));
        this->sysinfoMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(7));
        this->sysinfoMenuItem->SetIcon(gsets.PathForResource("/Common/Settings.png"));
        this->sysinfoMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sysinfoMenuItem->AddOnClick(std::bind(&MainMenuLayout::sysinfoMenuItem_Click, this));
        this->aboutMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(8));
        this->aboutMenuItem->SetIcon(gsets.PathForResource("/Common/Info.png"));
        this->aboutMenuItem->SetColor(gsets.CustomScheme.Text);
        this->aboutMenuItem->AddOnClick(std::bind(&MainMenuLayout::aboutMenuItem_Click, this));
        this->optionMenu->AddItem(this->sdcardMenuItem);
        this->optionMenu->AddItem(this->nandMenuItem);
        this->optionMenu->AddItem(this->usbMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->ticketMenuItem);
        this->optionMenu->AddItem(this->webMenuItem);
        this->optionMenu->AddItem(this->accountMenuItem);
        this->optionMenu->AddItem(this->sysinfoMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->AddChild(this->optionMenu);
    }

    void MainMenuLayout::optionMenu_SelectionChanged()
    {
        std::string info;
        pu::element::MenuItem *isel = this->optionMenu->GetSelectedItem();
        if(isel == this->sdcardMenuItem) info = set::GetDictionaryEntry(9);
        else if(isel == this->nandMenuItem) info = set::GetDictionaryEntry(10);
        else if(isel == this->usbMenuItem) info = set::GetDictionaryEntry(11);
        else if(isel == this->titleMenuItem) info = set::GetDictionaryEntry(12);
        else if(isel == this->ticketMenuItem) info = set::GetDictionaryEntry(13);
        else if(isel == this->webMenuItem) info = set::GetDictionaryEntry(14);
        else if(isel == this->accountMenuItem) info = set::GetDictionaryEntry(15);
        else if(isel == this->sysinfoMenuItem) info = set::GetDictionaryEntry(16);
        else if(isel == this->aboutMenuItem) info = set::GetDictionaryEntry(17);
        mainapp->UpdateFooter(info);
    }

    void MainMenuLayout::sdcardMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(19), "SdCard", mainapp->GetSDBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->GetSDBrowserLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
    }

    void MainMenuLayout::nandMenuItem_Click()
    {
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(20), set::GetDictionaryEntry(21) + "\n\n" + set::GetDictionaryEntry(22) + "\n" + set::GetDictionaryEntry(23) + "\n" + set::GetDictionaryEntry(24) + "\n" + set::GetDictionaryEntry(25), { set::GetDictionaryEntry(26), set::GetDictionaryEntry(27), set::GetDictionaryEntry(28), set::GetDictionaryEntry(29), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        if(sopt == 0) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::PRODINFOF);
        else if(sopt == 1) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::NANDSafe);
        else if(sopt == 2) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::NANDUser);
        else if(sopt == 3) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::NANDSystem);
        mainapp->LoadMenuData(set::GetDictionaryEntry(20), "NAND", mainapp->GetNANDBrowserLayout()->GetExplorer()->GetPresentableCwd());
        mainapp->LoadLayout(mainapp->GetNANDBrowserLayout());
    }

    void MainMenuLayout::usbMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(30), "USB", set::GetDictionaryEntry(31));
        mainapp->LoadLayout(mainapp->GetUSBInstallLayout());
        mainapp->GetUSBInstallLayout()->StartUSBConnection();
    }

    void MainMenuLayout::titleMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(32), "Storage", set::GetDictionaryEntry(33));
        EnsureDirectories();
        mainapp->LoadLayout(mainapp->GetContentManagerLayout());
    }

    void MainMenuLayout::ticketMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(34), "Ticket", set::GetDictionaryEntry(35));
        mainapp->GetTicketManagerLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetTicketManagerLayout());
    }

    void MainMenuLayout::webMenuItem_Click()
    {
        if(!IsInstalledTitle()) mainapp->CreateShowDialog(set::GetDictionaryEntry(36), set::GetDictionaryEntry(37), { set::GetDictionaryEntry(234) }, false);
        std::string out = AskForText(set::GetDictionaryEntry(38), "https://");
        if(out == "") return;
        else
        {
            bool nothttp = (out.substr(0, 6) != "http:/");
            bool nothttps = (out.substr(0, 7) != "https:/");
            if(nothttp && nothttps)
            {
                mainapp->CreateShowDialog(set::GetDictionaryEntry(36), set::GetDictionaryEntry(39), { set::GetDictionaryEntry(234) }, false);
                return;
            }
        }
        if(IsInstalledTitle())
        {
            /*
            WebPageConfig web;
            webPageCreate(&web, out.c_str());
            WebCommonReturnValue vret;
            webPageShow(&web, &vret);
            */
        }
        else
        {
            WebWifiConfig wwf;
            webWifiCreate(&wwf, "http://ctest.cdn.nintendo.net/", out.c_str(), 0, 0);
            WebWifiReturnValue vret;
            Result rc = webWifiShow(&wwf, &vret);
            if(rc != 0) HandleResult(rc, set::GetDictionaryEntry(40));
        }
    }

    void MainMenuLayout::accountMenuItem_Click()
    {
        u128 uid = AskForUser();
        if(uid == 0) return;
        mainapp->LoadMenuData(set::GetDictionaryEntry(41), "Accounts", set::GetDictionaryEntry(42));
        mainapp->GetAccountLayout()->Load(uid);
        mainapp->LoadLayout(mainapp->GetAccountLayout());
    }

    void MainMenuLayout::sysinfoMenuItem_Click()
    {
        mainapp->LoadMenuData(set::GetDictionaryEntry(43), "Settings", set::GetDictionaryEntry(44));
        mainapp->GetSystemInfoLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSystemInfoLayout());
    }

    void MainMenuLayout::aboutMenuItem_Click()
    {
        std::string rmode = set::GetDictionaryEntry(45);
        if(IsNRO()) rmode = set::GetDictionaryEntry(46);
        else if(IsInstalledTitle()) rmode = set::GetDictionaryEntry(47);
        else if(IsQlaunch()) rmode = set::GetDictionaryEntry(78);
        mainapp->LoadMenuData("Goldleaf v0.5", "Info", rmode);
        mainapp->LoadLayout(mainapp->GetAboutLayout());
    }

    PartitionBrowserLayout::PartitionBrowserLayout(fs::Partition Partition) : pu::Layout()
    {
        this->ChangePartition(Partition, false);
        this->browseMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, gsets.BrowserItemSize, (500 / gsets.BrowserItemSize));
        this->browseMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->dirEmptyText = new pu::element::TextBlock(30, 630, set::GetDictionaryEntry(49));
        this->dirEmptyText->SetTextAlign(pu::element::TextAlign::CenterAll);
        this->dirEmptyText->SetColor(gsets.CustomScheme.Text);
        this->AddChild(this->browseMenu);
        this->AddChild(this->dirEmptyText);
    }

    void PartitionBrowserLayout::ChangePartition(fs::Partition Partition, bool Update)
    {
        switch(Partition)
        {
            case fs::Partition::SdCard:
                this->gexp = fs::GetSdCardExplorer();
                break;
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
        }
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
                bool isdir = fs::IsDirectory(this->gexp->FullPathFor(itm));
                bool bin = fs::IsFileBinary(this->gexp->FullPathFor(itm));
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
                    else
                    {
                        if(bin) mitm->SetIcon(gsets.PathForResource("/FileSystem/File.png"));
                        else mitm->SetIcon(gsets.PathForResource("/FileSystem/Text.png"));
                    }
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
        if(this->gexp->GetPartition() == fs::Partition::SdCard) return true;
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(50), set::GetDictionaryEntry(51), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        return (sopt == 0);
    }

    void PartitionBrowserLayout::fsItems_Click()
    {
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        std::string fullitm = this->gexp->FullPathFor(itm);
        std::string pfullitm = this->gexp->FullPresentablePathFor(itm);
        if(this->gexp->NavigateForward(itm)) this->UpdateElements();
        else if(fs::IsFile(fullitm))
        {
            bool bin = fs::IsFileBinary(fullitm);
            std::string ext = fs::GetExtension(itm);
            std::string msg = set::GetDictionaryEntry(52) + " ";
            if(ext == "nsp") msg += set::GetDictionaryEntry(53);
            else if(ext == "nro") msg += set::GetDictionaryEntry(54);
            else if(ext == "tik") msg += set::GetDictionaryEntry(55);
            else if(ext == "nxtheme") msg += set::GetDictionaryEntry(56);
            else if(ext == "nca") msg += set::GetDictionaryEntry(57);
            else if(ext == "nacp") msg += set::GetDictionaryEntry(58);
            else if((ext == "jpg") || (ext == "jpeg")) msg += set::GetDictionaryEntry(59);
            else msg += std::string(bin ? set::GetDictionaryEntry(60) : set::GetDictionaryEntry(61)) + " " + set::GetDictionaryEntry(62);
            msg += "\n\n" + set::GetDictionaryEntry(64) + " " + fs::FormatSize(fs::GetFileSize(fullitm));
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
            else if(ext == "nca")
            {
                vopts.push_back(set::GetDictionaryEntry(68));
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
            else if(!bin)
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
                        u64 fsize = fs::GetFileSize(fullitm);
                        u64 rsize = fs::GetFreeSpaceForPartition(static_cast<fs::Partition>(dst));
                        if(rsize < (fsize * 2))
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
                            return;
                        }
                        bool isapp = (inst->GetContentType() == ncm::ContentMetaType::Application);
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
                        if(!fs::IsFile(ntnro))
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
            else if(ext == "nca")
            {
                switch(sopt)
                {
                    case 0:
                        /*
                        if(!HasKeyFile())
                        {
                            mainapp->CreateShowDialog("NCA extraction error", "External keys are required to extract the selected NCA archive.\nPlace them at \'switch\'.\nSupported names: keys.dat, keys.ini, keys.txt, prod.keys");
                            dlg->AddOption(set::GetDictionaryEntry(234));
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        dlg = new pu::Dialog("Extract ExeFs?", "Would you like to extract the ExeFs partition of the selected NCA?\n(it will be extracted to \'" + fullitm + ".ExeFs\' directory)\n\n(in case it isn't found, it won't be extracted)");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption(set::GetDictionaryEntry(18));
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2)) return;
                        bool xexefs = false;
                        if(sopt == 0) xexefs = true;
                        dlg = new pu::Dialog("Extract RomFs?", "Would you like to extract the RomFs partition of the selected NCA?\n(it will be extracted to \'" + fullitm + ".RomFs\' directory)\n\n(in case it isn't found, it won't be extracted)");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption(set::GetDictionaryEntry(18));
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2)) return;
                        bool xromfs = false;
                        if(sopt == 0) xromfs = true;
                        dlg = new pu::Dialog("Extract section 0?", "Would you like to extract the section no. 0 of the selected NCA?\nThis section could be present in CNMT NCAs or in program NCAs.\n(it will be extracted to \'" + fullitm + ".Section0\' directory)\n\n(in case it isn't found, it won't be extracted)");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption(set::GetDictionaryEntry(18));
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2)) return;
                        bool xlogo = false;
                        if(sopt == 0) xlogo = true;
                        gleaf::hactool::Extraction ext;
                        ext.DoExeFs = xexefs;
                        ext.DoRomFs = xromfs;
                        ext.DoLogo = xlogo;
                        if(xexefs) ext.ExeFs = fullitm + ".ExeFs";
                        if(xromfs) ext.RomFs = fullitm + ".RomFs";
                        if(xlogo) ext.Logo = fullitm + ".Section0";
                        bool ok = gleaf::hactool::Process(fullitm, ext, gleaf::hactool::ExtractionFormat::NCA, GetKeyFilePath()).Ok;
                        std::string msg = "The content extraction failed.\nAre you sure the NCA is valid (and that it doesn't require a titlekey) or that you have all the necessary keys?";
                        if(ok) msg = "The NCA extraction succeeded.\nAll the selected partitions were extracted (if they existed within the NCA)";
                        mainapp->UpdateFooter(msg);
                        if(ok) this->UpdateElements();
                        */
                        break;
                }
            }
            else if(ext == "nacp") 
            {
                switch(sopt)
                {
                    case 0:
                        u8 *rnacp = fs::ReadFile(fullitm).data();
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
                        std::vector<u8> vdata = fs::ReadFile(fullitm);
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
            else if(!bin)
            {
                switch(sopt)
                {
                    case 0:
                        mainapp->LoadLayout(mainapp->GetFileContentLayout());
                        mainapp->GetFileContentLayout()->LoadFile(fullitm, this, false);
                        break;
                }
            }
            int viewopt = copt - 5;
            int copyopt = copt - 4;
            int delopt = copt - 3;
            int renopt = copt - 2;
            if((sopt == viewopt) && (fs::GetFileSize(fullitm) > 0))
            {
                mainapp->LoadLayout(mainapp->GetFileContentLayout());
                mainapp->GetFileContentLayout()->LoadFile(fullitm, this, true);
            }
            else if(sopt == copyopt) UpdateClipboard(fullitm);
            else if(sopt == delopt)
            {
                if(this->WarnNANDWriteAccess())
                {
                    sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(127), set::GetDictionaryEntry(128), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                    if(sopt < 0) return;
                    Result rc = fs::DeleteFile(fullitm);
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
                    std::string newren = this->gexp->FullPathFor(kbdt);
                    if(fs::IsFile(newren) || fs::IsDirectory(newren)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(254));
                    else if(this->WarnNANDWriteAccess())
                    {
                        int rc = rename(fullitm.c_str(), newren.c_str());
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
        if(fs::IsDirectory(fullitm))
        {
            std::string msg = set::GetDictionaryEntry(134);
            msg += "\n\n" + set::GetDictionaryEntry(237) + " " + fs::FormatSize(fs::GetDirectorySize(fullitm));
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
                        Result rc = fs::DeleteDirectory(fullitm);
                        if(rc != 0)
                        {
                            HandleResult(rc, set::GetDictionaryEntry(253));
                            return;
                        }
                        this->UpdateElements();
                    }
                    break;
                case 3:
                    std::string kbdt = AskForText(set::GetDictionaryEntry(238), itm);
                    if(kbdt != "")
                    {
                        if(kbdt == itm) return;
                        std::string newren = this->gexp->FullPathFor(kbdt);
                        if(fs::IsFile(newren) || fs::IsDirectory(newren)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(254));
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

    FileContentLayout::FileContentLayout()
    {
        this->cntText = new pu::element::TextBlock(40, 180, "");
        this->cntText->SetColor(gsets.CustomScheme.Text);
        this->cntText->SetFont(pu::render::LoadFont(gsets.PathForResource("/FileSystem/FileDataFont.ttf"), 25));
        this->AddChild(this->cntText);
        this->loffset = 0;
    }

    void FileContentLayout::LoadFile(std::string Path, pu::Layout *Prev, bool Hex)
    {
        this->prev = Prev;
        this->pth = Path;
        this->mode = Hex;
        this->loffset = 0;
        this->Update();
    }

    void FileContentLayout::Update()
    {
        std::vector<std::string> lines;
        if(this->mode) lines = fs::ReadFileFormatHex(this->pth, this->loffset, 19);
        else lines = fs::ReadFileLines(this->pth, this->loffset, 19);
        if(lines.empty())
        {
            this->loffset--;
            return;
        }
        this->rlines = lines.size();
        std::string alines;
        if(this->rlines > 0) for(u32 i = 0; i < this->rlines; i++)
        {
            if(i > 0) alines += "\n";
            alines += lines[i];
        }
        this->cntText->SetText(alines);
        lines.clear();
    }

    void FileContentLayout::ScrollUp()
    {
        if(this->loffset > 0)
        {
            this->loffset--;
            this->Update();
        }
    }

    void FileContentLayout::ScrollDown()
    {
        this->loffset++;
        this->Update();
    }

    pu::Layout *FileContentLayout::GetPreviousLayout()
    {
        return this->prev;
    }

    CopyLayout::CopyLayout()
    {
        this->infoText = new pu::element::TextBlock(400, 300, set::GetDictionaryEntry(140));
        this->infoText->SetColor(gsets.CustomScheme.Text);
        this->copyBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->AddChild(this->infoText);
        this->AddChild(this->copyBar);
    }

    void CopyLayout::StartCopy(std::string Path, std::string NewPath, bool Directory, pu::Layout *Prev)
    {
        if(Directory)
        {
            fs::CopyDirectoryProgress(Path, NewPath, [&](u8 p)
            {
                this->copyBar->SetProgress(p);
                mainapp->CallForRender();
            });
            mainapp->UpdateFooter(set::GetDictionaryEntry(141));
        }
        else
        {
            if(fs::IsFile(NewPath))
            {
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(142), set::GetDictionaryEntry(143), { set::GetDictionaryEntry(239), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0)
                {
                    mainapp->LoadLayout(Prev);
                    return;
                }
            }
            fs::CopyFileProgress(Path, NewPath, [&](u8 p)
            {
                this->copyBar->SetProgress(p);
                mainapp->CallForRender();
            });
            mainapp->UpdateFooter(set::GetDictionaryEntry(240));
        }
        mainapp->LoadLayout(Prev);
    }

    InstallLayout::InstallLayout() : pu::Layout()
    {
        this->installText = new pu::element::TextBlock(150, 300, set::GetDictionaryEntry(144));
        this->installText->SetColor(gsets.CustomScheme.Text);
        this->installBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void InstallLayout::StartInstall(nsp::Installer *Inst, pu::Layout *Prev, bool Delete, std::string Input, std::string PInput)
    {
        if(IsInstalledTitle()) appletBeginBlockingHomeButton(0);
        mainapp->LoadMenuHead(set::GetDictionaryEntry(145) + " \'" + PInput + "\'");
        this->installText->SetText(set::GetDictionaryEntry(146));
        mainapp->CallForRender();
        Result rc = Inst->ProcessRecords();
        if(rc != 0)
        {
            int sopt = -1;
            if(rc == err::Make(err::ErrorDescription::TitleAlreadyInstalled))
            {
                sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(77), "The NSP's content seems to be already installed.\n(might be an older version, like with updates)\nWould you like to install it over the actual installed one?", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112) }, true);
                if(sopt == 0)
                {
                    horizon::Title t = horizon::Locate(Inst->GetApplicationId());
                    Result rc = horizon::RemoveTitle(t);
                    if(rc != 0)
                    {
                        HandleResult(rc, set::GetDictionaryEntry(251));
                        sopt = -1;
                    }
                }
            }
            if(sopt != 0)
            {
                if(IsInstalledTitle()) appletEndBlockingHomeButton();
                delete Inst;
                Inst = NULL;
                HandleResult(rc, set::GetDictionaryEntry(251));
                mainapp->LoadLayout(Prev);
                return;
            }
        }
        this->installText->SetText(set::GetDictionaryEntry(147));
        mainapp->CallForRender();
        rc = Inst->WriteContents([&](ncm::ContentRecord NCA, u32 Index, u32 Count, int Percentage)
        {
            std::string name = set::GetDictionaryEntry(148) + " \'"  + horizon::GetStringFromNCAId(NCA.NCAId);
            if(NCA.Type == ncm::ContentType::Meta) name += ".cnmt";
            name += ".nca\'... (NCA " + std::to_string(Index + 1) + " " + set::GetDictionaryEntry(149) + " " + std::to_string(Count) + ")";
            this->installText->SetText(name);
            this->installBar->SetProgress((u8)Percentage);
            mainapp->CallForRender();
        });
        if(rc != 0)
        {
            if(IsInstalledTitle()) appletEndBlockingHomeButton();
            delete Inst;
            Inst = NULL;
            HandleResult(rc, set::GetDictionaryEntry(251));
            mainapp->LoadLayout(Prev);
            return;
        }
        if(IsInstalledTitle()) appletEndBlockingHomeButton();
        delete Inst;
        Inst = NULL;
        if(rc == 0) mainapp->UpdateFooter(set::GetDictionaryEntry(150));
        else HandleResult(rc, set::GetDictionaryEntry(251));
        mainapp->LoadLayout(Prev);
    }

    USBInstallLayout::USBInstallLayout() : pu::Layout()
    {
        this->installText = new pu::element::TextBlock(150, 300, set::GetDictionaryEntry(151));
        this->installText->SetTextAlign(pu::element::TextAlign::CenterHorizontal);
        this->installText->SetColor(gsets.CustomScheme.Text);
        this->installBar = new pu::element::ProgressBar(380, 345, 600, 50);
        this->installBar->SetVisible(false);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void USBUpdate()
    {
        mainapp->CallForRender();
        if(usb::IsStateNotReady())
        {
            mainapp->CreateShowDialog("USB", "USB FAIL", { "OOF" }, true);
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
    }

    void USBInstallLayout::StartUSBConnection()
    {
        this->installText->SetText(set::GetDictionaryEntry(152) + " " + set::GetDictionaryEntry(153));
        mainapp->CallForRender();
        while(usb::IsStateNotReady())
        {
            hidScanInput();
            if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B)
            {
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                return;
            }
            mainapp->CallForRender();
        }
        this->installText->SetText(set::GetDictionaryEntry(155));
        mainapp->CallForRender();
        usb::Command req = usb::ReadCommand(USBUpdate);
        usb::Command fcmd = usb::MakeCommand(usb::CommandId::Finish);
        if(usb::CommandMagicOk(req))
        {
            if(usb::IsCommandId(req, usb::CommandId::ConnectionRequest))
            {
                this->installText->SetText(set::GetDictionaryEntry(241));
                mainapp->CallForRender();
                usb::Command cmd1 = usb::MakeCommand(usb::CommandId::ConnectionResponse);
                usb::WriteCommand(cmd1);
                req = usb::ReadCommand(USBUpdate);
                if(usb::CommandMagicOk(req))
                {
                    if(usb::IsCommandId(req, usb::CommandId::NSPName))
                    {
                        u32 nspnamesize = usb::Read32();
                        std::string nspname = usb::ReadString(nspnamesize);
                        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(157) + " \'" + nspname + "\'\n" + set::GetDictionaryEntry(158), { set::GetDictionaryEntry(65), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0)
                        {
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        mainapp->LoadMenuHead(set::GetDictionaryEntry(145) + " \'" + nspname + "\'");
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(78), { set::GetDictionaryEntry(19), set::GetDictionaryEntry(79), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0)
                        {
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        Storage dst = Storage::SdCard;
                        if(sopt == 1) dst = Storage::NANDUser;
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(156), set::GetDictionaryEntry(80), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(112), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0)
                        {
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        
                        bool ignorev = (sopt == 0);
                        usb::Command cmd2 = usb::MakeCommand(usb::CommandId::Start);
                        usb::WriteCommand(cmd2);
                        if(IsInstalledTitle()) appletBeginBlockingHomeButton(0);
                        this->installText->SetText(set::GetDictionaryEntry(144));
                        mainapp->CallForRender();
                        usb::Installer inst(dst, ignorev);
                        Result rc = inst.GetLatestResult();
                        if(rc != 0)
                        {
                            if(IsInstalledTitle()) appletEndBlockingHomeButton();
                            HandleResult(rc, "Test error text");
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        this->installText->SetText(set::GetDictionaryEntry(146));
                        mainapp->CallForRender();
                        rc = inst.ProcessRecords([&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed)
                        {
                            if(usb::IsStateNotReady())
                            {
                                mainapp->CreateShowDialog("USB", "USB FAIL", { "OOF" }, true);
                                mainapp->UnloadMenuData();
                                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                                return;
                            }
                            std::string name = set::GetDictionaryEntry(148) + " \'"  + Name + "\'... (" + std::to_string(Index + 1) + " " + set::GetDictionaryEntry(149) + " " + std::to_string(Count) + ")";
                            this->installText->SetText(name);
                            this->installBar->SetProgress((u8)Percentage);
                            mainapp->UpdateFooter(set::GetDictionaryEntry(159) + " " + horizon::DoubleToString(Speed) + " MB/s");
                            mainapp->CallForRender();
                        });
                        if(rc != 0)
                        {
                            if(IsInstalledTitle()) appletEndBlockingHomeButton();
                            HandleResult(rc, "Test error text");
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        this->installText->SetText(set::GetDictionaryEntry(147));
                        mainapp->CallForRender();
                        this->installBar->SetVisible(true);
                        rc = inst.ProcessContents([&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed)
                        {
                            if(usb::IsStateNotReady())
                            {
                                mainapp->CreateShowDialog("USB", "USB FAIL", { "OOF" }, true);
                                mainapp->UnloadMenuData();
                                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                                return;
                            }
                            std::string name = set::GetDictionaryEntry(148) + " \'"  + Name + "\'... (" + std::to_string(Index + 1) + " " + set::GetDictionaryEntry(149) + " " + std::to_string(Count) + ")";
                            this->installText->SetText(name);
                            this->installBar->SetProgress((u8)Percentage);
                            mainapp->UpdateFooter(set::GetDictionaryEntry(159) + " " + horizon::DoubleToString(Speed) + " MB/s");
                            mainapp->CallForRender();
                        });
                        this->installBar->SetVisible(false);
                        if(IsInstalledTitle()) appletEndBlockingHomeButton();
                        if(rc != 0)
                        {
                            HandleResult(rc, "Test error text");
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        inst.Finalize();
                        mainapp->UpdateFooter(set::GetDictionaryEntry(160));
                        mainapp->CallForRender();
                    }
                    else if(usb::IsCommandId(req, usb::CommandId::Finish)) mainapp->UpdateFooter(set::GetDictionaryEntry(242));
                    else
                    {
                        usb::WriteCommand(fcmd);
                        HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
                    }
                }
                else
                {
                    usb::WriteCommand(fcmd);
                    HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
                }
            }
            else if(usb::IsCommandId(req, usb::CommandId::Finish)) mainapp->UpdateFooter(set::GetDictionaryEntry(242));
            else
            {
                usb::WriteCommand(fcmd);
                HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
            }
        }
        else
        {
            usb::WriteCommand(fcmd);
            HandleResult(err::Make(err::ErrorDescription::BadGLUCCommand), set::GetDictionaryEntry(256));
        }
        mainapp->UnloadMenuData();
        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
    }

    ContentInformationLayout::ContentInformationLayout()
    {
        this->optionsMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->optionsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->AddChild(this->optionsMenu);
    }

    void ContentInformationLayout::UpdateElements()
    {
        this->optionsMenu->ClearItems();
        this->baseTitleItem = new pu::element::MenuItem("Title");
        this->baseTitleItem->SetColor(gsets.CustomScheme.Text);
        this->baseTitleItem->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
        this->optionsMenu->AddItem(this->baseTitleItem);
        for(u32 i = 0; i < this->subcnts.size(); i++)
        {
            horizon::Title scnt = this->subcnts[i];
            pu::element::MenuItem *subcnt = new pu::element::MenuItem(scnt.IsUpdate() ? "Update" : "DLC");
            subcnt->SetColor(gsets.CustomScheme.Text);
            subcnt->AddOnClick(std::bind(&ContentInformationLayout::options_Click, this));
            this->optionsMenu->AddItem(subcnt);
        }
    }

    /*
    void ContentInformationLayout::contentInfo_Click()
    {
        std::string msg = set::GetDictionaryEntry(169) + "\n\n";
        msg += set::GetDictionaryEntry(170) + " ";
        switch(this->content.Type)
        {
            case ncm::ContentMetaType::Application:
                msg += set::GetDictionaryEntry(171);
                break;
            case ncm::ContentMetaType::AddOnContent:
                msg += set::GetDictionaryEntry(172);
                break;
            case ncm::ContentMetaType::Patch:
                msg += set::GetDictionaryEntry(173);
                break;
            case ncm::ContentMetaType::SystemProgram:
                msg += set::GetDictionaryEntry(174);
                break;
            case ncm::ContentMetaType::SystemData:
                msg += set::GetDictionaryEntry(175);
                break;
            default:
                msg += set::GetDictionaryEntry(176);
                break;
        }
        msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(this->content.ApplicationId);
        msg += "\n\n" + set::GetDictionaryEntry(177) + " " + this->contents.GetFormattedTotalSize();
        msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(this->content.Version);
        if(this->content.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(this->content.Version >> 16) + "]";
        if(this->content.Type == ncm::ContentMetaType::Application)
        {
            if(IsQlaunch())
            {
                int sopt = mainapp->CreateShowDialog("Content information", msg, { "Launch", set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
                if(appletAHIsInProcess(&launchapp))
                {
                    sopt = mainapp->CreateShowDialog("Title launch", "There is already another title being executed.\nWould you like to close the current title and launch the selected one?", { "Close and launch", set::GetDictionaryEntry(18) }, true);
                    if(sopt < 0) return;
                    Result rc = appletAHTerminate(&launchapp);
                    if(rc != 0) mainapp->UpdateFooter("Terminated: " + horizon::FormatHex(rc));
                    appletAHClose(&launchapp);
                }
                Result rc = appletAHCreate(&launchapp, this->content.ApplicationId);
                if(rc == 0)
                {
                    rc = appletAHLaunch(&launchapp);
                    if(rc != 0) mainapp->UpdateFooter("Launched: " + horizon::FormatHex(rc));
                    horizon::Thread *th = new horizon::Thread(QProcess);
                    th->Start();
                }
                else mainapp->UpdateFooter("Created: " + horizon::FormatHex(rc));
            }
            else
            {
                if(this->content.Location == Storage::GameCart)
                {
                    mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(234) }, false);
                    return;
                }
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(245), set::GetDictionaryEntry(244), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
                if(sopt == 0)
                {
                    Result rc = horizon::RemoveTitle(this->content);
                    if(rc == 0) mainapp->UpdateFooter(set::GetDictionaryEntry(180));
                    else HandleResult(rc, set::GetDictionaryEntry(247));
                }
                else if(sopt == 1)
                {
                    if(this->contents.GetTotalSize() >= 0x100000000)
                    {
                        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(182), set::GetDictionaryEntry(183), { set::GetDictionaryEntry(234), set::GetDictionaryEntry(18) }, true);
                        if(sopt < 0) return;
                    }
                    sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(182), set::GetDictionaryEntry(184), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                    if(sopt < 0) return;
                    if(sopt == 0)
                    {
                        mainapp->LoadLayout(mainapp->GetTitleDumperLayout());
                        mainapp->GetTitleDumperLayout()->StartDump(this->content);
                        mainapp->UnloadMenuData();
                        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                    }
                }
            }
        }
        else
        {
            int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), msg, { set::GetDictionaryEntry(245), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
            if(sopt == 0)
            {
                if(this->content.Location == Storage::NANDSystem)
                {
                    mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(185), { set::GetDictionaryEntry(234) }, true);
                    return;
                }
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(243), set::GetDictionaryEntry(186), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
                if(sopt < 0) return;
                Result rc = horizon::RemoveTitle(this->content);
                if(rc == 0) mainapp->UpdateFooter(set::GetDictionaryEntry(246));
                else HandleResult(rc, set::GetDictionaryEntry(247));
            }
        }
    }
    */

    void ContentInformationLayout::options_Click()
    {
        u32 idx = this->optionsMenu->GetSelectedIndex();
        std::string msg = set::GetDictionaryEntry(169) + "\n\n";
        msg += set::GetDictionaryEntry(170) + " ";
        if(idx == 0)
        {
            switch(this->content.Type)
            {
                case ncm::ContentMetaType::Application:
                    msg += set::GetDictionaryEntry(171);
                    break;
                case ncm::ContentMetaType::AddOnContent:
                    msg += set::GetDictionaryEntry(172);
                    break;
                case ncm::ContentMetaType::Patch:
                    msg += set::GetDictionaryEntry(173);
                    break;
                case ncm::ContentMetaType::SystemProgram:
                    msg += set::GetDictionaryEntry(174);
                    break;
                case ncm::ContentMetaType::SystemData:
                    msg += set::GetDictionaryEntry(175);
                    break;
                default:
                    msg += set::GetDictionaryEntry(176);
                    break;
            }
            msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(this->content.ApplicationId);
            msg += "\n\n" + set::GetDictionaryEntry(177) + " " + this->contents.GetFormattedTotalSize();
            msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(this->content.Version);
            if(this->content.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(this->content.Version >> 16) + "]";
        }
        else
        {
            horizon::Title subcnt = this->subcnts[idx - 1];
            horizon::TitleContents subcnts = subcnt.GetContents();
            switch(subcnt.Type)
            {
                case ncm::ContentMetaType::Application:
                    msg += set::GetDictionaryEntry(171);
                    break;
                case ncm::ContentMetaType::AddOnContent:
                    msg += set::GetDictionaryEntry(172);
                    break;
                case ncm::ContentMetaType::Patch:
                    msg += set::GetDictionaryEntry(173);
                    break;
                case ncm::ContentMetaType::SystemProgram:
                    msg += set::GetDictionaryEntry(174);
                    break;
                case ncm::ContentMetaType::SystemData:
                    msg += set::GetDictionaryEntry(175);
                    break;
                default:
                    msg += set::GetDictionaryEntry(176);
                    break;
            }
            msg += "\n" + set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(subcnt.ApplicationId);
            msg += "\n\n" + set::GetDictionaryEntry(177) + " " + subcnts.GetFormattedTotalSize();
            msg += "\n\n" + set::GetDictionaryEntry(178) + " v" + std::to_string(subcnt.Version);
            if(subcnt.Version != 0) msg += " [" + set::GetDictionaryEntry(179) + " no. " + std::to_string(subcnt.Version >> 16) + "]";
        }
        mainapp->CreateShowDialog("TEST", msg, { "Ok" }, true);
    }

    void ContentInformationLayout::LoadContent(horizon::Title Content)
    {
        this->subcnts.clear();
        this->content = Content;
        this->contents = Content.GetContents();
        std::vector<horizon::Title> nusr = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::NANDUser);
        std::vector<horizon::Title> sdc = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::SdCard);
        std::vector<horizon::Title> gcrt = horizon::SearchTitles(ncm::ContentMetaType::Any, Storage::GameCart);
        bool hasupd = false;
        bool hasdlc = false;
        for(u32 i = 0; i < nusr.size(); i++)
        {
            if(Content.CheckBase(nusr[i]))
            {
                if(nusr[i].IsUpdate()) hasupd = true;
                if(nusr[i].IsDLC()) hasdlc = true;
                this->subcnts.push_back(nusr[i]);
            }
        }
        nusr.clear();
        for(u32 i = 0; i < sdc.size(); i++)
        {
            if(Content.CheckBase(sdc[i]))
            {
                if(sdc[i].IsUpdate()) hasupd = true;
                if(sdc[i].IsDLC()) hasdlc = true;
                this->subcnts.push_back(sdc[i]);
            }
        }
        sdc.clear();
        for(u32 i = 0; i < gcrt.size(); i++)
        {
            if(Content.CheckBase(gcrt[i]))
            {
                if(gcrt[i].IsUpdate()) hasupd = true;
                if(gcrt[i].IsDLC()) hasdlc = true;
                this->subcnts.push_back(gcrt[i]);
            }
        }
        gcrt.clear();
        NacpStruct *nacp = Content.TryGetNACP();
        std::string tcnt = horizon::FormatApplicationId(Content.ApplicationId);
        if(nacp != NULL)
        {
            tcnt = horizon::GetNACPName(nacp);
            tcnt += " (" + std::string(nacp->version);
            if(hasupd)
            {
                if(hasdlc) tcnt += ", with update and DLC)";
                else tcnt += ", with update)";
            }
            else
            {
                if(hasdlc) tcnt += ", with DLC)";
                else tcnt += ")";
            }
            free(nacp);
        }
        std::string icon = gsets.PathForResource("/Common/Storage.png");
        if(Content.TryGetIcon() != NULL) icon = horizon::GetExportedIconPath(Content.ApplicationId);
        mainapp->LoadMenuData(set::GetDictionaryEntry(187), icon, tcnt, false);
        this->UpdateElements();
    }


    StorageContentsLayout::StorageContentsLayout()
    {
        this->contentsMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->contentsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->noContentsText = new pu::element::TextBlock(30, 630, set::GetDictionaryEntry(188));
        this->noContentsText->SetColor(gsets.CustomScheme.Text);
        this->noContentsText->SetVisible(false);
        this->AddChild(this->noContentsText);
        this->AddChild(this->contentsMenu);
    }

    void StorageContentsLayout::contents_Click()
    {
        horizon::Title selcnt = this->contents[this->contentsMenu->GetSelectedIndex()];
        if(selcnt.IsBaseTitle() || (selcnt.Location == Storage::NANDSystem))
        {
            mainapp->GetContentInformationLayout()->LoadContent(selcnt);
            mainapp->LoadLayout(mainapp->GetContentInformationLayout());
        }
    }

    void StorageContentsLayout::LoadFromStorage(Storage Location)
    {
        if(!this->contents.empty())
        {
            this->contentsMenu->ClearItems();
            this->contents.clear();
        }
        this->contentsMenu->SetCooldownEnabled(true);
        this->contents = horizon::SearchTitles(ncm::ContentMetaType::Any, Location);
        if(this->contents.empty())
        {
            this->noContentsText->SetVisible(true);
            this->contentsMenu->SetVisible(false);
        }
        else
        {
            this->noContentsText->SetVisible(false);
            this->contentsMenu->SetVisible(true);
            for(u32 i = 0; i < this->contents.size(); i++)
            {
                horizon::Title cnt = this->contents[i];
                if(cnt.IsBaseTitle() || (cnt.Location == Storage::NANDSystem))
                {
                    NacpStruct *nacp = cnt.TryGetNACP();
                    std::string name = horizon::FormatApplicationId(cnt.ApplicationId);
                    if(nacp != NULL) name = horizon::GetNACPName(nacp);
                    pu::element::MenuItem *itm = new pu::element::MenuItem(name);
                    itm->SetColor(gsets.CustomScheme.Text);
                    bool hicon = cnt.DumpControlData();
                    if(hicon) itm->SetIcon(horizon::GetExportedIconPath(cnt.ApplicationId));
                    itm->AddOnClick(std::bind(&StorageContentsLayout::contents_Click, this));
                    this->contentsMenu->AddItem(itm);
                }
            }
            this->contentsMenu->SetSelectedIndex(0);
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(189));
    }

    std::vector<horizon::Title> StorageContentsLayout::GetContents()
    {
        return this->contents;
    }

    ContentManagerLayout::ContentManagerLayout() : pu::Layout()
    {
        this->typesMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->typesMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->sdCardMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(19));
        this->sdCardMenuItem->SetIcon(gsets.PathForResource("/Common/SdCard.png"));
        this->sdCardMenuItem->SetColor(gsets.CustomScheme.Text);
        this->sdCardMenuItem->AddOnClick(std::bind(&ContentManagerLayout::sdCardMenuItem_Click, this));
        this->nandUserMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(28));
        this->nandUserMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandUserMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandUserMenuItem->AddOnClick(std::bind(&ContentManagerLayout::nandUserMenuItem_Click, this));
        this->nandSystemMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(29));
        this->nandSystemMenuItem->SetIcon(gsets.PathForResource("/Common/NAND.png"));
        this->nandSystemMenuItem->SetColor(gsets.CustomScheme.Text);
        this->nandSystemMenuItem->AddOnClick(std::bind(&ContentManagerLayout::nandSystemMenuItem_Click, this));
        this->gameCartMenuItem = new pu::element::MenuItem(set::GetDictionaryEntry(190));
        this->gameCartMenuItem->SetIcon(gsets.PathForResource("/Common/GameCart.png"));
        this->gameCartMenuItem->SetColor(gsets.CustomScheme.Text);
        this->gameCartMenuItem->AddOnClick(std::bind(&ContentManagerLayout::gameCartMenuItem_Click, this));
        this->typesMenu->AddItem(this->sdCardMenuItem);
        this->typesMenu->AddItem(this->nandUserMenuItem);
        this->typesMenu->AddItem(this->nandSystemMenuItem);
        this->typesMenu->AddItem(this->gameCartMenuItem);
        this->AddChild(this->typesMenu);
    }

    void ContentManagerLayout::sdCardMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::SdCard);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandUserMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::NANDUser);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::nandSystemMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::NANDSystem);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    void ContentManagerLayout::gameCartMenuItem_Click()
    {
        mainapp->GetStorageContentsLayout()->LoadFromStorage(Storage::GameCart);
        mainapp->LoadLayout(mainapp->GetStorageContentsLayout());
    }

    TitleDumperLayout::TitleDumperLayout()
    {
        this->dumpText = new pu::element::TextBlock(300, 300, set::GetDictionaryEntry(191));
        this->dumpText->SetColor(gsets.CustomScheme.Text);
        this->ncaBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->ncaBar->SetVisible(false);
        this->AddChild(this->dumpText);
        this->AddChild(this->ncaBar);
    }

    void TitleDumperLayout::StartDump(horizon::Title &Target)
    {
        EnsureDirectories();
        mainapp->CallForRender();
        FsStorageId stid = dump::GetApplicationLocation(Target.ApplicationId);
        if(stid == FsStorageId_NandUser)
        {
            FsFileSystem bisfs;
            Result rc = fsOpenBisFileSystem(&bisfs, 30, "");
            if(rc != 0)
            {
                // err
                mainapp->LoadLayout(mainapp->GetContentManagerLayout());
                return;
            }
            fsdevMountDevice("glduser", bisfs);
        }
        std::string fappid = horizon::FormatApplicationId(Target.ApplicationId);
        std::string outdir = "sdmc:/goldleaf/dump/" + fappid;
        fs::CreateDirectory(outdir);
        this->dumpText->SetText(set::GetDictionaryEntry(192));
        mainapp->CallForRender();
        std::string tkey = dump::GetTitleKeyData(Target.ApplicationId, true);
        this->dumpText->SetText(set::GetDictionaryEntry(193));
        mainapp->CallForRender();
        NcmContentStorage cst;
        Result rc = ncmOpenContentStorage(stid, &cst);
        if(rc != 0)
        {
            // err
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        NcmContentMetaDatabase cmdb;
        rc = ncmOpenContentMetaDatabase(stid, &cmdb);
        if(rc != 0)
        {
            // err
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        NcmMetaRecord mrec;
        bool ok = dump::GetMetaRecord(&cmdb, Target.ApplicationId, &mrec);
        if(!ok)
        {
            // err
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        NcmNcaId program;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Program, &program);
        if(!ok)
        {
            // err
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        std::string sprogram = dump::GetNCAIdPath(&cst, &program);
        NcmNcaId meta;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Meta, &meta);
        if(!ok)
        {
            // err
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        std::string smeta = dump::GetNCAIdPath(&cst, &meta);
        NcmNcaId control;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Control, &control);
        if(!ok)
        {
            // err
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        std::string scontrol = dump::GetNCAIdPath(&cst, &control);
        NcmNcaId linfo;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::LegalInfo, &linfo);
        bool haslinfo = ok;
        std::string slinfo;
        if(ok) slinfo = dump::GetNCAIdPath(&cst, &linfo);
        NcmNcaId hoff;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::OfflineHtml, &hoff);
        bool hashoff = ok;
        std::string shoff;
        if(ok) shoff = dump::GetNCAIdPath(&cst, &hoff);
        std::string xprogram = sprogram;
        std::string xmeta = smeta;
        std::string xcontrol = scontrol;
        std::string xlinfo = slinfo;
        std::string xhoff = shoff;
        if(stid == FsStorageId_SdCard)
        {
            this->dumpText->SetText(set::GetDictionaryEntry(194));
            xprogram = outdir + "/" + horizon::GetStringFromNCAId(program) + ".nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, program, xprogram, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xmeta = outdir + "/" + horizon::GetStringFromNCAId(meta) + ".cnmt.nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, meta, xmeta, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xcontrol = outdir + "/" + horizon::GetStringFromNCAId(control) + ".nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, control, xcontrol, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            if(haslinfo)
            {
                xlinfo = outdir + "/" + horizon::GetStringFromNCAId(linfo) + ".nca";
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, linfo, xlinfo, [&](u8 p)
                {
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(hashoff)
            {
                xhoff = outdir + "/" + horizon::GetStringFromNCAId(hoff) + ".nca";
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, hoff, xhoff, [&](u8 p)
                {
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
        }
        if(stid == FsStorageId_NandUser)
        {
            this->dumpText->SetText(set::GetDictionaryEntry(195));
            xprogram = "glduser:/Contents/" + xprogram.substr(15);
            std::string txprogram = outdir + "/" + horizon::GetStringFromNCAId(program) + ".nca";
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xprogram, txprogram, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xprogram = txprogram;
            xmeta = "glduser:/Contents/" + xmeta.substr(15);
            std::string txmeta = outdir + "/" + horizon::GetStringFromNCAId(meta) + ".cnmt.nca";
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xmeta, txmeta, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xmeta = txmeta;
            xcontrol = "glduser:/Contents/" + xcontrol.substr(15);
            std::string txcontrol = outdir + "/" + horizon::GetStringFromNCAId(control) + ".nca";
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xcontrol, txcontrol, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xcontrol = txcontrol;
            if(haslinfo)
            {
                xlinfo = "glduser:/Contents/" + xlinfo.substr(15);
                std::string txlinfo = outdir + "/" + horizon::GetStringFromNCAId(linfo) + ".nca";
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xlinfo, txlinfo, [&](u8 p)
                {
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xlinfo = txlinfo;
            }
            if(hashoff)
            {
                xhoff = "glduser:/Contents/" + xhoff.substr(15);
                std::string txhoff = outdir + "/" + horizon::GetStringFromNCAId(hoff) + ".nca";
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xhoff, txhoff, [&](u8 p)
                {
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xhoff = txhoff;
            }
            fsdevUnmountDevice("glduser");
        }
        std::string fout = "sdmc:/goldleaf/dump/out/" + fappid + ".nsp";
        this->ncaBar->SetVisible(true);
        this->dumpText->SetText(set::GetDictionaryEntry(196));
        int qi = nsp::BuildPFS(outdir, fout, [&](u8 p)
        {
            this->ncaBar->SetProgress(p);
            mainapp->CallForRender();
        });
        ok = (qi == 0);
        fs::DeleteDirectory("sdmc:/goldleaf/dump/temp");
        fs::DeleteDirectory(outdir);
        /*
        if(ok) mainapp->UpdateFooter(set::GetDictionaryEntry(197) + " '" + fout + "'");
        else mainapp->UpdateFooter(set::GetDictionaryEntry(198));
        */
        serviceClose(&cst.s);
        serviceClose(&cmdb.s);
    }

    TicketManagerLayout::TicketManagerLayout() : pu::Layout()
    {
        this->ticketsMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->ticketsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        this->notTicketsText = new pu::element::TextBlock(450, 400, set::GetDictionaryEntry(199));
        this->notTicketsText->SetColor(gsets.CustomScheme.Text);
        this->AddChild(this->notTicketsText);
        this->AddChild(this->ticketsMenu);
    }

    void TicketManagerLayout::UpdateElements()
    {
        if(!this->tickets.empty()) this->tickets.clear();
        this->tickets = horizon::GetAllTickets();
        mainapp->LoadMenuHead(set::GetDictionaryEntry(248));
        this->ticketsMenu->ClearItems();
        this->ticketsMenu->SetCooldownEnabled(true);
        if(this->tickets.empty())
        {
            this->notTicketsText->SetVisible(true);
            this->ticketsMenu->SetVisible(false);
        }
        else
        {
            this->notTicketsText->SetVisible(false);
            for(u32 i = 0; i < this->tickets.size(); i++)
            {
                horizon::Ticket ticket = this->tickets[i];
                u64 tappid = ticket.GetApplicationId();
                std::string tname = horizon::FormatApplicationId(tappid);
                pu::element::MenuItem *itm = new pu::element::MenuItem(tname);
                itm->SetColor(gsets.CustomScheme.Text);
                itm->SetIcon(gsets.PathForResource("/Common/Ticket.png"));
                itm->AddOnClick(std::bind(&TicketManagerLayout::tickets_Click, this));
                this->ticketsMenu->AddItem(itm);
            }
            this->ticketsMenu->SetSelectedIndex(0);
        }
    }

    void TicketManagerLayout::tickets_Click()
    {
        horizon::Ticket seltick = this->tickets[this->ticketsMenu->GetSelectedIndex()];
        std::string info = set::GetDictionaryEntry(201) + "\n\n\n";
        u64 tappid = seltick.GetApplicationId();
        info += set::GetDictionaryEntry(90) + " " + horizon::FormatApplicationId(tappid);
        info += "\n" + set::GetDictionaryEntry(95) + " " + std::to_string(seltick.GetKeyGeneration() + 1);
        info += "\n\n";
        bool used = horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::SdCard, tappid);
        if(!used) used = horizon::ExistsTitle(ncm::ContentMetaType::Any, Storage::NANDUser, tappid);
        if(used) info += set::GetDictionaryEntry(202);
        else info += set::GetDictionaryEntry(203);
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), info, { set::GetDictionaryEntry(245), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(204), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt < 0) return;
        if(used)
        {
            sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(200), set::GetDictionaryEntry(205), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
            if(sopt < 0) return;
        }
        Result rc = es::DeleteTicket(&seltick.RId, sizeof(es::RightsId));
        if(rc == 0)
        {
            mainapp->UpdateFooter(set::GetDictionaryEntry(206));
            this->UpdateElements();
        }
        else HandleResult(rc, set::GetDictionaryEntry(207));
    }

    AccountLayout::AccountLayout() : pu::Layout()
    {
        this->optsMenu = new pu::element::Menu(0, 170, 1280, gsets.CustomScheme.Base, 100, 5);
        this->optsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        pu::element::MenuItem *itm = new pu::element::MenuItem(set::GetDictionaryEntry(208));
        itm->SetColor(gsets.CustomScheme.Text);
        itm->AddOnClick(std::bind(&AccountLayout::optsRename_Click, this));
        this->optsMenu->AddItem(itm);
        pu::element::MenuItem *itm2 = new pu::element::MenuItem(set::GetDictionaryEntry(209));
        itm2->SetColor(gsets.CustomScheme.Text);
        itm2->AddOnClick(std::bind(&AccountLayout::optsIcon_Click, this));
        this->optsMenu->AddItem(itm2);
        pu::element::MenuItem *itm3 = new pu::element::MenuItem(set::GetDictionaryEntry(210));
        itm3->SetColor(gsets.CustomScheme.Text);
        itm3->AddOnClick(std::bind(&AccountLayout::optsDelete_Click, this));
        this->optsMenu->AddItem(itm3);
        this->AddChild(this->optsMenu);
    }

    void AccountLayout::Load(u128 UserId)
    {
        this->uid = UserId;
        Result rc = accountGetProfile(&this->prf, UserId);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            this->CleanData();
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        this->pbase = (AccountProfileBase*)malloc(sizeof(AccountProfileBase));
        this->udata = (AccountUserData*)malloc(sizeof(AccountUserData));
        rc = accountProfileGet(&this->prf, this->udata, this->pbase);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            this->CleanData();
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        mainapp->LoadMenuHead(set::GetDictionaryEntry(212) + " " + std::string(this->pbase->username));
        auto res = acc::GetProfileEditor(UserId);
        rc = std::get<0>(res);
        if(rc != 0)
        {
            HandleResult(rc, set::GetDictionaryEntry(211));
            this->CleanData();
            mainapp->UnloadMenuData();
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            return;
        }
        this->pred = std::get<1>(res);
        std::string iconpth = "sdmc:/goldleaf/userdata/" + horizon::FormatHex128(UserId) + ".jpg";
        fs::DeleteFile(iconpth);
        size_t imgsz = 0;
        size_t pimgsz = 0;
        rc = accountProfileGetImageSize(&this->prf, &pimgsz);
        u8 *img = (u8*)malloc(pimgsz);
        rc = accountProfileLoadImage(&this->prf, img, pimgsz, &imgsz);
        FILE *f = fopen(iconpth.c_str(), "wb");
        if((rc == 0) && f) fwrite(img, pimgsz, 1, f);
        fclose(f);
        free(img);
    }

    void AccountLayout::CleanData()
    {
        if(this->uid != 0)
        {
            this->uid = 0;
            serviceClose(&this->prf.s);
            free(this->pbase);
            free(this->udata);
            if(this->pred != NULL) this->pred->Close();
            this->pred = NULL;
        }
    }

    void AccountLayout::optsRename_Click()
    {
        std::string name = AskForText(set::GetDictionaryEntry(213), "");
        if(name != "")
        {
            if(name.length() <= 10)
            {
                strcpy(this->pbase->username, name.c_str());
                Result rc = this->pred->Store(this->pbase, this->udata);
                if(rc == 0)
                {
                    mainapp->LoadMenuHead(set::GetDictionaryEntry(212) + " " + name);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(214) + " \'" + name + "\'.");
                }
                else HandleResult(rc, set::GetDictionaryEntry(215));
            }
            else mainapp->UpdateFooter(set::GetDictionaryEntry(249));
        }
    }

    void AccountLayout::optsIcon_Click()
    {
        std::string iconpth = "sdmc:/goldleaf/userdata/" + horizon::FormatHex128(this->uid) + ".jpg";
        mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(217) + "\n\'" + iconpth + "\'", { set::GetDictionaryEntry(234) }, false, iconpth);
    }

    void AccountLayout::optsDelete_Click()
    {
        int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(216), set::GetDictionaryEntry(218), { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true);
        if(sopt == 0)
        {
            Result rc = acc::DeleteUser(this->uid);
            if(rc == 0)
            {
                mainapp->UpdateFooter(set::GetDictionaryEntry(219));
                this->CleanData();
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
            else HandleResult(rc, set::GetDictionaryEntry(220));
        }
    }

    SystemInfoLayout::SystemInfoLayout() : pu::Layout()
    {
        horizon::FwVersion fwv = horizon::GetFwVersion();
        this->fwText = new pu::element::TextBlock(30, 630, "Firmware: " + fwv.ToString() + " (" + fwv.DisplayName + ")");
        this->fwText->SetColor(gsets.CustomScheme.Text);
        this->sdText = new pu::element::TextBlock(280, 300, set::GetDictionaryEntry(19), 35);
        this->sdText->SetColor(gsets.CustomScheme.Text);
        this->sdBar = new pu::element::ProgressBar(220, 345, 300, 30);
        this->sdFreeText = new pu::element::TextBlock(225, 385, "0 bytes " + set::GetDictionaryEntry(221));
        this->sdFreeText->SetColor(gsets.CustomScheme.Text);
        this->nandText = new pu::element::TextBlock(600, 300, set::GetDictionaryEntry(79), 35);
        this->nandText->SetColor(gsets.CustomScheme.Text);
        this->nandBar = new pu::element::ProgressBar(660, 345, 300, 30);
        this->nandFreeText = new pu::element::TextBlock(655, 385, "0 bytes " + set::GetDictionaryEntry(221));
        this->nandText->SetColor(gsets.CustomScheme.Text);
        this->safeText = new pu::element::TextBlock(105, 480, set::GetDictionaryEntry(27));
        this->safeText->SetColor(gsets.CustomScheme.Text);
        this->safeBar = new pu::element::ProgressBar(100, 515, 300, 30);
        this->userText = new pu::element::TextBlock(455, 480, set::GetDictionaryEntry(28));
        this->userText->SetColor(gsets.CustomScheme.Text);
        this->userBar = new pu::element::ProgressBar(450, 515, 300, 30);
        this->systemText = new pu::element::TextBlock(805, 480, set::GetDictionaryEntry(29));
        this->systemText->SetColor(gsets.CustomScheme.Text);
        this->systemBar = new pu::element::ProgressBar(800, 515, 300, 30);
        this->AddChild(this->fwText);
        this->AddChild(this->sdText);
        this->AddChild(this->sdBar);
        this->AddChild(this->sdFreeText);
        this->AddChild(this->nandText);
        this->AddChild(this->nandBar);
        this->AddChild(this->nandFreeText);
        this->AddChild(this->safeText);
        this->AddChild(this->safeBar);
        this->AddChild(this->userText);
        this->AddChild(this->userBar);
        this->AddChild(this->systemText);
        this->AddChild(this->systemBar);
    }

    void SystemInfoLayout::UpdateElements()
    {
        u64 sdctotal = fs::GetTotalSpaceForPartition(fs::Partition::SdCard);
        u64 sdcfree = fs::GetFreeSpaceForPartition(fs::Partition::SdCard);
        u64 nsftotal = fs::GetTotalSpaceForPartition(fs::Partition::NANDSafe);
        u64 nsffree = fs::GetFreeSpaceForPartition(fs::Partition::NANDSafe);
        u64 nsutotal = fs::GetTotalSpaceForPartition(fs::Partition::NANDUser);
        u64 nsufree = fs::GetFreeSpaceForPartition(fs::Partition::NANDUser);
        u64 nsstotal = fs::GetTotalSpaceForPartition(fs::Partition::NANDSystem);
        u64 nssfree = fs::GetFreeSpaceForPartition(fs::Partition::NANDSystem);
        u8 sdval = ((100 * (sdctotal - sdcfree)) / sdctotal);
        u8 nsfval = ((100 * (nsftotal - nsffree)) / nsftotal);
        u8 nsuval = ((100 * (nsutotal - nsufree)) / nsutotal);
        u8 nssval = ((100 * (nsstotal - nssfree)) / nsstotal);
        this->sdBar->SetProgress(sdval);
        this->safeBar->SetProgress(nsfval);
        this->userBar->SetProgress(nsuval);
        this->systemBar->SetProgress(nssval);
        u64 nandtotal = (nsftotal + nsutotal + nsstotal);
        u64 nandfree = (nsffree + nsufree + nssfree);
        u8 nandval = ((100 * (nandtotal - nandfree)) / nandtotal);
        this->nandBar->SetProgress(nandval);
        this->nandFreeText->SetText(fs::FormatSize(nandfree) + " free");
        this->sdFreeText->SetText(fs::FormatSize(sdcfree) + " free");
    }

    AboutLayout::AboutLayout()
    {
        this->logoImage = new pu::element::Image(85, 150, gsets.PathForResource("/Logo.png"));
        this->AddChild(this->logoImage);
    }

    MainApplication::MainApplication() : pu::Application()
    {
        gsets = set::ProcessSettings();
        set::Initialize();
        this->SetBackgroundColor(gsets.CustomScheme.Background);
        this->preblv = 0;
        this->preisch = false;
        this->pretime = "";
        this->vfirst = true;
        this->baseImage = new pu::element::Image(0, 0, gsets.PathForResource("/Base.png"));
        this->timeText = new pu::element::TextBlock(1124, 18, "00:00:00");
        this->timeText->SetColor(gsets.CustomScheme.Text);
        this->batteryText = new pu::element::TextBlock(1020, 20, "0%", 20);
        this->batteryText->SetColor(gsets.CustomScheme.Text);
        this->batteryImage = new pu::element::Image(960, 8, gsets.PathForResource("/Battery/0.png"));
        this->batteryChargeImage = new pu::element::Image(960, 8, gsets.PathForResource("/Battery/Charge.png"));
        this->menuBanner = new pu::element::Image(10, 62, gsets.PathForResource("/MenuBanner.png"));
        this->menuImage = new pu::element::Image(10, 67, gsets.PathForResource("/Common/SdCard.png"));
        this->menuImage->SetWidth(100);
        this->menuImage->SetHeight(100);
        this->usbImage = new pu::element::Image(905, 12, gsets.PathForResource("/Common/USB.png"));
        this->usbImage->SetWidth(40);
        this->usbImage->SetHeight(40);
        this->usbImage->SetVisible(false);
        this->menuNameText = new pu::element::TextBlock(120, 90, "-");
        this->menuNameText->SetColor(gsets.CustomScheme.Text);
        this->menuHeadText = new pu::element::TextBlock(120, 125, "-", 20);
        this->menuHeadText->SetColor(gsets.CustomScheme.Text);
        this->UnloadMenuData();
        this->footerText = new pu::element::TextBlock(15, 685, set::GetDictionaryEntry(9), 20);
        this->footerText->SetColor(gsets.CustomScheme.Text);
        this->UpdateValues();
        this->mainMenu = new MainMenuLayout();
        this->sdBrowser = new PartitionBrowserLayout(fs::Partition::SdCard);
        this->sdBrowser->SetOnInput(std::bind(&MainApplication::sdBrowser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->nandBrowser = new PartitionBrowserLayout(fs::Partition::NANDSystem);
        this->nandBrowser->SetOnInput(std::bind(&MainApplication::nandBrowser_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->fileContent = new FileContentLayout();
        this->fileContent->SetOnInput(std::bind(&MainApplication::fileContent_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->copy = new CopyLayout();
        this->nspInstall = new InstallLayout();
        this->usbInstall = new USBInstallLayout();
        this->usbInstall->SetOnInput(std::bind(&MainApplication::usbInstall_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->contentInformation = new ContentInformationLayout();
        this->contentInformation->SetOnInput(std::bind(&MainApplication::contentInformation_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->storageContents = new StorageContentsLayout();
        this->storageContents->SetOnInput(std::bind(&MainApplication::storageContents_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->contentManager = new ContentManagerLayout();
        this->contentManager->SetOnInput(std::bind(&MainApplication::contentManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->titleDump = new TitleDumperLayout();
        this->ticketManager = new TicketManagerLayout();
        this->ticketManager->SetOnInput(std::bind(&MainApplication::ticketManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->account = new AccountLayout();
        this->account->SetOnInput(std::bind(&MainApplication::account_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->sysInfo = new SystemInfoLayout();
        this->sysInfo->SetOnInput(std::bind(&MainApplication::sysInfo_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->about = new AboutLayout();
        this->about->SetOnInput(std::bind(&MainApplication::about_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->mainMenu->AddChild(this->usbImage);
        this->mainMenu->AddChild(this->baseImage);
        this->sdBrowser->AddChild(this->baseImage);
        this->nandBrowser->AddChild(this->baseImage);
        this->fileContent->AddChild(this->baseImage);
        this->copy->AddChild(this->baseImage);
        this->nspInstall->AddChild(this->baseImage);
        this->usbInstall->AddChild(this->baseImage);
        this->contentInformation->AddChild(this->baseImage);
        this->storageContents->AddChild(this->baseImage);
        this->contentManager->AddChild(this->baseImage);
        this->titleDump->AddChild(this->baseImage);
        this->ticketManager->AddChild(this->baseImage);
        this->account->AddChild(this->baseImage);
        this->sysInfo->AddChild(this->baseImage);
        this->about->AddChild(this->baseImage);
        this->mainMenu->AddChild(this->timeText);
        this->sdBrowser->AddChild(this->timeText);
        this->nandBrowser->AddChild(this->timeText);
        this->fileContent->AddChild(this->timeText);
        this->copy->AddChild(this->timeText);
        this->nspInstall->AddChild(this->timeText);
        this->usbInstall->AddChild(this->timeText);
        this->contentInformation->AddChild(this->timeText);
        this->storageContents->AddChild(this->timeText);
        this->contentManager->AddChild(this->timeText);
        this->titleDump->AddChild(this->timeText);
        this->ticketManager->AddChild(this->timeText);
        this->account->AddChild(this->timeText);
        this->sysInfo->AddChild(this->timeText);
        this->about->AddChild(this->timeText);
        this->mainMenu->AddChild(this->batteryText);
        this->sdBrowser->AddChild(this->batteryText);
        this->nandBrowser->AddChild(this->batteryText);
        this->fileContent->AddChild(this->batteryText);
        this->copy->AddChild(this->batteryText);
        this->nspInstall->AddChild(this->batteryText);
        this->usbInstall->AddChild(this->batteryText);
        this->contentInformation->AddChild(this->batteryText);
        this->storageContents->AddChild(this->batteryText);
        this->contentManager->AddChild(this->batteryText);
        this->titleDump->AddChild(this->batteryText);
        this->ticketManager->AddChild(this->batteryText);
        this->account->AddChild(this->batteryText);
        this->sysInfo->AddChild(this->batteryText);
        this->about->AddChild(this->batteryText);
        this->mainMenu->AddChild(this->batteryImage);
        this->sdBrowser->AddChild(this->batteryImage);
        this->nandBrowser->AddChild(this->batteryImage);
        this->fileContent->AddChild(this->batteryImage);
        this->copy->AddChild(this->batteryImage);
        this->nspInstall->AddChild(this->batteryImage);
        this->usbInstall->AddChild(this->batteryImage);
        this->contentInformation->AddChild(this->batteryImage);
        this->storageContents->AddChild(this->batteryImage);
        this->contentManager->AddChild(this->batteryImage);
        this->titleDump->AddChild(this->batteryImage);
        this->ticketManager->AddChild(this->batteryImage);
        this->account->AddChild(this->batteryImage);
        this->sysInfo->AddChild(this->batteryImage);
        this->about->AddChild(this->batteryImage);
        this->mainMenu->AddChild(this->batteryChargeImage);
        this->sdBrowser->AddChild(this->batteryChargeImage);
        this->nandBrowser->AddChild(this->batteryChargeImage);
        this->fileContent->AddChild(this->batteryChargeImage);
        this->copy->AddChild(this->batteryChargeImage);
        this->nspInstall->AddChild(this->batteryChargeImage);
        this->usbInstall->AddChild(this->batteryChargeImage);
        this->contentInformation->AddChild(this->batteryChargeImage);
        this->storageContents->AddChild(this->batteryChargeImage);
        this->contentManager->AddChild(this->batteryChargeImage);
        this->titleDump->AddChild(this->batteryChargeImage);
        this->ticketManager->AddChild(this->batteryChargeImage);
        this->account->AddChild(this->batteryChargeImage);
        this->sysInfo->AddChild(this->batteryChargeImage);
        this->about->AddChild(this->batteryChargeImage);
        this->mainMenu->AddChild(this->menuImage);
        this->sdBrowser->AddChild(this->menuImage);
        this->nandBrowser->AddChild(this->menuImage);
        this->fileContent->AddChild(this->menuImage);
        this->copy->AddChild(this->menuImage);
        this->nspInstall->AddChild(this->menuImage);
        this->usbInstall->AddChild(this->menuImage);
        this->contentInformation->AddChild(this->menuImage);
        this->storageContents->AddChild(this->menuImage);
        this->contentManager->AddChild(this->menuImage);
        this->titleDump->AddChild(this->menuImage);
        this->ticketManager->AddChild(this->menuImage);
        this->account->AddChild(this->menuImage);
        this->sysInfo->AddChild(this->menuImage);
        this->about->AddChild(this->menuImage);
        this->mainMenu->AddChild(this->menuBanner);
        this->mainMenu->AddChild(this->menuNameText);
        this->sdBrowser->AddChild(this->menuNameText);
        this->nandBrowser->AddChild(this->menuNameText);
        this->fileContent->AddChild(this->menuNameText);
        this->copy->AddChild(this->menuNameText);
        this->nspInstall->AddChild(this->menuNameText);
        this->usbInstall->AddChild(this->menuNameText);
        this->contentInformation->AddChild(this->menuNameText);
        this->storageContents->AddChild(this->menuNameText);
        this->contentManager->AddChild(this->menuNameText);
        this->titleDump->AddChild(this->menuNameText);
        this->ticketManager->AddChild(this->menuNameText);
        this->account->AddChild(this->menuNameText);
        this->sysInfo->AddChild(this->menuNameText);
        this->about->AddChild(this->menuNameText);
        this->mainMenu->AddChild(this->menuHeadText);
        this->sdBrowser->AddChild(this->menuHeadText);
        this->nandBrowser->AddChild(this->menuHeadText);
        this->fileContent->AddChild(this->menuHeadText);
        this->copy->AddChild(this->menuHeadText);
        this->nspInstall->AddChild(this->menuHeadText);
        this->usbInstall->AddChild(this->menuHeadText);
        this->contentInformation->AddChild(this->menuHeadText);
        this->storageContents->AddChild(this->menuHeadText);
        this->contentManager->AddChild(this->menuHeadText);
        this->titleDump->AddChild(this->menuHeadText);
        this->ticketManager->AddChild(this->menuHeadText);
        this->account->AddChild(this->menuHeadText);
        this->sysInfo->AddChild(this->menuHeadText);
        this->about->AddChild(this->menuHeadText);
        this->mainMenu->AddChild(this->footerText);
        this->sdBrowser->AddChild(this->footerText);
        this->nandBrowser->AddChild(this->footerText);
        this->fileContent->AddChild(this->footerText);
        this->copy->AddChild(this->footerText);
        this->nspInstall->AddChild(this->footerText);
        this->usbInstall->AddChild(this->footerText);
        this->contentInformation->AddChild(this->footerText);
        this->storageContents->AddChild(this->footerText);
        this->contentManager->AddChild(this->footerText);
        this->titleDump->AddChild(this->footerText);
        this->ticketManager->AddChild(this->footerText);
        this->account->AddChild(this->footerText);
        this->sysInfo->AddChild(this->footerText);
        this->AddThread(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->LoadLayout(this->mainMenu);
    }

    void MainApplication::UpdateFooter(std::string Text)
    {
        this->footerText->SetText(Text);
    }

    void MainApplication::UpdateValues()
    {
        std::string dtime = horizon::GetCurrentTime();
        u32 blv = horizon::GetBatteryLevel();
        bool isch = horizon::IsCharging();
        if((this->preblv != blv) || this->vfirst)
        {
            if(blv <= 10) this->batteryImage->SetImage(gsets.PathForResource("/Battery/0.png"));
            else if((blv > 10) && (blv <= 20)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/10.png"));
            else if((blv > 20) && (blv <= 30)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/20.png"));
            else if((blv > 30) && (blv <= 40)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/30.png"));
            else if((blv > 40) && (blv <= 50)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/40.png"));
            else if((blv > 50) && (blv <= 60)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/50.png"));
            else if((blv > 60) && (blv <= 70)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/60.png"));
            else if((blv > 70) && (blv <= 80)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/70.png"));
            else if((blv > 80) && (blv <= 90)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/80.png"));
            else if((blv > 90) && (blv < 100)) this->batteryImage->SetImage(gsets.PathForResource("/Battery/90.png"));
            else if(blv == 100) this->batteryImage->SetImage(gsets.PathForResource("/Battery/100.png"));
            this->batteryText->SetText(std::to_string(blv) + "%");
            this->preblv = blv;
        }
        if((this->preisch != isch) || this->vfirst)
        {
            if(isch) this->batteryChargeImage->SetVisible(true);
            else this->batteryChargeImage->SetVisible(false);
            this->preisch = isch;
        }
        if((this->pretime != dtime) || this->vfirst)
        {
            this->timeText->SetText(dtime);
            this->pretime = dtime;
        }
        if(this->vfirst) this->vfirst = false;
        u32 ustate = 0;
        usbDsGetState(&ustate);
        this->hasusb = (ustate == 5);
        this->usbImage->SetVisible(this->hasusb);
        if(IsQlaunch())
        {
            if(IsGpioInputPressed(horizon::GpioInput::VolumeDown)) mainapp->UpdateFooter("GPIO DOWN");
        }
    }

    void MainApplication::LoadMenuData(std::string Name, std::string ImageName, std::string TempHead, bool CommonIcon)
    {
        if(this->menuImage != NULL)
        {
            this->menuImage->SetVisible(true);
            if(CommonIcon) this->menuImage->SetImage(gsets.PathForResource("/Common/" + ImageName + ".png"));
            else this->menuImage->SetImage(ImageName);
        }
        if(this->menuNameText != NULL)
        {
            this->menuNameText->SetVisible(true);
            this->menuNameText->SetText(Name);
        }
        if(this->menuHeadText != NULL)
        {
            this->menuHeadText->SetVisible(true);
            this->LoadMenuHead(TempHead);
        }
    }

    void MainApplication::LoadMenuHead(std::string Head)
    {
        if(this->menuHeadText != NULL) this->menuHeadText->SetText(Head);
    }

    void MainApplication::UnloadMenuData()
    {
        this->menuImage->SetVisible(false);
        this->menuNameText->SetVisible(false);
        this->menuHeadText->SetVisible(false);
    }

    void MainApplication::sdBrowser_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            if(this->sdBrowser->GoBack()) this->sdBrowser->UpdateElements();
            else
            {
                this->UnloadMenuData();
                this->LoadLayout(this->mainMenu);
            }
        }
        else if(Down & KEY_X)
        {
            if(clipboard != "")
            {
                bool cdir = fs::IsDirectory(clipboard);
                std::string fsicon;
                if(cdir) fsicon = gsets.PathForResource("/FileSystem/Directory.png");
                else
                {
                    std::string ext = fs::GetExtension(clipboard);
                    if(ext == "nsp") fsicon = gsets.PathForResource("/FileSystem/NSP.png");
                    else if(ext == "nro") fsicon = gsets.PathForResource("/FileSystem/NRO.png");
                    else if(ext == "tik") fsicon = gsets.PathForResource("/FileSystem/TIK.png");
                    else if(ext == "cert") fsicon = gsets.PathForResource("/FileSystem/CERT.png");
                    else if(ext == "nca") fsicon = gsets.PathForResource("/FileSystem/NCA.png");
                    else if(ext == "nxtheme") fsicon = gsets.PathForResource("/FileSystem/NXTheme.png");
                    else fsicon = gsets.PathForResource("/FileSystem/File.png");
                }
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(222), set::GetDictionaryEntry(223) + "\n(" + clipboard + ")", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true, fsicon);
                if(sopt == 0)
                {
                    std::string cname = fs::GetFileName(clipboard);
                    mainapp->LoadLayout(mainapp->GetCopyLayout());
                    mainapp->GetCopyLayout()->StartCopy(clipboard, this->sdBrowser->GetExplorer()->FullPathFor(cname), cdir, this->sdBrowser);
                    this->sdBrowser->UpdateElements();
                    clipboard = "";
                }
            }
            else mainapp->UpdateFooter(set::GetDictionaryEntry(224));
        }
        else if(Down & KEY_L)
        {
            std::string cfile = AskForText(set::GetDictionaryEntry(225), "");
            if(cfile != "")
            {
                std::string ffile = this->sdBrowser->GetExplorer()->FullPathFor(cfile);
                std::string pffile = this->sdBrowser->GetExplorer()->FullPresentablePathFor(cfile);
                if(fs::IsFile(ffile) || fs::IsDirectory(ffile)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(255));
                else
                {
                    fs::CreateFile(ffile);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(227) + " \'" + pffile + "\'");
                    this->sdBrowser->UpdateElements();
                }
            }
        }
        else if(Down & KEY_R)
        {
            std::string cdir = AskForText(set::GetDictionaryEntry(250), "");
            if(cdir != "")
            {
                std::string fdir = this->sdBrowser->GetExplorer()->FullPathFor(cdir);
                std::string pfdir = this->sdBrowser->GetExplorer()->FullPresentablePathFor(cdir);
                if(fs::IsFile(fdir) || fs::IsDirectory(fdir)) HandleResult(err::Make(err::ErrorDescription::FileDirectoryAlreadyPresent), set::GetDictionaryEntry(255));
                else
                {
                    fs::CreateDirectory(fdir);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(228) + " \'" + pfdir + "\'");
                    this->sdBrowser->UpdateElements();
                }
            }
        }
    }

    void MainApplication::nandBrowser_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            if(this->nandBrowser->GoBack()) this->nandBrowser->UpdateElements();
            else
            {
                this->UnloadMenuData();
                this->LoadLayout(this->mainMenu);
            }
        }
        else if(Down & KEY_X)
        {
            if(clipboard != "")
            {
                bool cdir = fs::IsDirectory(clipboard);
                std::string fsicon;
                if(cdir) fsicon = gsets.PathForResource("/FileSystem/Directory.png");
                else
                {
                    std::string ext = fs::GetExtension(clipboard);
                    if(ext == "nsp") fsicon = gsets.PathForResource("/FileSystem/NSP.png");
                    else if(ext == "nro") fsicon = gsets.PathForResource("/FileSystem/NRO.png");
                    else if(ext == "tik") fsicon = gsets.PathForResource("/FileSystem/TIK.png");
                    else if(ext == "cert") fsicon = gsets.PathForResource("/FileSystem/CERT.png");
                    else if(ext == "nca") fsicon = gsets.PathForResource("/FileSystem/NCA.png");
                    else if(ext == "nxtheme") fsicon = gsets.PathForResource("/FileSystem/NXTheme.png");
                    else fsicon = gsets.PathForResource("/FileSystem/File.png");
                }
                int sopt = mainapp->CreateShowDialog(set::GetDictionaryEntry(222), set::GetDictionaryEntry(223) + "\n(" + clipboard + ")", { set::GetDictionaryEntry(111), set::GetDictionaryEntry(18) }, true, fsicon);
                if((sopt == 0) && this->nandBrowser->WarnNANDWriteAccess())
                {
                    std::string cname = fs::GetFileName(clipboard);
                    mainapp->LoadLayout(mainapp->GetCopyLayout());
                    mainapp->GetCopyLayout()->StartCopy(clipboard, this->nandBrowser->GetExplorer()->FullPathFor(cname), cdir, this->nandBrowser);
                    this->nandBrowser->UpdateElements();
                    ClearClipboard();
                }
            }
            else mainapp->UpdateFooter(set::GetDictionaryEntry(224));
        }
        else if(Down & KEY_L)
        {
            std::string cfile = AskForText(set::GetDictionaryEntry(225), "");
            if(cfile != "")
            {
                std::string ffile = this->nandBrowser->GetExplorer()->FullPathFor(cfile);
                std::string pffile = this->nandBrowser->GetExplorer()->FullPresentablePathFor(cfile);
                if(fs::IsFile(ffile) || fs::IsDirectory(ffile)) mainapp->UpdateFooter(set::GetDictionaryEntry(226));
                else if(this->nandBrowser->WarnNANDWriteAccess())
                {
                    fs::CreateFile(ffile);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(227) + " \'" + pffile + "\'");
                    this->nandBrowser->UpdateElements();
                }
            }
        }
        else if(Down & KEY_R)
        {
            std::string cdir = AskForText(set::GetDictionaryEntry(250), "");
            if(cdir != "")
            {
                std::string fdir = this->nandBrowser->GetExplorer()->FullPathFor(cdir);
                std::string pfdir = this->nandBrowser->GetExplorer()->FullPresentablePathFor(cdir);
                if(fs::IsFile(fdir) || fs::IsDirectory(fdir)) mainapp->UpdateFooter(set::GetDictionaryEntry(226));
                else if(this->nandBrowser->WarnNANDWriteAccess())
                {
                    fs::CreateDirectory(fdir);
                    mainapp->UpdateFooter(set::GetDictionaryEntry(228) + " \'" + pfdir + "\'");
                    this->nandBrowser->UpdateElements();
                }
            }
        }
    }

    void MainApplication::usbInstall_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::fileContent_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B) this->LoadLayout(this->fileContent->GetPreviousLayout());
        else if((Down & KEY_DDOWN) || (Down & KEY_LSTICK_DOWN) || (Held & KEY_RSTICK_DOWN)) this->fileContent->ScrollDown();
        else if((Down & KEY_DUP) || (Down & KEY_LSTICK_UP) || (Held & KEY_RSTICK_UP)) this->fileContent->ScrollUp();
    }

    void MainApplication::contentInformation_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            mainapp->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(189));
            this->LoadLayout(this->storageContents);
        }
    }

    void MainApplication::storageContents_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            mainapp->LoadMenuData(set::GetDictionaryEntry(187), "Storage", set::GetDictionaryEntry(33));
            this->LoadLayout(this->contentManager);
        }
    }

    void MainApplication::contentManager_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::ticketManager_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::account_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->account->CleanData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::sysInfo_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::about_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::OnInput(u64 Down, u64 Up, u64 Held)
    {
        if(((Down & KEY_PLUS) || (Down & KEY_MINUS)) && IsNRO()) this->Close();
        else if((Down & KEY_ZL) || (Down & KEY_ZR)) ShowPowerTasksDialog(set::GetDictionaryEntry(229), set::GetDictionaryEntry(230));
    }

    MainMenuLayout *MainApplication::GetMainMenuLayout()
    {
        return this->mainMenu;
    }

    PartitionBrowserLayout *MainApplication::GetSDBrowserLayout()
    {
        return this->sdBrowser;
    }

    PartitionBrowserLayout *MainApplication::GetNANDBrowserLayout()
    {
        return this->nandBrowser;
    }

    FileContentLayout *MainApplication::GetFileContentLayout()
    {
        return this->fileContent;
    }

    CopyLayout *MainApplication::GetCopyLayout()
    {
        return this->copy;
    }

    InstallLayout *MainApplication::GetInstallLayout()
    {
        return this->nspInstall;
    }

    USBInstallLayout *MainApplication::GetUSBInstallLayout()
    {
        return this->usbInstall;
    }

    ContentInformationLayout *MainApplication::GetContentInformationLayout()
    {
        return this->contentInformation;
    }

    StorageContentsLayout *MainApplication::GetStorageContentsLayout()
    {
        return this->storageContents;
    }

    ContentManagerLayout *MainApplication::GetContentManagerLayout()
    {
        return this->contentManager;
    }

    TitleDumperLayout *MainApplication::GetTitleDumperLayout()
    {
        return this->titleDump;
    }

    TicketManagerLayout *MainApplication::GetTicketManagerLayout()
    {
        return this->ticketManager;
    }

    AccountLayout *MainApplication::GetAccountLayout()
    {
        return this->account;
    }

    SystemInfoLayout *MainApplication::GetSystemInfoLayout()
    {
        return this->sysInfo;
    }

    AboutLayout *MainApplication::GetAboutLayout()
    {
        return this->about;
    }

    void UpdateClipboard(std::string Path)
    {
        SetClipboard(Path);
        std::string copymsg;
        if(fs::IsFile(Path)) copymsg = set::GetDictionaryEntry(257);
        else copymsg = set::GetDictionaryEntry(258);
        mainapp->UpdateFooter(copymsg);
    }

    void SetMainApplication(MainApplication *MainApp)
    {
        mainapp = MainApp;
    }
}