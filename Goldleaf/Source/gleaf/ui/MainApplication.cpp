#include <gleaf/ui/MainApplication.hpp>
#include <threads.h>

namespace gleaf::ui
{
    MainApplication *mainapp;
    std::string clipboard;

    MainMenuLayout::MainMenuLayout() : pu::Layout()
    {
        this->optionMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->optionMenu->SetOnSelectionChanged(std::bind(&MainMenuLayout::optionMenu_SelectionChanged, this));
        this->sdcardMenuItem = new pu::element::MenuItem("Browse SD card");
        this->sdcardMenuItem->SetIcon("romfs:/Common/SdCard.png");
        this->sdcardMenuItem->AddOnClick(std::bind(&MainMenuLayout::sdcardMenuItem_Click, this));
        this->nandMenuItem = new pu::element::MenuItem("Browse console memory");
        this->nandMenuItem->SetIcon("romfs:/Common/NAND.png");
        this->nandMenuItem->AddOnClick(std::bind(&MainMenuLayout::nandMenuItem_Click, this));
        this->usbMenuItem = new pu::element::MenuItem("USB installation (via Goldtree)");
        this->usbMenuItem->SetIcon("romfs:/Common/USB.png");
        this->usbMenuItem->AddOnClick(std::bind(&MainMenuLayout::usbMenuItem_Click, this));
        this->titleMenuItem = new pu::element::MenuItem("Manage installed titles");
        this->titleMenuItem->SetIcon("romfs:/Common/Storage.png");
        this->titleMenuItem->AddOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->ticketMenuItem = new pu::element::MenuItem("Manage imported tickets");
        this->ticketMenuItem->SetIcon("romfs:/Common/Ticket.png");
        this->ticketMenuItem->AddOnClick(std::bind(&MainMenuLayout::ticketMenuItem_Click, this));
        this->webMenuItem = new pu::element::MenuItem("Browse the internet");
        this->webMenuItem->SetIcon("romfs:/Common/Browser.png");
        this->webMenuItem->AddOnClick(std::bind(&MainMenuLayout::webMenuItem_Click, this));
        this->accountMenuItem = new pu::element::MenuItem("Manage user accounts");
        this->accountMenuItem->SetIcon("romfs:/Common/Accounts.png");
        this->accountMenuItem->AddOnClick(std::bind(&MainMenuLayout::accountMenuItem_Click, this));
        this->cfwConfigMenuItem = new pu::element::MenuItem("CFW configuration");
        this->cfwConfigMenuItem->SetIcon("romfs:/Common/CFW.png");
        this->cfwConfigMenuItem->AddOnClick(std::bind(&MainMenuLayout::cfwConfigMenuItem_Click, this));
        this->sysinfoMenuItem = new pu::element::MenuItem("Console information");
        this->sysinfoMenuItem->SetIcon("romfs:/Common/Settings.png");
        this->sysinfoMenuItem->AddOnClick(std::bind(&MainMenuLayout::sysinfoMenuItem_Click, this));
        this->aboutMenuItem = new pu::element::MenuItem("About Goldleaf");
        this->aboutMenuItem->SetIcon("romfs:/Common/Info.png");
        this->aboutMenuItem->AddOnClick(std::bind(&MainMenuLayout::aboutMenuItem_Click, this));
        this->optionMenu->AddItem(this->sdcardMenuItem);
        this->optionMenu->AddItem(this->nandMenuItem);
        this->optionMenu->AddItem(this->usbMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->ticketMenuItem);
        this->optionMenu->AddItem(this->webMenuItem);
        this->optionMenu->AddItem(this->accountMenuItem);
        this->optionMenu->AddItem(this->cfwConfigMenuItem);
        this->optionMenu->AddItem(this->sysinfoMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->AddChild(this->optionMenu);
    }

    void MainMenuLayout::optionMenu_SelectionChanged()
    {
        std::string info = "Welcome to Goldleaf! You can install NSPs, import tickets, uninstall titles, remove tickets, browse SD and NAND...";
        pu::element::MenuItem *isel = this->optionMenu->GetSelectedItem();
        if(isel == this->sdcardMenuItem) info = "Browse the SD card. Press A to view file options or to browse a directory, X to paste clipboard or Y to view directory options.";
        else if(isel == this->nandMenuItem) info = "Browse NAND. Press A to view file options or to browse a directory, X to paste clipboard or Y to view directory options.";
        else if(isel == this->usbMenuItem) info = "Install NSPs from a PC via USB, using Goldtree client.";
        else if(isel == this->titleMenuItem) info = "Browse currently installed titles. You can view their information, dump them as NSPs or uninstall them.";
        else if(isel == this->ticketMenuItem) info = "Browse currently installed tickets. You can view their information and remove them.";
        else if(isel == this->webMenuItem) info = "Browse the internet using the console's web-applet. (hidden browser title)";
        else if(isel == this->accountMenuItem) info = "Manage user accounts: rename, change icon, delete them...";
        else if(isel == this->cfwConfigMenuItem) info = "Browse which CFWs are available to install themes of if there is any theme installed.";
        else if(isel == this->sysinfoMenuItem) info = "Display information about this Nintendo Switch: current firmware and used space in NAND and SD card and firmware version.";
        else if(isel == this->aboutMenuItem) info = "Display information about Goldleaf. You can check Goldleaf's version there.";
        mainapp->UpdateFooter(info);
    }

    void MainMenuLayout::sdcardMenuItem_Click()
    {
        mainapp->LoadMenuData("SD card", "SdCard", mainapp->GetSDBrowserLayout()->GetExplorer()->GetCwd());
        mainapp->GetSDBrowserLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
    }

    void MainMenuLayout::nandMenuItem_Click()
    {
        pu::Dialog *dlg = new pu::Dialog("Select console partition", "Which partition of the console memory would you like to browse?\n\nPRODINFOF contains several \"calibration\" contents.\nNAND's SAFE partition is usually empty.\nNAND's SYSTEM partition contains system NCAs and save datas.\nNAND's USER partition contains NCAs of titles installed to console memory.");
        dlg->AddOption("PRODINFOF");
        dlg->AddOption("NAND SAFE");
        dlg->AddOption("NAND USER");
        dlg->AddOption("NAND SYSTEM");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 4)) return;
        if(sopt == 0) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::PRODINFOF);
        else if(sopt == 1) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::NANDSafe);
        else if(sopt == 2) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::NANDUser);
        else if(sopt == 3) mainapp->GetNANDBrowserLayout()->ChangePartition(fs::Partition::NANDSystem);
        mainapp->LoadMenuData("NAND", "NAND", mainapp->GetNANDBrowserLayout()->GetExplorer()->GetCwd());
        mainapp->LoadLayout(mainapp->GetNANDBrowserLayout());
    }

    void MainMenuLayout::usbMenuItem_Click()
    {
        pu::Dialog *dlg = new pu::Dialog("Start USB installation", "Would you like to use USB installation?\nIt's currently unstable, and might not work due to weird bugs.\nProceed with caution if so.");
        dlg->AddOption("Start");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 1)) return;
        mainapp->LoadMenuData("USB installation", "USB", "Preparing USB install...");
        mainapp->LoadLayout(mainapp->GetUSBInstallLayout());
        mainapp->GetUSBInstallLayout()->StartUSBConnection();
    }

    void MainMenuLayout::titleMenuItem_Click()
    {
        mainapp->LoadMenuData("Titles", "Storage", "Searching for installed titles...");
        EnsureDirectories();
        mainapp->GetTitleManagerLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
    }

    void MainMenuLayout::ticketMenuItem_Click()
    {
        mainapp->LoadMenuData("Tickets", "Ticket", "Searching for imported tickets...");
        mainapp->GetTicketManagerLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetTicketManagerLayout());
        pu::Dialog *dlg = new pu::Dialog("Removing tickets", "Removing tickets can be dangerous.\nIf tickets from installed titles get removed, they won't probably work.");
        dlg->AddOption("Ok");
        mainapp->ShowDialog(dlg);
    }

    void MainMenuLayout::webMenuItem_Click()
    {
        if(IsApplication())
        {
            std::string out = AskForText("Select web page to browse.", "https://dns.switchbru.com/");
            if(out == "") return;
            else
            {
                bool nothttp = (out.substr(0, 6) != "http:/");
                bool nothttps = (out.substr(0, 7) != "https:/");
                if(nothttp && nothttps)
                {
                    pu::Dialog *dlg = new pu::Dialog("Web browsing", "An invalid web page was selected.");
                    dlg->AddOption("Ok");
                    mainapp->ShowDialog(dlg);
                    return;
                }
            }
            AppletHolder aph;
            AppletStorage hast1;
            LibAppletArgs args;
            appletCreateLibraryApplet(&aph, AppletId_web, LibAppletMode_AllForeground);
            libappletArgsCreate(&args, 0x50000);
            libappletArgsPush(&args, &aph);
            appletCreateStorage(&hast1, 8192);
            u8 indata[8192] = { 0 };
            *(u64*)&indata[4] = 281530811285509;
            *(u64*)&indata[17] = 201326593;
            *(u8*)&indata[16] = 1;
            *(u16*)indata = 2;
            strcpy((char*)&indata[25], out.c_str());
            appletStorageWrite(&hast1, 0, indata, 8192);
            appletHolderPushInData(&aph, &hast1);
            appletHolderStart(&aph);
            appletHolderJoin(&aph);
        }
        else
        {
            pu::Dialog *dlg = new pu::Dialog("Web browsing", "For technical reasons, the console's hidden browser (the web-applet title) can only be used as an application.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
        }
    }

    void MainMenuLayout::accountMenuItem_Click()
    {
        u128 uid = AskForUser();
        if(uid == 0) return;
        mainapp->LoadMenuData("User accounts", "Accounts", "Processing user account...");
        mainapp->GetAccountLayout()->Load(uid);
        mainapp->LoadLayout(mainapp->GetAccountLayout());
    }

    void MainMenuLayout::cfwConfigMenuItem_Click()
    {
        mainapp->LoadMenuData("CFW and themes", "CFW", "Searching for CFWs on the SD card...");
        mainapp->GetCFWConfigLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetCFWConfigLayout());
    }

    void MainMenuLayout::sysinfoMenuItem_Click()
    {
        mainapp->LoadMenuData("System information", "Settings", "Loading settings and information of the system...");
        mainapp->GetSystemInfoLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSystemInfoLayout());
    }

    void MainMenuLayout::aboutMenuItem_Click()
    {
        mainapp->LoadMenuData("Goldleaf v0.3", "Info", (IsApplication() ? "Running as an installed title." : "Running as a homebrew NRO binary."));
        mainapp->LoadLayout(mainapp->GetAboutLayout());
    }

    PartitionBrowserLayout::PartitionBrowserLayout(fs::Partition Partition) : pu::Layout()
    {
        this->gexp = new fs::Explorer(Partition);
        this->browseMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->dirEmptyText = new pu::element::TextBlock(30, 630, "The directory is empty.");
        this->AddChild(this->browseMenu);
        this->AddChild(this->dirEmptyText);
    }

    void PartitionBrowserLayout::ChangePartition(fs::Partition Partition)
    {
        this->gexp->MovePartition(Partition);
        this->UpdateElements();
    }

    void PartitionBrowserLayout::UpdateElements()
    {
        if(!this->elems.empty()) this->elems.clear();
        this->elems = this->gexp->GetContents();
        this->browseMenu->ClearItems();
        mainapp->LoadMenuHead(this->gexp->GetCwd());
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
                pu::element::MenuItem *mitm = new pu::element::MenuItem(itm);
                if(isdir) mitm->SetIcon("romfs:/FileSystem/Directory.png");
                else
                {
                    std::string ext = fs::GetExtension(itm);
                    if(ext == "nsp") mitm->SetIcon("romfs:/FileSystem/NSP.png");
                    else if(ext == "nro") mitm->SetIcon("romfs:/FileSystem/NRO.png");
                    else if(ext == "tik") mitm->SetIcon("romfs:/FileSystem/TIK.png");
                    else if(ext == "cert") mitm->SetIcon("romfs:/FileSystem/CERT.png");
                    else if(ext == "nxtheme") mitm->SetIcon("romfs:/FileSystem/NXTheme.png");
                    else if(ext == "nca") mitm->SetIcon("romfs:/FileSystem/NCA.png");
                    else if(ext == "nacp") mitm->SetIcon("romfs:/FileSystem/NACP.png");
                    else if(ext == "jpg") mitm->SetIcon("romfs:/FileSystem/JPEG.png");
                    else mitm->SetIcon("romfs:/FileSystem/File.png");
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
        bool ok = false;
        if(this->gexp->GetPartition() == fs::Partition::SdCard) return true;
        pu::Dialog *dlg = new pu::Dialog("Warning: NAND access", "You are trying to write or delete content within the console's NAND memory.\n\nDeleting or replacing content there can be a risky operation.\nImportant file loss could lead to a bricked NAND, where the console won't boot.\n\nAre you sure you want to perform this operation?");
        dlg->AddOption("Yes");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        if(dlg->GetSelectedIndex() == 0) ok = true;
        return ok;
    }

    void PartitionBrowserLayout::fsItems_Click()
    {
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        std::string fullitm = this->gexp->FullPathFor(itm);
        if(this->gexp->NavigateForward(itm)) this->UpdateElements();
        else if(fs::IsFile(fullitm))
        {
            bool bin = fs::IsFileBinary(fullitm);
            std::string ext = fs::GetExtension(itm);
            std::string msg = "What would you like to do with the selected ";
            if(ext == "nsp") msg += "NSP package";
            else if(ext == "nro") msg += "NRO binary";
            else if(ext == "tik") msg += "ticket file";
            else if(ext == "nxtheme") msg += "Home Menu theme file";
            else if(ext == "nca") msg += "NCA archive";
            else if(ext == "nacp") msg += "NACP data";
            else if(ext == "jpg") msg += "JPEG icon";
            else msg += std::string(bin ? "binary" : "text") + " file";
            msg += "?";
            msg += "\n\nFile size: " + fs::FormatSize(fs::GetFileSize(fullitm));
            pu::Dialog *dlg = new pu::Dialog("File options", msg);
            u32 copt = 5;
            if(ext == "nsp")
            {
                dlg->AddOption("Install");
                copt = 6;
            }
            else if(ext == "nro")
            {
                dlg->AddOption("Launch");
                copt = 6;
            }
            else if(ext == "tik")
            {
                dlg->AddOption("Import");
                copt = 6;
            }
            else if(ext == "nxtheme")
            {
                dlg->AddOption("Install");
                copt = 6;
            }
            else if(ext == "nca")
            {
                dlg->AddOption("Extract");
                copt = 6;
            }
            else if(ext == "nacp")
            {
                dlg->AddOption("View information");
                copt = 6;
            }
            else if(ext == "jpg")
            {
                dlg->AddOption("Replace icon");
                copt = 6;
            }
            else if(!bin)
            {
                dlg->AddOption("View text");
                copt = 6;
            }
            dlg->AddOption("View hex");
            dlg->AddOption("Copy");
            dlg->AddOption("Delete");
            dlg->AddOption("Rename");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            u32 sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == copt)) return;
            if(ext == "nsp")
            {
                switch(sopt)
                {
                    case 0:
                        dlg = new pu::Dialog("Select NSP install location", "Which location would you like to install the selected NSP on?");
                        dlg->AddOption("SD card");
                        dlg->AddOption("Console memory (NAND)");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        u32 sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || ( sopt == 2)) return;
                        Destination dst = Destination::SdCard;
                        if(sopt == 0) dst = Destination::SdCard;
                        else if(sopt == 1) dst = Destination::NAND;
                        dlg = new pu::Dialog("Ignore required firmware version", "Should Goldleaf ignore the required firmware version of the NSP?");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2)) return;
                        bool ignorev = (sopt == 0);
                        std::string fullitm = this->gexp->FullPathFor(itm);
                        std::string nspipt = "@Sdcard:" + fullitm.substr(5);
                        nsp::Installer inst(dst, nspipt, ignorev);
                        InstallerResult irc = inst.GetLatestResult();
                        if(!irc.IsSuccess())
                        {
                            mainapp->GetInstallLayout()->LogError(irc);
                            return;
                        }
                        bool isapp = (inst.GetContentType() == ncm::ContentMetaType::Application);
                        bool hasnacp = inst.HasContent(ncm::ContentType::Control);
                        std::string info = "No control data was found inside the NSP. (control NCA seems to be missing)";
                        if(!isapp) info = "The NSP isn't an application (could be an update, a patch, add-on content...)";
                        if(hasnacp && isapp)
                        {
                            info = "Information about the NSP's control data:\n\n\n";
                            NacpStruct *nacp = inst.GetNACP();
                            NacpLanguageEntry lent;
                            for(u32 i = 0; i < 16; i++)
                            {
                                lent = nacp->lang[i];
                                if((strlen(lent.name) != 0) && (strlen(lent.author) != 0)) break;
                            }
                            info += "Name: ";
                            info += lent.name;
                            info += "\nAuthor: ";
                            info += lent.author;
                            info += "\nVersion: ";
                            info += nacp->version;
                            std::vector<ncm::ContentRecord> ncas = inst.GetRecords();
                            info += "\n\nContents (" + std::to_string(ncas.size()) + "): ";
                            for(u32 i = 0; i < ncas.size(); i++)
                            {
                                ncm::ContentType t = ncas[i].Type;
                                switch(t)
                                {
                                    case ncm::ContentType::Control:
                                        info += "Control";
                                        break;
                                    case ncm::ContentType::Data:
                                        info += "Data";
                                        break;
                                    case ncm::ContentType::DeltaFragment:
                                        info += "Delta fragment";
                                        break;
                                    case ncm::ContentType::LegalInformation:
                                        info += "Legal information";
                                        break;
                                    case ncm::ContentType::Meta:
                                        info += "Meta (CNMT)";
                                        break;
                                    case ncm::ContentType::OfflineHTML:
                                        info += "Offline HTML";
                                        break;
                                    case ncm::ContentType::Program:
                                        info += "Program";
                                        break;
                                }
                                if(i != (ncas.size() - 1)) info += ", ";
                            }
                        }
                        info += "\n\n";
                        if(isapp && inst.HasTicketAndCert())
                        {
                            info += "This NSP has a ticket and it will be installed. Ticket information:\n\n";
                            horizon::TicketData tik = inst.GetTicketData();
                            info += "Title key: " + tik.TitleKey;
                            info += "\nSignature type: ";
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
                            info += "\nKey generation: " + std::to_string(tik.KeyGeneration);
                            switch(tik.KeyGeneration)
                            {
                                case 0:
                                    info += " (1.0.0 - 2.3.0)";
                                    break;
                                case 1:
                                    info += " (3.0.0)";
                                    break;
                                case 2:
                                    info += " (3.0.1 - 3.0.2)";
                                    break;
                                case 3:
                                    info += " (4.0.0 - 4.1.0)";
                                    break;
                                case 4:
                                    info += " (5.0.0 - 5.1.0)";
                                    break;
                                case 5:
                                    info += " (6.0.0 - 6.1.0)";
                                    break;
                                case 6:
                                    info += " (6.2.0)";
                                    break;
                                default:
                                    info += " (unknown supported version?)";
                                    break;
                            }
                        }
                        else if(!inst.HasTicketAndCert()) info += "This NSP doesn't have a ticket. It seems to only have standard crypto.";
                        dlg = new pu::Dialog("Ready to start installing?", info);
                        if(hasnacp) dlg->SetIcon(inst.GetExportedIconPath(), 994, 30);
                        dlg->AddOption("Install");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 1)) return;
                        mainapp->LoadLayout(mainapp->GetInstallLayout());
                        mainapp->GetInstallLayout()->StartInstall(&inst, mainapp->GetSDBrowserLayout(), false, fullitm);
                        this->UpdateElements();
                        break;
                }
            }
            else if(ext == "nro") 
            {
                switch(sopt)
                {
                    case 0:
                        if(IsApplication())
                        {
                            dlg = new pu::Dialog("NRO launch error", "For technical reasons, NRO binaries cannot be launched if Goldleaf is launched as a title.");
                            dlg->AddOption("Ok");
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        dlg = new pu::Dialog("NRO launch confirmation", "The selected NRO binary will be launched. (or attempted to be launched)\nGoldleaf has to be closed to proceed with the launch.");
                        dlg->AddOption("Launch");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        if(!dlg->UserCancelled() || (dlg->GetSelectedIndex() == 0))
                        {
                            envSetNextLoad(fullitm.c_str(), "sdmc:/hbmenu.nro");
                            mainapp->Close();
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
                        std::string tcert = fullitm.substr(0, fullitm.length() - 3) + "cert";
                        if(!fs::Exists(tcert))
                        {
                            dlg = new pu::Dialog("Ticket import error", "To be able to import this ticket, both the *.tik and *.cert files are required.\nYou selected the *.cert one, but the *.tik one couldn't be found.\n\nBoth need to have the same name.");
                            dlg->AddOption("Ok");
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        dlg = new pu::Dialog("Ticket import confirmation", "The selected ticket will be imported.");
                        dlg->AddOption("Import");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        if(dlg->GetSelectedIndex() == 0)
                        {
                            std::ifstream ifs(fullitm, std::ios::binary);
                            ifs.seekg(0, ifs.end);
                            int sztik = ifs.tellg();
                            ifs.seekg(0, ifs.beg);
                            auto btik = std::make_unique<u8[]>(sztik);
                            ifs.read((char*)btik.get(), sztik);
                            ifs.close();
                            ifs = std::ifstream(tcert, std::ios::binary);
                            ifs.seekg(0, ifs.end);
                            int szcert = ifs.tellg();
                            ifs.seekg(0, ifs.beg);
                            auto bcert = std::make_unique<u8[]>(szcert);
                            ifs.read((char*)bcert.get(), szcert);
                            ifs.close();
                            Result rc = es::ImportTicket(btik.get(), sztik, bcert.get(), szcert);
                            if(rc != 0) mainapp->UpdateFooter("An error ocurred trying to install the ticket (error code " + horizon::FormatHex(rc) + ")");
                        }
                        break;
                }
            }
            else if(ext == "nxtheme")
            {
                switch(sopt)
                {
                    case 0:
                        dlg = new pu::Dialog("Select CFW", "Select CFW on which to install the theme.");
                        std::vector<std::string> cfws = GetSdCardCFWNames();
                        for(u32 i = 0; i < cfws.size(); i++) dlg->AddOption(cfws[i]);
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        u32 sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == cfws.size())) return;
                        std::string installdir = "sdmc:/" + GetSdCardCFWs()[sopt];
                        std::vector<u8> data = gleaf::fs::ReadFile(fullitm);
                        std::vector<u8> ddata = gleaf::sarc::YAZ0::Decompress(data);
                        gleaf::sarc::SARC::SarcData sdata = gleaf::sarc::SARC::Unpack(ddata);
                        auto nxth = gleaf::theme::ParseNXThemeFile(sdata);
                        if(nxth.Version == -1)
                        {
                            dlg = new pu::Dialog("Theme install error", "The selected theme file seems to be invalid.");
                            dlg->AddOption("Ok");
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        if(!gleaf::theme::ThemeTargetToName.count(nxth.Target))
                        {
                            dlg = new pu::Dialog("Theme install error", "The target of the selected theme file was not found.");
                            dlg->AddOption("Ok");
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        std::string msg = "Information about the selected theme file:\n\n";
                        msg += "Name: " + nxth.ThemeName;
                        msg += "\nAuthor: " + ((nxth.Author == "") ? "<no author specified>" : nxth.Author);
                        msg += "\nLayout: " + ((nxth.LayoutInfo == "") ? "<no layout information>" : nxth.LayoutInfo);
                        msg += "\nTarget: " + gleaf::theme::ThemeTargetToName[nxth.Target] + " (will patch " + gleaf::theme::ThemeTargetToFileName[nxth.Target] + " file)";
                        msg += "\n\nIf there's another theme installed in the selected CFW, it will be overwritten.\nProceed with the installation?";
                        dlg = new pu::Dialog("Theme information", msg);
                        dlg->AddOption("Install");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 1)) return;
                        mainapp->LoadLayout(mainapp->GetThemeInstallLayout());
                        mainapp->GetThemeInstallLayout()->StartInstall(nxth, sdata, installdir);
                        break;
                }
            }
            else if(ext == "nca")
            {
                switch(sopt)
                {
                    case 0:
                        if(!HasKeyFile())
                        {
                            dlg = new pu::Dialog("NCA extraction error", "External keys are required to extract the selected NCA archive.\nPlace them at \'goldleaf\'.\nSupported names: keys.dat, keys.ini, keys.txt, prod.keys");
                            dlg->AddOption("Ok");
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        dlg = new pu::Dialog("Extract ExeFs?", "Would you like to extract the ExeFs partition of the selected NCA?\n(it will be extracted to \'" + fullitm + ".ExeFs\' directory)\n\n(in case it isn't found, it won't be extracted)");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2)) return;
                        bool xexefs = false;
                        if(sopt == 0) xexefs = true;
                        dlg = new pu::Dialog("Extract RomFs?", "Would you like to extract the RomFs partition of the selected NCA?\n(it will be extracted to \'" + fullitm + ".RomFs\' directory)\n\n(in case it isn't found, it won't be extracted)");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2)) return;
                        bool xromfs = false;
                        if(sopt == 0) xromfs = true;
                        dlg = new pu::Dialog("Extract section 0?", "Would you like to extract the section no. 0 of the selected NCA?\nThis section could be present in CNMT NCAs or in program NCAs.\n(it will be extracted to \'" + fullitm + ".Section0\' directory)\n\n(in case it isn't found, it won't be extracted)");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption("Cancel");
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
                        dlg = new pu::Dialog("NCA extraction", msg);
                        dlg->AddOption("Ok");
                        mainapp->ShowDialog(dlg);
                        this->UpdateElements();
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
                        std::string name = "<unknown name>";
                        std::string author = "<unknown author>";
                        std::string version = std::string(snacp->version);
                        if(lent != NULL)
                        {
                            name = std::string(lent->name);
                            author = std::string(lent->author);
                        }
                        std::string msg = "Information of the title provided by the NACP file:\n\n";
                        msg += "Name: " + name;
                        msg += "\nAuthor: " + author;
                        msg += "\n(data according to the console's actual language)\n";
                        msg += "\nVersion: " + version;
                        msg += "\nAsks for user account? ";
                        u8 uacc = rnacp[0x3025];
                        if(uacc == 0) msg += "No";
                        else if(uacc == 1) msg += "Yes";
                        else if(uacc == 2) msg += "Yes, and requires to have a NSA active";
                        else msg += "<unknown value>";
                        u8 scrc = rnacp[0x3034];
                        msg += "\nSupports taking screenshots? ";
                        if(scrc == 0) msg += "Yes";
                        else if(scrc == 1) msg += "No";
                        else msg += "<unknown value>";
                        u8 vidc = rnacp[0x3035];
                        msg += "\nSupports capturing video? ";
                        if(vidc == 0) msg += "No";
                        else if(vidc == 1) msg += "Yes (manually)";
                        else if(vidc == 2) msg += "Yes";
                        else msg += "<unknown value>";
                        u8 logom = rnacp[0x30f0];
                        msg += "\nLogo type: ";
                        if(logom == 0) msg += "Licensed by Nintendo";
                        else if(logom == 2) msg += "Nintendo";
                        else msg += "<unknown value>";
                        dlg = new pu::Dialog("NACP information", msg);
                        dlg->AddOption("Ok");
                        mainapp->ShowDialog(dlg);
                        break;
                }
            }
            else if(ext == "jpg") 
            {
                switch(sopt)
                {
                    case 0:
                        u128 uid = AskForUser();
                        if(uid == 0) return;
                        dlg = new pu::Dialog("Accounts and custom icons", "Custom icons are one of the most dangerous ways to get banned, almost guaranteeing it.\nAre you sure you want to continue?");
                        dlg->AddOption("Replace icon");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 1)) return;
                        AccountProfile prf;
                        AccountProfileBase pbase;
                        AccountUserData udata;
                        Result rc = accountGetProfile(&prf, uid);
                        rc = accountProfileGet(&prf, &udata, &pbase);
                        auto res = acc::GetProfileEditor(uid);
                        rc = std::get<0>(res);
                        acc::ProfileEditor *pedit = std::get<1>(res);
                        std::vector<u8> vdata = fs::ReadFile(fullitm);
                        rc = pedit->StoreWithImage(&pbase, &udata, vdata.data(), vdata.size());
                        if(rc == 0) mainapp->UpdateFooter(std::string(pbase.username) + "'s icon has been updated.");
                        else mainapp->UpdateFooter("An error ocurred updating the icon: " + horizon::FormatHex(rc));
                        delete pedit;
                        serviceClose(&prf.s);
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
            u32 viewopt = copt - 5;
            u32 copyopt = copt - 4;
            u32 delopt = copt - 3;
            u32 renopt = copt - 2;
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
                    fs::DeleteFile(fullitm);
                    mainapp->UpdateFooter("File deleted: \'" + fullitm + "\'");
                    this->UpdateElements();
                }
            }
            else if(sopt == renopt)
            {
                std::string kbdt = AskForText("Rename file", itm);
                if(kbdt != "")
                {
                    if(kbdt == itm) return;
                    std::string newren = this->gexp->FullPathFor(kbdt);
                    if(fs::IsFile(newren) || fs::IsDirectory(newren)) mainapp->UpdateFooter("A file or directory with this name already exists.");
                    else if(this->WarnNANDWriteAccess())
                    {
                        int rc = rename(fullitm.c_str(), newren.c_str());
                        if(rc) mainapp->UpdateFooter("There was an error trying to rename the file.");
                        else
                        {
                            mainapp->UpdateFooter("A file was renamed.");
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
        if(fs::IsDirectory(fullitm))
        {
            std::string msg = "What would you like to do with the selected directory?";
            msg += "\n\nDirectory size: " + fs::FormatSize(fs::GetDirectorySize(fullitm));
            pu::Dialog *dlg = new pu::Dialog("Directory options", msg);
            dlg->AddOption("Browse");
            dlg->AddOption("Copy");
            dlg->AddOption("Delete");
            dlg->AddOption("Rename");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            u32 sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 4)) return;
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
                        fs::DeleteDirectory(fullitm);
                        mainapp->UpdateFooter("Directory deleted: \'" + fullitm + "\'");
                        this->UpdateElements();
                    }
                    break;
                case 3:
                    std::string kbdt = AskForText("Rename directory", itm);
                    if(kbdt != "")
                    {
                        if(kbdt == itm) return;
                        std::string newren = this->gexp->FullPathFor(kbdt);
                        if(fs::IsFile(newren) || fs::IsDirectory(newren)) mainapp->UpdateFooter("A file or directory with this name already exists.");
                        else if(this->WarnNANDWriteAccess())
                        {
                            int rc = rename(fullitm.c_str(), newren.c_str());
                            if(rc) mainapp->UpdateFooter("There was an error trying to rename the directory.");
                            else mainapp->UpdateFooter("A directory was renamed.");
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
        this->cntText = new pu::element::TextBlock(25, 180, "");
        this->cntText->SetFont(pu::render::LoadFont("romfs:/Consolas.ttf", 25));
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
        this->infoText = new pu::element::TextBlock(400, 300, "Starting copy...");
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
            mainapp->UpdateFooter("Directory copied: '" + NewPath + "'");
        }
        else
        {
            if(fs::IsFile(NewPath))
            {
                pu::Dialog *dlg = new pu::Dialog("File copy", "Another file exists with the same name.\nIf you want to continue, the file will be overwritten.");
                dlg->AddOption("Overwrite");
                dlg->AddOption("Cancel");
                mainapp->ShowDialog(dlg);
                u32 sopt = dlg->GetSelectedIndex();
                if(dlg->UserCancelled() || (sopt == 1))
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
            mainapp->UpdateFooter("File copied: '" + NewPath + "'");
        }
        mainapp->LoadLayout(Prev);
    }

    InstallLayout::InstallLayout() : pu::Layout()
    {
        this->installText = new pu::element::TextBlock(150, 300, "Starting NSP installation...");
        this->installBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void InstallLayout::StartInstall(nsp::Installer *Inst, pu::Layout *Prev, bool Delete, std::string Input)
    {
        if(IsApplication()) appletBeginBlockingHomeButton(0);
        this->installText->SetText("Processing title records...");
        mainapp->UpdateFooter("Installing NSP...");
        mainapp->CallForRender();
        InstallerResult rc = Inst->ProcessRecords();
        if(!rc.IsSuccess())
        {
            this->LogError(rc);
            if(IsApplication()) appletEndBlockingHomeButton();
        }
        this->installText->SetText("Starting to write contents...");
        mainapp->CallForRender();
        if(rc.IsSuccess()) rc = Inst->WriteContents([&](ncm::ContentRecord NCA, u32 Index, u32 Count, int Percentage)
        {
            std::string name = "Writing content \'"  + horizon::GetStringFromNCAId(NCA.NCAId);
            if(NCA.Type == ncm::ContentType::Meta) name += ".cnmt";
            name += ".nca\'... (NCA " + std::to_string(Index + 1) + " of " + std::to_string(Count) + ")";
            this->installText->SetText(name);
            this->installBar->SetProgress((u8)Percentage);
            mainapp->CallForRender();
        });
        if(!rc.IsSuccess())
        {
            this->LogError(rc);
            if(IsApplication()) appletEndBlockingHomeButton();
        }
        Inst->Finalize();
        if(IsApplication()) appletEndBlockingHomeButton();
        if(rc.IsSuccess())
        {
            mainapp->UpdateFooter("The NSP was successfully installed.");
            if(Delete) mainapp->UpdateFooter("The NSP was successfully installed and deleted.");
        }
        else mainapp->UpdateFooter("An error ocurred installing the NSP.");
        if(Delete)
        {
            fs::DeleteFile(Input);
            mainapp->GetSDBrowserLayout()->UpdateElements();
        }
        mainapp->LoadLayout(Prev);
        mainapp->CallForRender();
    }

    void InstallLayout::LogError(InstallerResult Res)
    {
        if(!Res.IsSuccess())
        {
            std::string err = "An error ocurred while installing NSP package:\n\n";
            if(Res.Type == InstallerError::Success) return;
            switch(Res.Type)
            {
                case InstallerError::BadNSP:
                    err += "Failed to read from the NSP file.\nThis error could be caused by various reasons: invalid NSP, minimum firmware mismatch...";
                    break;
                case InstallerError::NSPOpen:
                    err += "Failed to open the NSP file. Does it exist?";
                    break;
                case InstallerError::BadCNMTNCA:
                    err += "Failed to read from the meta NCA (CNMT NCA) within the NSP.";
                    break;
                case InstallerError::CNMTMCAOpen:
                    err += "Failed to open the meta NCA (CNMT NCA) within the NSP.";
                    break;
                case InstallerError::BadCNMT:
                    err += "Failed to read from the meta file (CNMT) within the CNMT NCA.";
                    break;
                case InstallerError::CNMTOpen:
                    err += "Failed to open the meta file (CNMT) within the CNMT NCA.";
                    break;
                case InstallerError::BadControlNCA:
                    err += "Failed to open the control NCA within the NSP.";
                    break;
                case InstallerError::MetaDatabaseOpen:
                    err += "Failed to open content meta database for record processing.";
                    break;
                case InstallerError::MetaDatabaseSet:
                    err += "Failed to set in the content meta database for record processing.";
                    break;
                case InstallerError::MetaDatabaseCommit:
                    err += "Failed to commit on the content meta database for record processing.";
                    break;
                case InstallerError::ContentMetaCount:
                    err += "Failed to count content meta for registered application.";
                    break;
                case InstallerError::ContentMetaList:
                    err += "Failed to list content meta for registered application.";
                    break;
                case InstallerError::RecordPush:
                    err += "Failed to push record for application.";
                    break;
                case InstallerError::InstallBadNCA:
                    err += "Failed to find NCA content to write within the NSP.";
                    break;
                default:
                    err += "<undocumented error>";
                    break;
            }
            err += " (error code " + horizon::FormatHex(Res.Error) + ")";
            pu::Dialog *dlg = new pu::Dialog("NSP installation error", err);
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->UpdateFooter("An error ocurred installing the NSP (error code " + horizon::FormatHex(Res.Error) + ")");
        }
    }

    USBInstallLayout::USBInstallLayout() : pu::Layout()
    {
        this->installText = new pu::element::TextBlock(150, 300, "Starting NSP installation via USB...");
        this->installBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->installBar->SetVisible(false);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void USBInstallLayout::StartUSBConnection()
    {
        this->installText->SetText("Waiting for USB connection...\nPlug this console to a USB-C cable to connect it with a PC and open Goldtree.");
        mainapp->CallForRender();
        while(true)
        {
            hidScanInput();
            if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B)
            {
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                return;
            }
            Result rc = usbDsWaitReady(U64_MAX);
            if(rc == 0) break;
            else if((rc & 0x3fffff) != 0xea01)
            {
                mainapp->UpdateFooter("USB failed to connect. Try again!");
                mainapp->CallForRender();
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                return;
            }
            mainapp->CallForRender();
        }
        this->installText->SetText("USB connection was detected. Open Goldtree to connect it via USB.");
        mainapp->CallForRender();
        usb::Command req = usb::ReadCommand();
        usb::Command fcmd = usb::MakeCommand(usb::CommandId::Finish);
        if(req.MagicOk())
        {
            if(req.IsCommandId(usb::CommandId::ConnectionRequest))
            {
                this->installText->SetText("Connection request was received from a Goldtree PC client.\nSending confirmation and waiting to select a NSP...");
                mainapp->CallForRender();
                usb::Command cmd1 = usb::MakeCommand(usb::CommandId::ConnectionResponse);
                usb::WriteCommand(cmd1);
                req = usb::ReadCommand();
                if(req.MagicOk())
                {
                    if(req.IsCommandId(usb::CommandId::NSPName))
                    {
                        u32 nspnamesize = usb::Read32();
                        std::string nspname = usb::ReadString(nspnamesize);
                        pu::Dialog *dlg = new pu::Dialog("USB install confirmation", "Selected NSP via Goldtree: \'" + nspname + "\'\nWould you like to install this NSP?");
                        dlg->AddOption("Install");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        u32 sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 1))
                        {
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        dlg = new pu::Dialog("Select NSP install location", "Which location would you like to install the selected NSP on?");
                        dlg->AddOption("SD card");
                        dlg->AddOption("Console memory (NAND)");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || ( sopt == 2))
                        {
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        Destination dst = Destination::SdCard;
                        if(sopt == 0) dst = Destination::SdCard;
                        else if(sopt == 1) dst = Destination::NAND;
                        dlg = new pu::Dialog("Ignore required firmware version", "Should Goldleaf ignore the required firmware version of the NSP?");
                        dlg->AddOption("Yes");
                        dlg->AddOption("No");
                        dlg->AddOption("Cancel");
                        mainapp->ShowDialog(dlg);
                        sopt = dlg->GetSelectedIndex();
                        if(dlg->UserCancelled() || (sopt == 2))
                        {
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        
                        bool ignorev = (sopt == 0);
                        usb::Command cmd2 = usb::MakeCommand(usb::CommandId::Start);
                        usb::WriteCommand(cmd2);
                        if(IsApplication()) appletBeginBlockingHomeButton(0);
                        this->installText->SetText("Starting installation...");
                        mainapp->CallForRender();
                        usb::Installer inst(dst, ignorev);
                        InstallerResult rc = inst.GetLatestResult();
                        if(!rc.IsSuccess())
                        {
                            if(IsApplication()) appletEndBlockingHomeButton();
                            mainapp->GetInstallLayout()->LogError(rc);
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        this->installText->SetText("Processing title records...");
                        mainapp->CallForRender();
                        rc = inst.ProcessRecords();
                        if(!rc.IsSuccess())
                        {
                            if(IsApplication()) appletEndBlockingHomeButton();
                            mainapp->GetInstallLayout()->LogError(rc);
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        this->installText->SetText("Starting to write contents...");
                        mainapp->CallForRender();
                        this->installBar->SetVisible(true);
                        rc = inst.ProcessContents([&](std::string Name, u32 Index, u32 Count, int Percentage, double Speed)
                        {
                            std::string name = "Writing content \'"  + Name + "\'... (" + std::to_string(Index + 1) + " of " + std::to_string(Count) + ")";
                            this->installText->SetText(name);
                            this->installBar->SetProgress((u8)Percentage);
                            mainapp->UpdateFooter("Average speed: " + horizon::DoubleToString(Speed) + " MB/s");
                            mainapp->CallForRender();
                        });
                        this->installBar->SetVisible(false);
                        if(IsApplication()) appletEndBlockingHomeButton();
                        if(!rc.IsSuccess())
                        {
                            mainapp->GetInstallLayout()->LogError(rc);
                            usb::WriteCommand(fcmd);
                            mainapp->UnloadMenuData();
                            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
                            return;
                        }
                        inst.Finish();
                        mainapp->UpdateFooter("The NSP was successfully installed via USB. You can close Goldtree now.");
                        mainapp->CallForRender();
                    }
                    else if(req.IsCommandId(usb::CommandId::Finish)) mainapp->UpdateFooter("Goldtree has closed the connection.");
                    else
                    {
                        usb::WriteCommand(fcmd);
                        mainapp->UpdateFooter("An invalid command was received. Are you sure you're using Goldtree?");
                    }
                }
                else
                {
                    usb::WriteCommand(fcmd);
                    mainapp->UpdateFooter("An invalid command was received. Are you sure you're using Goldtree?");
                }
            }
            else if(req.IsCommandId(usb::CommandId::Finish)) mainapp->UpdateFooter("Goldtree has closed the connection.");
            else
            {
                usb::WriteCommand(fcmd);
                mainapp->UpdateFooter("An invalid command was received. Are you sure you're using Goldtree?");
            }
        }
        else
        {
            usb::WriteCommand(fcmd);
            mainapp->UpdateFooter("An invalid command was received. Are you sure you're using Goldtree?");
        }
        mainapp->UnloadMenuData();
        mainapp->LoadLayout(mainapp->GetMainMenuLayout());
    }

    void USBInstallLayout::LogError(InstallerResult Res)
    {
        if(!Res.IsSuccess())
        {
            std::string err = "An error ocurred while installing the NSP via USB:\n\n";
            if(Res.Type == InstallerError::Success) return;
            switch(Res.Type)
            {
                case InstallerError::BadNSP:
                    err += "Failed to read from the NSP file.\nThis error could be caused by various reasons: invalid NSP, minimum firmware mismatch...";
                    break;
                case InstallerError::NSPOpen:
                    err += "Failed to open the NSP file. Does it exist?";
                    break;
                case InstallerError::BadCNMTNCA:
                    err += "Failed to read from the meta NCA (CNMT NCA) within the NSP.";
                    break;
                case InstallerError::CNMTMCAOpen:
                    err += "Failed to open the meta NCA (CNMT NCA) within the NSP.";
                    break;
                case InstallerError::BadCNMT:
                    err += "Failed to read from the meta file (CNMT) within the CNMT NCA.";
                    break;
                case InstallerError::CNMTOpen:
                    err += "Failed to open the meta file (CNMT) within the CNMT NCA.";
                    break;
                case InstallerError::BadControlNCA:
                    err += "Failed to open the control NCA within the NSP.";
                    break;
                case InstallerError::MetaDatabaseOpen:
                    err += "Failed to open content meta database for record processing.";
                    break;
                case InstallerError::MetaDatabaseSet:
                    err += "Failed to set in the content meta database for record processing.";
                    break;
                case InstallerError::MetaDatabaseCommit:
                    err += "Failed to commit on the content meta database for record processing.";
                    break;
                case InstallerError::ContentMetaCount:
                    err += "Failed to count content meta for registered application.";
                    break;
                case InstallerError::ContentMetaList:
                    err += "Failed to list content meta for registered application.";
                    break;
                case InstallerError::RecordPush:
                    err += "Failed to push record for application.";
                    break;
                case InstallerError::InstallBadNCA:
                    err += "Failed to find NCA content to write within the NSP.";
                    break;
                default:
                    err += "<undocumented error>";
                    break;
            }
            err += " (error code " + horizon::FormatHex(Res.Error) + ")";
            pu::Dialog *dlg = new pu::Dialog("NSP via USB installation error", err);
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->UpdateFooter("An error ocurred installing the NSP (error code " + horizon::FormatHex(Res.Error) + ")");
        }
    }

    ThemeInstallLayout::ThemeInstallLayout() : pu::Layout()
    {
        this->infoText = new pu::element::TextBlock(150, 300, "Starting theme installation...");
        this->AddChild(this->infoText);
    }

    void ThemeInstallLayout::StartInstall(gleaf::theme::ThemeFileManifest &NXTheme, gleaf::sarc::SARC::SarcData &SData, std::string CFWPath)
    {
        std::string baseszs = "sdmc:/goldleaf/qlaunch/lyt/" + gleaf::theme::ThemeTargetToFileName[NXTheme.Target];
        if(!gleaf::fs::Exists(baseszs))
        {
            if(!HasKeyFile())
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "To install themes, keys are required to dump qlaunch SZS contents in case they aren't found.\nPlace them at \'goldleaf\'.\nSupported names: keys.dat, keys.ini, keys.txt, prod.keys");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                return;
            }
            this->infoText->SetText("Required qlaunch SZS contents weren't found.\nExtracting them from system's qlaunch NCA...\n(this might take some time)");
            mainapp->CallForRender();
            bool exok = gleaf::horizon::ExportQlaunchRomFs();
            if(!exok)
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "There was an error trying to export qlaunch's files from the console.\nThey are required to proceed with the installation.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
            else
            {
                this->infoText->SetText("Files exported. Processing theme...");
                mainapp->CallForRender();
            }
        }
        auto fdata = gleaf::fs::ReadFile(baseszs);
        auto dfdata = gleaf::sarc::YAZ0::Decompress(fdata);
        gleaf::sarc::SARC::SarcData szstp = gleaf::sarc::SARC::Unpack(dfdata);
        auto ptp = gleaf::theme::DetectSarc(szstp);
        if(ptp.FirmName == "")
        {
            pu::Dialog *dlg = new pu::Dialog("Theme install error", "There was an error trying to determine the patch for the theme.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
            return;
        }
        bool p5x = false;
        this->infoText->SetText("Patch type determined. Applying it...");
        mainapp->CallForRender();
        if((NXTheme.Target == "home") && (ptp.FirmName == "<= 5.X") && NXTheme.UseCommon5X)
        {
            p5x = true;
            std::string cszs = "sdmc:/goldleaf/qlaunch/lyt/common.szs";
            auto c_fdata = gleaf::fs::ReadFile(cszs);
            auto c_dfdata = gleaf::sarc::YAZ0::Decompress(c_fdata);
            gleaf::sarc::SARC::SarcData commonszs = gleaf::sarc::SARC::Unpack(c_dfdata);
            auto pcommon = gleaf::theme::DetectSarc(commonszs);
            auto pres = gleaf::theme::PatchBgLayouts(commonszs, pcommon);
            if(pres != gleaf::lyt::BflytFile::PatchResult::OK)
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "Failed to patch background layout.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
            pres = gleaf::theme::PatchBntx(commonszs, SData.files["image.dds"], pcommon);
            if(pres != gleaf::lyt::BflytFile::PatchResult::OK)
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "Failed to patch BNTX texture.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
            gleaf::fs::CreateDirectory(CFWPath + "/titles");
            gleaf::fs::CreateDirectory(CFWPath + "/titles/" + pcommon.TitleId);
            gleaf::fs::CreateDirectory(CFWPath + "/titles/" + pcommon.TitleId + "/romfs");
            gleaf::fs::CreateDirectory(CFWPath + "/titles/" + pcommon.TitleId + "/romfs/lyt");
            gleaf::fs::CreateFile(CFWPath + "/titles/" + pcommon.TitleId + "/fsmitm.flag");
            auto cpack = gleaf::sarc::SARC::Pack(commonszs);
            auto cydata = gleaf::sarc::YAZ0::Compress(cpack.data, 3, cpack.align);
            gleaf::fs::WriteFile(CFWPath + "/titles/" + pcommon.TitleId + "/romfs/lyt/common.szs", cydata);
        }
        else
        {
            auto pres = gleaf::theme::PatchBgLayouts(szstp, ptp);
            if(pres != gleaf::lyt::BflytFile::PatchResult::OK)
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "Failed to patch background layout.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
            pres = gleaf::theme::PatchBntx(szstp, SData.files["image.dds"], ptp);
            if(pres != gleaf::lyt::BflytFile::PatchResult::OK)
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "Failed to patch BNTX texture.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
        }
        if(SData.files.count("layout.json"))
        {
            p5x = false;
            auto jbin = SData.files["layout.json"];
            std::string jstr(reinterpret_cast<char*>(jbin.data()), jbin.size());
            auto pt = gleaf::lyt::LoadLayout(jstr);
            if(!pt.IsCompatible(szstp))
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "The provided layout was not compatible with the patch to apply.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
            auto pres = gleaf::theme::PatchLayouts(szstp, pt.Files);
            if(pres != gleaf::lyt::BflytFile::PatchResult::OK)
            {
                pu::Dialog *dlg = new pu::Dialog("Theme install error", "Failed to patch layouts.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
                return;
            }
        }
        if(!p5x)
        {
            gleaf::fs::CreateDirectory(CFWPath + "/titles");
            gleaf::fs::CreateDirectory(CFWPath + "/titles/" + ptp.TitleId);
            gleaf::fs::CreateDirectory(CFWPath + "/titles/" + ptp.TitleId + "/romfs");
            gleaf::fs::CreateDirectory(CFWPath + "/titles/" + ptp.TitleId + "/romfs/lyt");
            gleaf::fs::CreateFile(CFWPath + "/titles/" + ptp.TitleId + "/fsmitm.flag");
            auto cpack = gleaf::sarc::SARC::Pack(szstp);
            auto cydata = gleaf::sarc::YAZ0::Compress(cpack.data, 3, cpack.align);
            gleaf::fs::WriteFile(CFWPath + "/titles/" + ptp.TitleId + "/romfs/lyt/" + ptp.szsName, cydata);
        }
        mainapp->UpdateFooter("The Home Menu theme was successfully installed in " + GetCFWName(CFWPath) + ".");
        mainapp->CallForRender();
    }

    TitleManagerLayout::TitleManagerLayout() : pu::Layout()
    {
        this->titlesMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->titlesMenu->SetCooldownEnabled(true);
        this->notTitlesText = new pu::element::TextBlock(450, 400, "No titles were found for this console.");
        this->AddChild(this->notTitlesText);
        this->AddChild(this->titlesMenu);
    }

    void TitleManagerLayout::UpdateElements()
    {
        if(!this->titles.empty()) this->titles.clear();
        this->titles = horizon::GetAllSystemTitles();
        mainapp->LoadMenuHead("Found " + std::to_string(this->titles.size()) + " titles on this console.");
        this->titlesMenu->ClearItems();
        this->titlesMenu->SetCooldownEnabled(true);
        if(this->titles.empty())
        {
            this->notTitlesText->SetVisible(true);
            this->titlesMenu->SetVisible(false);
        }
        else
        {
            this->notTitlesText->SetVisible(false);
            for(u32 i = 0; i < this->titles.size(); i++)
            {
                horizon::Title title = this->titles[i];
                pu::element::MenuItem *itm = new pu::element::MenuItem(title.Name);
                itm->SetIcon(title.GetExportedIconPath());
                itm->AddOnClick(std::bind(&TitleManagerLayout::titles_Click, this));
                this->titlesMenu->AddItem(itm);
            }
            this->titlesMenu->SetSelectedIndex(0);
        }
    }

    void TitleManagerLayout::titles_Click()
    {
        horizon::Title seltit = this->titles[this->titlesMenu->GetSelectedIndex()];
        std::string info = "Information about selected title:\n\n\n";
        info += "Name: " + seltit.Name;
        info += "\nAuthor: " + seltit.Author;
        info += "\nVersion: " + seltit.Version;
        info += "\nInstall location: ";
        switch(seltit.Location)
        {
            case Storage::GameCart:
                info += "Game cart";
                break;
            case Storage::NAND:
                info += "Console memory (NAND)";
                break;
            case Storage::SdCard:
                info += "SD card";
                break;
        }
        info += "\nApplication Id: " + horizon::FormatApplicationId(seltit.ApplicationId);
        info += "\n\nThe JPEG icon and the NACP data can be found at:";
        info += "\n\'sdmc:/goldleaf/title/" + horizon::FormatApplicationId(seltit.ApplicationId) + ".jpg\'";
        info += "\n\'sdmc:/goldleaf/title/" + horizon::FormatApplicationId(seltit.ApplicationId) + ".nacp\'";
        pu::Dialog *dlg = new pu::Dialog("Installed title information", info);
        dlg->SetIcon(seltit.GetExportedIconPath(), 994, 30);
        if(seltit.Location == gleaf::Storage::GameCart)
        {
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            return;
        }
        dlg->AddOption("Title options");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 1)) return;
        else
        {
            dlg = new pu::Dialog("Title options", "What would you like to do with the selected title?");
            dlg->AddOption("Dump NSP");
            dlg->AddOption("Uninstall");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 2)) return;
            else
            {
                if(sopt == 0)
                {
                    dlg = new pu::Dialog("Title dump", "Are you sure you want to dump the selected title?\nTitle dumping can be unstable, so do this at your own risk.\n\nIMPORTANT: DON'T dump +4GB titles if you're using FAT32.");
                    dlg->AddOption("Yes");
                    dlg->AddOption("Cancel");
                    mainapp->ShowDialog(dlg);
                    sopt = dlg->GetSelectedIndex();
                    if(dlg->UserCancelled() || (sopt == 1)) return;
                    else
                    {
                        if(!HasKeyFile())
                        {
                            dlg = new pu::Dialog("Title dump error", "External keys are required to dump a title as a NSP.\nPlace them at \'goldleaf\'.\nSupported names: keys.dat, keys.ini, keys.txt, prod.keys");
                            dlg->AddOption("Ok");
                            mainapp->ShowDialog(dlg);
                            return;
                        }
                        mainapp->LoadLayout(mainapp->GetTitleDumperLayout());
                        mainapp->GetTitleDumperLayout()->StartDump(seltit);
                        mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
                    }
                }
                else if(sopt == 1)
                {
                    dlg = new pu::Dialog("Title uninstall", "Are you sure you want to uninstall the selected title?");
                    dlg->AddOption("Yes");
                    dlg->AddOption("Cancel");
                    mainapp->ShowDialog(dlg);
                    sopt = dlg->GetSelectedIndex();
                    if(dlg->UserCancelled() || (sopt == 1)) return;
                    else
                    {
                        Result rc = ns::DeleteApplicationCompletely(seltit.ApplicationId);
                        std::string resstr = "The title was successfully uninstalled from this console.";
                        if(rc != 0) resstr = "The title was not successfully uninstalled (error code " + std::to_string(rc) + ")";
                        dlg = new pu::Dialog("Title uninstall", resstr);
                        dlg->AddOption("Ok");
                        mainapp->ShowDialog(dlg);
                        if(rc == 0) this->UpdateElements();
                    }
                }
            }
        }
    }

    std::vector<horizon::Title> TitleManagerLayout::GetTitles()
    {
        return this->titles;
    }

    TitleDumperLayout::TitleDumperLayout()
    {
        this->dumpText = new pu::element::TextBlock(300, 300, "Starting NSP dump...");
        this->ncaBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->ncaBar->SetVisible(false);
        this->AddChild(this->dumpText);
        this->AddChild(this->ncaBar);
    }

    void TitleDumperLayout::StartDump(horizon::Title &Target)
    {
        EnsureDirectories();
        this->dumpText->SetText("Starting title dump...");
        mainapp->CallForRender();
        FsStorageId stid = dump::GetApplicationLocation(Target.ApplicationId);
        if(stid == FsStorageId_NandUser)
        {
            FsFileSystem bisfs;
            Result rc = fsOpenBisFileSystem(&bisfs, 30, "");
            if(rc != 0)
            {
                pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error mounting NAND filesystem.");
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
                return;
            }
            fsdevMountDevice("glduser", bisfs);
        }
        std::string fappid = horizon::FormatApplicationId(Target.ApplicationId);
        std::string outdir = "sdmc:/goldleaf/dump/" + fappid;
        fs::CreateDirectory(outdir);
        this->dumpText->SetText("Terminating ES process...");
        mainapp->CallForRender();
        u32 esf = dump::TerminateES();
        this->dumpText->SetText("Accessing ticket data...");
        mainapp->CallForRender();
        std::string tkey = dump::GetTitleKeyData(Target.ApplicationId, true);
        this->dumpText->SetText("Initializing NCM services and retrieving NCAs...");
        mainapp->CallForRender();
        bool istkey = (tkey != "");
        dump::RelaunchES(esf);
        NcmContentStorage cst;
        Result rc = ncmOpenContentStorage(stid, &cst);
        if(rc != 0)
        {
            pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error creating a NCM content storage.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
            return;
        }
        NcmContentMetaDatabase cmdb;
        rc = ncmOpenContentMetaDatabase(stid, &cmdb);
        if(rc != 0)
        {
            pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error creating a NCM meta database.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
            return;
        }
        NcmMetaRecord mrec;
        bool ok = dump::GetMetaRecord(&cmdb, Target.ApplicationId, &mrec);
        if(!ok)
        {
            pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error getting a meta record of the title.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
            return;
        }
        NcmNcaId program;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Program, &program);
        if(!ok)
        {
            pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error getting Program NCA.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
            return;
        }
        std::string sprogram = dump::GetNCAIdPath(&cst, &program);
        NcmNcaId meta;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Meta, &meta);
        if(!ok)
        {
            pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error getting Meta CNMT NCA.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
            return;
        }
        std::string smeta = dump::GetNCAIdPath(&cst, &meta);
        NcmNcaId control;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Control, &control);
        if(!ok)
        {
            pu::Dialog *dlg = new pu::Dialog("Title dump error", "Error getting Control NCA.");
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
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
            xprogram = outdir + "/" + horizon::GetStringFromNCAId(program) + ".nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, program, xprogram, [&](u8 p)
            {
                this->dumpText->SetText("Decrypting and exporting Program NCA...");
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xmeta = outdir + "/" + horizon::GetStringFromNCAId(meta) + ".cnmt.nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, meta, xmeta, [&](u8 p)
            {
                this->dumpText->SetText("Decrypting and exporting Meta CNMT NCA...");
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xcontrol = outdir + "/" + horizon::GetStringFromNCAId(control) + ".nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, control, xcontrol, [&](u8 p)
            {
                this->dumpText->SetText("Decrypting and exporting Control NCA...");
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
                    this->dumpText->SetText("Decrypting and exporting LegalInfo NCA...");
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
                    this->dumpText->SetText("Decrypting and exporting Offline Html NCA...");
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
        }
        if(stid == FsStorageId_NandUser)
        {
            xprogram = "glduser:/Contents/" + xprogram.substr(15);
            std::string txprogram = outdir + "/" + horizon::GetStringFromNCAId(program) + ".nca";
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xprogram, txprogram, [&](u8 p)
            {
                this->dumpText->SetText("Copying Program NCA from NAND memory...");
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
                this->dumpText->SetText("Copying Meta CNMT NCA from NAND memory...");
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
                this->dumpText->SetText("Copying Control NCA from NAND memory...");
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
                    this->dumpText->SetText("Copying LegalInfo NCA from NAND memory...");
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
                    this->dumpText->SetText("Copying Offline Html NCA from NAND memory...");
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xhoff = txhoff;
            }
            fsdevUnmountDevice("glduser");
        }
        std::string info;
        bool istkey2 = dump::HasTitleKeyCrypto(xprogram);
        if(istkey)
        {
            if(istkey2) info = "The title is titlekey encrypted. The NSP will contain a ticket and a cert.";
            else info = "The title doesn't seem to require a titlekey, but the ticket and the cert will be included anyway.";
        }
        else
        {
            if(istkey2) info = "The title requires a titlekey but it was not found.\nThe NSP will be created, but it won't be playable without the required ticket.";
            else info = "The title doesn't need a titlekey, so the rest will be really easy.";
        }
        this->dumpText->SetText(info += "\n\nStarting final NSP build...");
        mainapp->CallForRender();
        std::string fout = "sdmc:/goldleaf/dump/out/" + fappid + ".nsp";
        this->ncaBar->SetVisible(true);
        int qi = nsp::BuildPFS(outdir, fout, [&](u8 p)
        {
            this->dumpText->SetText("Building final NSP from exported contents...");
            this->ncaBar->SetProgress(p);
            mainapp->CallForRender();
        });
        ok = (qi == 0);
        fs::DeleteDirectory("sdmc:/goldleaf/dump/temp");
        fs::DeleteDirectory(outdir);
        if(ok) mainapp->UpdateFooter("Title dumped as NSP: '" + fout + "'");
        else mainapp->UpdateFooter("There was an error dumping the title.");
        pu::Dialog *dlg = new pu::Dialog("Title dump warning", "The dump has finished, but title dumps break ETicket services.\nTicket-related things will be broken in any homebrew.\nA reboot is recommended: dump all the titles you want, then press ZL or ZR to reboot the console anytime.");
        dlg->AddOption("Ok");
        mainapp->ShowDialog(dlg);
        serviceClose(&cst.s);
        serviceClose(&cmdb.s);
    }

    TicketManagerLayout::TicketManagerLayout() : pu::Layout()
    {
        this->ticketsMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->notTicketsText = new pu::element::TextBlock(450, 400, "No tickets were found for this console.");
        this->AddChild(this->notTicketsText);
        this->AddChild(this->ticketsMenu);
    }

    void TicketManagerLayout::UpdateElements()
    {
        if(!this->tickets.empty()) this->tickets.clear();
        this->tickets = horizon::GetAllSystemTickets();
        mainapp->LoadMenuHead("Found " + std::to_string(this->tickets.size()) + " tickets on this console.");
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
            std::vector<horizon::Title> ots = horizon::GetAllSystemTitles();
            for(u32 i = 0; i < this->tickets.size(); i++)
            {
                horizon::Ticket ticket = this->tickets[i];
                u64 tappid = ticket.GetApplicationId();
                std::string tname = "Unknown title (" + horizon::FormatApplicationId(tappid) + ")";
                if(!ots.empty()) for(u32 i = 0; i < ots.size(); i++) if(ots[i].ApplicationId == tappid)
                {
                    tname = ots[i].Name;
                    break;
                }
                pu::element::MenuItem *itm = new pu::element::MenuItem(tname);
                itm->SetIcon("romfs:/Common/Ticket.png");
                itm->AddOnClick(std::bind(&TicketManagerLayout::tickets_Click, this));
                this->ticketsMenu->AddItem(itm);
            }
            this->ticketsMenu->SetSelectedIndex(0);
        }
    }

    void TicketManagerLayout::tickets_Click()
    {
        horizon::Ticket seltick = this->tickets[this->ticketsMenu->GetSelectedIndex()];
        std::string info = "Information about selected ticket:\n\n\n";
        u64 tappid = seltick.GetApplicationId();
        info += "Application Id: " + horizon::FormatApplicationId(tappid);
        info += "\nKey generation: " + std::to_string(tappid);
        pu::Dialog *dlg = new pu::Dialog("Installed ticket information", info);
        dlg->AddOption("Remove ticket");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 1)) return;
        else
        {
            dlg = new pu::Dialog("Ticket remove", "Are you sure you want to remove the selected ticket?");
            dlg->AddOption("Yes");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 1)) return;
            else
            {
                Result rc = es::DeleteTicket(&seltick.RId, sizeof(es::RightsId));
                std::string resstr = "The ticket was successfully removed from this console.";
                if(rc != 0) resstr = "The title was not successfully removed (error code " + std::to_string(rc) + ")";
                else this->UpdateElements();
                mainapp->UpdateFooter(resstr);
            }
        }
    }

    AccountLayout::AccountLayout() : pu::Layout()
    {
        this->optsMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        pu::element::MenuItem *itm = new pu::element::MenuItem("Rename");
        itm->AddOnClick(std::bind(&AccountLayout::optsRename_Click, this));
        this->optsMenu->AddItem(itm);
        pu::element::MenuItem *itm2 = new pu::element::MenuItem("Manage icon");
        itm2->AddOnClick(std::bind(&AccountLayout::optsIcon_Click, this));
        this->optsMenu->AddItem(itm2);
        pu::element::MenuItem *itm3 = new pu::element::MenuItem("Delete account");
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
            mainapp->UpdateFooter("An error ocurred trying to access account data (error " + horizon::FormatHex(rc));
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
        }
        this->pbase = (AccountProfileBase*)malloc(sizeof(AccountProfileBase));
        this->udata = (AccountUserData*)malloc(sizeof(AccountUserData));
        rc = accountProfileGet(&this->prf, this->udata, this->pbase);
        if(rc != 0)
        {
            mainapp->UpdateFooter("An error ocurred trying to access account data (error " + horizon::FormatHex(rc));
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
        }
        mainapp->LoadMenuHead("Loaded user: " + std::string(this->pbase->username));
        auto res = acc::GetProfileEditor(UserId);
        rc = std::get<0>(res);
        if(rc != 0)
        {
            mainapp->UpdateFooter("An error ocurred trying to access account data (error " + horizon::FormatHex(rc));
            mainapp->LoadLayout(mainapp->GetMainMenuLayout());
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
        if(rc == 0) fwrite(img, 0x20000, 1, f);
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
            this->pred->Close();
            this->pred = NULL;
        }
    }

    void AccountLayout::optsRename_Click()
    {
        std::string name = AskForText("Select new account name.", "");
        if(name != "")
        {
            if(name.length() <= 10)
            {
                strcpy(this->pbase->username, name.c_str());
                Result rc = this->pred->Store(this->pbase, this->udata);
                if(rc == 0)
                {
                    mainapp->LoadMenuHead("Loaded user: " + name);
                    mainapp->UpdateFooter("Account was renamed to \'" + name + "\'.");
                }
                else mainapp->UpdateFooter("Error renaming user account: " + horizon::FormatHex(rc));
            }
            else mainapp->UpdateFooter("Error renaming user account. The name was too long.");
        }
    }

    void AccountLayout::optsIcon_Click()
    {
        std::string iconpth = "sdmc:/goldleaf/userdata/" + horizon::FormatHex128(this->uid) + ".jpg";
        pu::Dialog *dlg = new pu::Dialog("Account icon", "If you want to replace the icon with another JPEG, you can to that from the file browsers.\n\nFor the actual icon, it has been exported here:\n\'" + iconpth + "\'");
        dlg->AddOption("Ok");
        mainapp->ShowDialog(dlg);
    }

    void AccountLayout::optsDelete_Click()
    {
        pu::Dialog *dlg = new pu::Dialog("Account delete", "Are you sure you want to delete this account?");
        dlg->AddOption("Delete");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(sopt == 0)
        {
            Result rc = acc::DeleteUser(this->uid);
            if(rc == 0)
            {
                mainapp->UpdateFooter("The account was successfully deleted from this console.");
                this->CleanData();
                mainapp->UnloadMenuData();
                mainapp->LoadLayout(mainapp->GetMainMenuLayout());
            }
            else mainapp->UpdateFooter("An error ocurred attempting to delete the account: " + horizon::FormatHex(rc));
        }
    }

    CFWConfigLayout::CFWConfigLayout() : pu::Layout()
    {
        this->cfwsMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->cfws = GetSdCardCFWs();
        this->cfwnms = GetSdCardCFWNames();
        this->AddChild(this->cfwsMenu);
    }

    void CFWConfigLayout::UpdateElements()
    {
        this->cfwsMenu->ClearItems();
        mainapp->LoadMenuHead("Found " + std::to_string(this->cfws.size()) + " CFWs on the SD card.");
        for(u32 i = 0; i < this->cfws.size(); i++) if(gleaf::fs::IsDirectory("sdmc:/" + this->cfws[i]))
        {
            pu::element::MenuItem *itm = new pu::element::MenuItem(this->cfwnms[i]);
            itm->SetIcon("romfs:/Common/CFW.png");
            itm->AddOnClick(std::bind(&CFWConfigLayout::cfws_Click, this));
            this->cfwsMenu->AddItem(itm);
        }
        this->cfwsMenu->SetSelectedIndex(0);
    }

    void CFWConfigLayout::cfws_Click()
    {
        std::string cfw = this->cfws[this->cfwsMenu->GetSelectedIndex()];
        bool htheme = false;
        std::string msg = this->cfwnms[this->cfwsMenu->GetSelectedIndex()] + " was selected.";
        if((gleaf::fs::IsDirectory("sdmc:/" + cfw + "/titles/0100000000001000/romfs") && gleaf::fs::Exists("sdmc:/" + cfw + "/titles/0100000000001000/fsmitm.flag")) || (gleaf::fs::IsDirectory("sdmc:/" + cfw + "/titles/0100000000001013/romfs") && gleaf::fs::Exists("sdmc:/" + cfw + "/titles/0100000000001013/fsmitm.flag")))
        {
            msg += "\nHome Menu (aka qlaunch) has a LayeredFS RomFs modification in this CFW.\nThis means that it seems to have a custom Home Menu theme.";
            htheme = true;
        }
        else msg += "\nThere are no Home Menu modifications in this CFW, so it doesn't have any theme installed.";
        pu::Dialog *dlg = new pu::Dialog("CFW information", msg);   
        if(htheme)
        {
            dlg->AddOption("Remove modification");
            dlg->AddOption("Cancel");
        }
        else dlg->AddOption("Ok");
        mainapp->ShowDialog(dlg);
        if(htheme)
        {
            u32 sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 1)) return;
            gleaf::fs::DeleteDirectory("sdmc:/" + cfw + "/titles/0100000000001000");
            gleaf::fs::DeleteDirectory("sdmc:/" + cfw + "/titles/0100000000001013");
            ShowRebootShutDownDialog("Modification removed", "Modification was removed successfully.\nNow, you can shut down or reboot to see the applied changes.");
        }
    }

    SystemInfoLayout::SystemInfoLayout() : pu::Layout()
    {
        horizon::FwVersion fwv = horizon::GetFwVersion();
        this->fwText = new pu::element::TextBlock(30, 630, "Firmware: " + fwv.ToString() + " (" + fwv.DisplayName + ")");
        this->sdText = new pu::element::TextBlock(280, 300, "SD card", 35);
        this->sdBar = new pu::element::ProgressBar(220, 345, 300, 30);
        this->sdFreeText = new pu::element::TextBlock(225, 385, "free");
        this->nandText = new pu::element::TextBlock(600, 300, "Console memory (NAND)", 35);
        this->nandBar = new pu::element::ProgressBar(660, 345, 300, 30);
        this->nandFreeText = new pu::element::TextBlock(655, 385, "free");
        this->safeText = new pu::element::TextBlock(105, 480, "NAND (SAFE partition)");
        this->safeBar = new pu::element::ProgressBar(100, 515, 300, 30);
        this->userText = new pu::element::TextBlock(455, 480, "NAND (USER partition)");
        this->userBar = new pu::element::ProgressBar(450, 515, 300, 30);
        this->systemText = new pu::element::TextBlock(805, 480, "NAND (SYSTEM partition)");
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
        this->logoImage = new pu::element::Image(85, 150, "romfs:/Logo.png");
        this->AddChild(this->logoImage);
    }

    MainApplication::MainApplication() : pu::Application()
    {
        this->SetBackgroundColor({ 235, 235, 235, 255 });
        this->preblv = 0;
        this->preisch = false;
        this->pretime = "";
        this->vfirst = true;
        this->baseImage = new pu::element::Image(0, 0, "romfs:/Base.png");
        this->timeText = new pu::element::TextBlock(1124, 20, "00:00:00");
        this->batteryText = new pu::element::TextBlock(1020, 24, "0%", 20);
        this->batteryImage = new pu::element::Image(960, 10, "romfs:/Battery/0.png");
        this->batteryChargeImage = new pu::element::Image(960, 10, "romfs:/Battery/Charge.png");
        this->menuBanner = new pu::element::Image(0, 70, "romfs:/MenuBanner.png");
        this->menuImage = new pu::element::Image(10, 67, "romfs:/Common/SdCard.png");
        this->menuNameText = new pu::element::TextBlock(112, 90, "Name");
        this->menuHeadText = new pu::element::TextBlock(112, 125, "Head", 20);
        this->UnloadMenuData();
        this->footerText = new pu::element::TextBlock(15, 685, "Welcome to Goldleaf! You can install NSPs, import tickets, uninstall titles, remove tickets, browse SD and NAND...", 20);
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
        this->themeInstall = new ThemeInstallLayout();
        this->titleManager = new TitleManagerLayout();
        this->titleManager->SetOnInput(std::bind(&MainApplication::titleManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->titleDump = new TitleDumperLayout();
        this->ticketManager = new TicketManagerLayout();
        this->ticketManager->SetOnInput(std::bind(&MainApplication::ticketManager_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->account = new AccountLayout();
        this->account->SetOnInput(std::bind(&MainApplication::account_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->cfwConfig = new CFWConfigLayout();
        this->cfwConfig->SetOnInput(std::bind(&MainApplication::cfwConfig_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->sysInfo = new SystemInfoLayout();
        this->sysInfo->SetOnInput(std::bind(&MainApplication::sysInfo_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->about = new AboutLayout();
        this->about->SetOnInput(std::bind(&MainApplication::about_Input, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        this->mainMenu->AddChild(this->baseImage);
        this->sdBrowser->AddChild(this->baseImage);
        this->nandBrowser->AddChild(this->baseImage);
        this->fileContent->AddChild(this->baseImage);
        this->copy->AddChild(this->baseImage);
        this->nspInstall->AddChild(this->baseImage);
        this->usbInstall->AddChild(this->baseImage);
        this->themeInstall->AddChild(this->baseImage);
        this->titleManager->AddChild(this->baseImage);
        this->titleDump->AddChild(this->baseImage);
        this->ticketManager->AddChild(this->baseImage);
        this->account->AddChild(this->baseImage);
        this->cfwConfig->AddChild(this->baseImage);
        this->sysInfo->AddChild(this->baseImage);
        this->about->AddChild(this->baseImage);
        this->mainMenu->AddChild(this->timeText);
        this->sdBrowser->AddChild(this->timeText);
        this->nandBrowser->AddChild(this->timeText);
        this->fileContent->AddChild(this->timeText);
        this->copy->AddChild(this->timeText);
        this->nspInstall->AddChild(this->timeText);
        this->usbInstall->AddChild(this->timeText);
        this->themeInstall->AddChild(this->timeText);
        this->titleManager->AddChild(this->timeText);
        this->titleDump->AddChild(this->timeText);
        this->ticketManager->AddChild(this->timeText);
        this->account->AddChild(this->timeText);
        this->cfwConfig->AddChild(this->timeText);
        this->sysInfo->AddChild(this->timeText);
        this->about->AddChild(this->timeText);
        this->mainMenu->AddChild(this->batteryText);
        this->sdBrowser->AddChild(this->batteryText);
        this->nandBrowser->AddChild(this->batteryText);
        this->fileContent->AddChild(this->batteryText);
        this->copy->AddChild(this->batteryText);
        this->nspInstall->AddChild(this->batteryText);
        this->usbInstall->AddChild(this->batteryText);
        this->themeInstall->AddChild(this->batteryText);
        this->titleManager->AddChild(this->batteryText);
        this->titleDump->AddChild(this->batteryText);
        this->ticketManager->AddChild(this->batteryText);
        this->account->AddChild(this->batteryText);
        this->cfwConfig->AddChild(this->batteryText);
        this->sysInfo->AddChild(this->batteryText);
        this->about->AddChild(this->batteryText);
        this->mainMenu->AddChild(this->batteryImage);
        this->sdBrowser->AddChild(this->batteryImage);
        this->nandBrowser->AddChild(this->batteryImage);
        this->fileContent->AddChild(this->batteryImage);
        this->copy->AddChild(this->batteryImage);
        this->nspInstall->AddChild(this->batteryImage);
        this->usbInstall->AddChild(this->batteryImage);
        this->themeInstall->AddChild(this->batteryImage);
        this->titleManager->AddChild(this->batteryImage);
        this->titleDump->AddChild(this->batteryImage);
        this->ticketManager->AddChild(this->batteryImage);
        this->account->AddChild(this->batteryImage);
        this->cfwConfig->AddChild(this->batteryImage);
        this->sysInfo->AddChild(this->batteryImage);
        this->about->AddChild(this->batteryImage);
        this->mainMenu->AddChild(this->batteryChargeImage);
        this->sdBrowser->AddChild(this->batteryChargeImage);
        this->nandBrowser->AddChild(this->batteryChargeImage);
        this->fileContent->AddChild(this->batteryChargeImage);
        this->copy->AddChild(this->batteryChargeImage);
        this->nspInstall->AddChild(this->batteryChargeImage);
        this->usbInstall->AddChild(this->batteryChargeImage);
        this->themeInstall->AddChild(this->batteryChargeImage);
        this->titleManager->AddChild(this->batteryChargeImage);
        this->titleDump->AddChild(this->batteryChargeImage);
        this->ticketManager->AddChild(this->batteryChargeImage);
        this->account->AddChild(this->batteryChargeImage);
        this->cfwConfig->AddChild(this->batteryChargeImage);
        this->sysInfo->AddChild(this->batteryChargeImage);
        this->about->AddChild(this->batteryChargeImage);
        this->mainMenu->AddChild(this->menuImage);
        this->sdBrowser->AddChild(this->menuImage);
        this->nandBrowser->AddChild(this->menuImage);
        this->fileContent->AddChild(this->menuImage);
        this->copy->AddChild(this->menuImage);
        this->nspInstall->AddChild(this->menuImage);
        this->usbInstall->AddChild(this->menuImage);
        this->themeInstall->AddChild(this->menuImage);
        this->titleManager->AddChild(this->menuImage);
        this->titleDump->AddChild(this->menuImage);
        this->ticketManager->AddChild(this->menuImage);
        this->account->AddChild(this->menuImage);
        this->cfwConfig->AddChild(this->menuImage);
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
        this->themeInstall->AddChild(this->menuNameText);
        this->titleManager->AddChild(this->menuNameText);
        this->titleDump->AddChild(this->menuNameText);
        this->ticketManager->AddChild(this->menuNameText);
        this->account->AddChild(this->menuNameText);
        this->cfwConfig->AddChild(this->menuNameText);
        this->sysInfo->AddChild(this->menuNameText);
        this->about->AddChild(this->menuNameText);
        this->mainMenu->AddChild(this->menuHeadText);
        this->sdBrowser->AddChild(this->menuHeadText);
        this->nandBrowser->AddChild(this->menuHeadText);
        this->fileContent->AddChild(this->menuHeadText);
        this->copy->AddChild(this->menuHeadText);
        this->nspInstall->AddChild(this->menuHeadText);
        this->usbInstall->AddChild(this->menuHeadText);
        this->themeInstall->AddChild(this->menuHeadText);
        this->titleManager->AddChild(this->menuHeadText);
        this->titleDump->AddChild(this->menuHeadText);
        this->ticketManager->AddChild(this->menuHeadText);
        this->account->AddChild(this->menuHeadText);
        this->cfwConfig->AddChild(this->menuHeadText);
        this->sysInfo->AddChild(this->menuHeadText);
        this->about->AddChild(this->menuHeadText);
        this->mainMenu->AddChild(this->footerText);
        this->sdBrowser->AddChild(this->footerText);
        this->nandBrowser->AddChild(this->footerText);
        this->fileContent->AddChild(this->footerText);
        this->copy->AddChild(this->footerText);
        this->nspInstall->AddChild(this->footerText);
        this->usbInstall->AddChild(this->footerText);
        this->themeInstall->AddChild(this->footerText);
        this->titleManager->AddChild(this->footerText);
        this->titleDump->AddChild(this->footerText);
        this->ticketManager->AddChild(this->footerText);
        this->account->AddChild(this->footerText);
        this->cfwConfig->AddChild(this->footerText);
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
            if(blv <= 10) this->batteryImage->SetImage("romfs:/Battery/0.png");
            else if((blv > 10) && (blv <= 20)) this->batteryImage->SetImage("romfs:/Battery/10.png");
            else if((blv > 20) && (blv <= 30)) this->batteryImage->SetImage("romfs:/Battery/20.png");
            else if((blv > 30) && (blv <= 40)) this->batteryImage->SetImage("romfs:/Battery/30.png");
            else if((blv > 40) && (blv <= 50)) this->batteryImage->SetImage("romfs:/Battery/40.png");
            else if((blv > 50) && (blv <= 60)) this->batteryImage->SetImage("romfs:/Battery/50.png");
            else if((blv > 60) && (blv <= 70)) this->batteryImage->SetImage("romfs:/Battery/60.png");
            else if((blv > 70) && (blv <= 80)) this->batteryImage->SetImage("romfs:/Battery/70.png");
            else if((blv > 80) && (blv <= 90)) this->batteryImage->SetImage("romfs:/Battery/80.png");
            else if((blv > 90) && (blv < 100)) this->batteryImage->SetImage("romfs:/Battery/90.png");
            else if(blv == 100) this->batteryImage->SetImage("romfs:/Battery/100.png");
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
    }

    void MainApplication::LoadMenuData(std::string Name, std::string ImageName, std::string TempHead)
    {
        if(this->menuImage != NULL)
        {
            this->menuImage->SetVisible(true);
            this->menuImage->SetImage("romfs:/Common/" + ImageName + ".png");
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
                pu::Dialog *dlg = new pu::Dialog("Clipboard paste", "Current clipboard path:\n\'" + clipboard + "\'\n\nDo you want to copy clipboard contents into this directory?");
                if(cdir) dlg->SetIcon("romfs:/FileSystem/Directory.png", 1150, 30);
                else
                {
                    std::string ext = fs::GetExtension(clipboard);
                    if(ext == "nsp") dlg->SetIcon("romfs:/FileSystem/NSP.png", 1150, 30);
                    else if(ext == "nro") dlg->SetIcon("romfs:/FileSystem/NRO.png", 1150, 30);
                    else if(ext == "tik") dlg->SetIcon("romfs:/FileSystem/TIK.png", 1150, 30);
                    else if(ext == "cert") dlg->SetIcon("romfs:/FileSystem/CERT.png", 1150, 30);
                    else if(ext == "nca") dlg->SetIcon("romfs:/FileSystem/NCA.png", 1150, 30);
                    else if(ext == "nxtheme") dlg->SetIcon("romfs:/FileSystem/NXTheme.png", 1150, 30);
                    else dlg->SetIcon("romfs:/FileSystem/File.png", 1150, 30);
                }
                dlg->AddOption("Yes");
                dlg->AddOption("Cancel");
                mainapp->ShowDialog(dlg);
                u32 sopt = dlg->GetSelectedIndex();
                if(sopt == 0)
                {
                    std::string cname = fs::GetFileName(clipboard);
                    mainapp->LoadLayout(mainapp->GetCopyLayout());
                    mainapp->GetCopyLayout()->StartCopy(clipboard, this->sdBrowser->GetExplorer()->FullPathFor(cname), cdir, this->sdBrowser);
                    this->sdBrowser->UpdateElements();
                    clipboard = "";
                }
            }
            else mainapp->UpdateFooter("Clipboard is not selected.");
        }
        else if(Down & KEY_L)
        {
            std::string cfile = AskForText("Create file", "");
            if(cfile != "")
            {
                std::string ffile = this->sdBrowser->GetExplorer()->FullPathFor(cfile);
                if(fs::IsFile(ffile) || fs::IsDirectory(ffile)) mainapp->UpdateFooter("A file or directory with this name already exists.");
                else
                {
                    fs::CreateFile(ffile);
                    mainapp->UpdateFooter("A file was created: \'" + ffile + "\'");
                    this->sdBrowser->UpdateElements();
                }
            }
        }
        else if(Down & KEY_R)
        {
            std::string cdir = AskForText("Create directory", "");
            if(cdir != "")
            {
                std::string fdir = this->sdBrowser->GetExplorer()->FullPathFor(cdir);
                if(fs::IsFile(fdir) || fs::IsDirectory(fdir)) mainapp->UpdateFooter("A file or directory with this name already exists.");
                else
                {
                    fs::CreateDirectory(fdir);
                    mainapp->UpdateFooter("A directory was created: \'" + fdir + "\'");
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
                if(!cdir && !fs::IsFile(clipboard)) return;
                pu::Dialog *dlg = new pu::Dialog("Clipboard paste", "Current clipboard path:\n\'" + clipboard + "\'\n\nDo you want to copy clipboard contents into this directory?");
                if(cdir) dlg->SetIcon("romfs:/FileSystem/Directory.png", 1150, 30);
                else
                {
                    std::string ext = fs::GetExtension(clipboard);
                    if(ext == "nsp") dlg->SetIcon("romfs:/FileSystem/NSP.png", 1150, 30);
                    else if(ext == "nro") dlg->SetIcon("romfs:/FileSystem/NRO.png", 1150, 30);
                    else if(ext == "tik") dlg->SetIcon("romfs:/FileSystem/TIK.png", 1150, 30);
                    else if(ext == "cert") dlg->SetIcon("romfs:/FileSystem/CERT.png", 1150, 30);
                    else if(ext == "nca") dlg->SetIcon("romfs:/FileSystem/NCA.png", 1150, 30);
                    else if(ext == "nxtheme") dlg->SetIcon("romfs:/FileSystem/NXTheme.png", 1150, 30);
                    else dlg->SetIcon("romfs:/FileSystem/File.png", 1150, 30);
                }
                dlg->AddOption("Yes");
                dlg->AddOption("Cancel");
                mainapp->ShowDialog(dlg);
                u32 sopt = dlg->GetSelectedIndex();
                if((sopt == 0) && this->nandBrowser->WarnNANDWriteAccess())
                {
                    std::string cname = fs::GetFileName(clipboard);
                    mainapp->LoadLayout(mainapp->GetCopyLayout());
                    mainapp->GetCopyLayout()->StartCopy(clipboard, this->nandBrowser->GetExplorer()->FullPathFor(cname), cdir, this->nandBrowser);
                    this->nandBrowser->UpdateElements();
                    clipboard = "";
                }
            }
            else mainapp->UpdateFooter("Couldn't paste clipboard path. There is nothing selected to paste.");
        }
        else if(Down & KEY_L)
        {
            std::string cfile = AskForText("Create file", "");
            if(cfile != "")
            {
                std::string ffile = this->nandBrowser->GetExplorer()->FullPathFor(cfile);
                if(fs::IsFile(ffile) || fs::IsDirectory(ffile)) mainapp->UpdateFooter("A file or directory with this name already exists.");
                else if(this->nandBrowser->WarnNANDWriteAccess())
                {
                    fs::CreateFile(ffile);
                    mainapp->UpdateFooter("A file was created: \'" + ffile + "\'");
                    this->nandBrowser->UpdateElements();
                }
            }
        }
        else if(Down & KEY_R)
        {
            std::string cdir = AskForText("Create directory", "");
            if(cdir != "")
            {
                std::string fdir = this->nandBrowser->GetExplorer()->FullPathFor(cdir);
                if(fs::IsFile(fdir) || fs::IsDirectory(fdir)) mainapp->UpdateFooter("A file or directory with this name already exists.");
                else if(this->nandBrowser->WarnNANDWriteAccess())
                {
                    fs::CreateDirectory(fdir);
                    mainapp->UpdateFooter("A directory was created: \'" + fdir + "\'");
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

    void MainApplication::titleManager_Input(u64 Down, u64 Up, u64 Held)
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

    void MainApplication::cfwConfig_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down & KEY_B)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::sysInfo_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::about_Input(u64 Down, u64 Up, u64 Held)
    {
        if(Down)
        {
            this->UnloadMenuData();
            this->LoadLayout(this->mainMenu);
        }
    }

    void MainApplication::OnInput(u64 Down, u64 Up, u64 Held)
    {
        if((Down & KEY_PLUS) || (Down & KEY_MINUS))
        {
            if(!IsApplication()) this->Close();
        }
        else if((Down & KEY_ZL) || (Down & KEY_ZR)) ShowRebootShutDownDialog("Reboot or shut down console", "You can shut down or reboot your console here.");
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

    ThemeInstallLayout *MainApplication::GetThemeInstallLayout()
    {
        return this->themeInstall;
    }

    TitleManagerLayout *MainApplication::GetTitleManagerLayout()
    {
        return this->titleManager;
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

    CFWConfigLayout *MainApplication::GetCFWConfigLayout()
    {
        return this->cfwConfig;
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
        clipboard = Path;
        mainapp->UpdateFooter("Clipboard path was updated: \'" + Path + "\'");
    }

    void ShowRebootShutDownDialog(std::string Title, std::string Message)
    {
        pu::Dialog *dlg = new pu::Dialog(Title, Message + "\nYou can always hold Vol- or Vol+ and reboot the console to automatically enter RCM mode.");
        dlg->AddOption("Shut down");
        dlg->AddOption("Reboot");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 2)) return;
        else switch(sopt)
        {
            case 0:
                bpcInitialize();
                bpcShutdownSystem();
                bpcExit();
                break;
            case 1:
                bpcInitialize();
                bpcRebootSystem();
                bpcExit();
                break;
        }
    }

    std::string AskForText(std::string Guide, std::string Initial)
    {
        std::string out = "";
        char tmpout[FS_MAX_PATH] = { 0 };
        SwkbdConfig kbd;
        Result rc = swkbdCreate(&kbd, 0);
        if(rc == 0)
        {
            swkbdConfigMakePresetDefault(&kbd);
            if(Guide != "") swkbdConfigSetGuideText(&kbd, Guide.c_str());
            if(Initial != "") swkbdConfigSetInitialText(&kbd, Initial.c_str());
            rc = swkbdShow(&kbd, tmpout, sizeof(tmpout));
            if(rc == 0) out = std::string(tmpout);
        }
        swkbdClose(&kbd);
        return out;
    }

    u128 AskForUser()
    {
        AppletHolder aph;
        AppletStorage hast1;
        LibAppletArgs args;
        appletCreateLibraryApplet(&aph, AppletId_playerSelect, LibAppletMode_AllForeground);
        libappletArgsCreate(&args, 0);
        libappletArgsPush(&args, &aph);
        appletCreateStorage(&hast1, 0xa0);
        u8 indata[0xa0] = { 0 };
        appletStorageWrite(&hast1, 0, indata, 0xa0);
        appletHolderPushInData(&aph, &hast1);
        appletHolderStart(&aph);
        appletStorageClose(&hast1);
        while(appletHolderWaitInteractiveOut(&aph));
        appletHolderJoin(&aph);
        AppletStorage ost;
        appletHolderPopOutData(&aph, &ost);
        u8 out[24] = { 0 };
        appletStorageRead(&ost, 0, out, 24);
        appletStorageClose(&ost);
        appletHolderClose(&aph);
        return *(u128*)&out[8];
    }

    void SetMainApplication(MainApplication *MainApp)
    {
        mainapp = MainApp;
    }
}
