#include <gleaf/ui/MainApplication.hpp>

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
        this->nandMenuItem = new pu::element::MenuItem("Browse system NAND");
        this->nandMenuItem->SetIcon("romfs:/Common/NAND.png");
        this->nandMenuItem->AddOnClick(std::bind(&MainMenuLayout::nandMenuItem_Click, this));
        this->remoteMenuItem = new pu::element::MenuItem("Remote installation");
        this->remoteMenuItem->SetIcon("romfs:/Common/USB.png");
        this->titleMenuItem = new pu::element::MenuItem("Manage installed titles");
        this->titleMenuItem->SetIcon("romfs:/Common/Storage.png");
        this->titleMenuItem->AddOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->ticketMenuItem = new pu::element::MenuItem("Manage installed tickets");
        this->ticketMenuItem->SetIcon("romfs:/Common/Ticket.png");
        this->ticketMenuItem->AddOnClick(std::bind(&MainMenuLayout::ticketMenuItem_Click, this));
        this->sysinfoMenuItem = new pu::element::MenuItem("Show system information");
        this->sysinfoMenuItem->SetIcon("romfs:/Common/Settings.png");
        this->sysinfoMenuItem->AddOnClick(std::bind(&MainMenuLayout::sysinfoMenuItem_Click, this));
        this->aboutMenuItem = new pu::element::MenuItem("About this application");
        this->aboutMenuItem->SetIcon("romfs:/Common/Info.png");
        this->optionMenu->AddItem(this->sdcardMenuItem);
        this->optionMenu->AddItem(this->nandMenuItem);
        this->optionMenu->AddItem(this->remoteMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->ticketMenuItem);
        this->optionMenu->AddItem(this->sysinfoMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->AddChild(this->optionMenu);
    }

    void MainMenuLayout::optionMenu_SelectionChanged()
    {
        std::string info = "Welcome to Goldleaf. You can install NSPs, import tickets, uninstall titles, remove tickets, browse SD and NAND...";
        pu::element::MenuItem *isel = this->optionMenu->GetSelectedItem();
        if(isel == this->sdcardMenuItem) info = "Browse the SD card's files and folders: install NSPs, launch NROs, install tickets... (and copy, paste and delete)";
        else if(isel == this->nandMenuItem) info = "Browse the NAND's files and folders. (and copy, paste and delete)";
        else if(isel == this->remoteMenuItem) info = "Install NSPs remotely, via network or USB communications.";
        else if(isel == this->titleMenuItem) info = "Browse currently installed titles. You can view their information and uninstall them.";
        else if(isel == this->ticketMenuItem) info = "Browse currently installed tickets. You can view their information and remove them.";
        else if(isel == this->sysinfoMenuItem) info = "Display information about this Nintendo Switch: current firmware and used space in NAND and SD card and firmware version.";
        else if(isel == this->aboutMenuItem) info = "Display information about Goldleaf: author, version, credits...";
        mainapp->UpdateFooter(info);
    }

    void MainMenuLayout::sdcardMenuItem_Click()
    {
        mainapp->GetSDBrowserLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
    }

    void MainMenuLayout::nandMenuItem_Click()
    {
        pu::Dialog *dlg = new pu::Dialog("Select NAND partition", "Select NAND partition to explore via Goldleaf.", pu::draw::Font::NintendoStandard);
        dlg->AddOption("Safe");
        dlg->AddOption("System");
        dlg->AddOption("User");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 3)) return;
        else if(sopt == 0) mainapp->GetNANDBrowserLayout()->ChangePartition(gleaf::fs::Partition::NANDSafe);
        else if(sopt == 1) mainapp->GetNANDBrowserLayout()->ChangePartition(gleaf::fs::Partition::NANDSystem);
        else if(sopt == 2) mainapp->GetNANDBrowserLayout()->ChangePartition(gleaf::fs::Partition::NANDUser);
        mainapp->LoadLayout(mainapp->GetNANDBrowserLayout());
    }

    void MainMenuLayout::titleMenuItem_Click()
    {
        mainapp->GetTitleManagerLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetTitleManagerLayout());
    }

    void MainMenuLayout::ticketMenuItem_Click()
    {
        mainapp->GetTicketManagerLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetTicketManagerLayout());
        pu::Dialog *dlg = new pu::Dialog("Removing tickets", "Removing tickets can be dangerous.\nIf tickets from installed apps get removed, the title won't probably work.", pu::draw::Font::NintendoStandard);
        dlg->AddOption("Ok");
        mainapp->ShowDialog(dlg);
    }

    void MainMenuLayout::sysinfoMenuItem_Click()
    {
        mainapp->GetSystemInfoLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSystemInfoLayout());
    }

    PartitionBrowserLayout::PartitionBrowserLayout(gleaf::fs::Partition Partition) : pu::Layout()
    {
        this->gexp = new gleaf::fs::Explorer(Partition);
        this->browseMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->dirEmptyText = new pu::element::TextBlock(450, 400, "Directory is empty.");
        this->AddChild(this->browseMenu);
        this->AddChild(this->dirEmptyText);
    }

    void PartitionBrowserLayout::ChangePartition(gleaf::fs::Partition Partition)
    {
        this->gexp->MovePartition(Partition);
        this->UpdateElements();
    }

    void PartitionBrowserLayout::UpdateElements()
    {
        std::vector<std::string> elems = this->gexp->GetContents();
        this->browseMenu->ClearItems();
        if(elems.empty())
        {
            this->browseMenu->SetVisible(false);
            this->dirEmptyText->SetVisible(true);
        }
        else
        {
            this->browseMenu->SetVisible(true);
            this->dirEmptyText->SetVisible(false);
            for(u32 i = 0; i < elems.size(); i++)
            {
                std::string itm = elems[i];
                bool isdir = gleaf::fs::IsDirectory(this->gexp->FullPathFor(itm));
                pu::element::MenuItem *mitm = new pu::element::MenuItem(itm);
                if(isdir) mitm->SetIcon("romfs:/FileSystem/Directory.png");
                else
                {
                    std::string ext = gleaf::fs::GetExtension(itm);
                    if(ext == "nsp") mitm->SetIcon("romfs:/FileSystem/NSP.png");
                    else if(ext == "nro") mitm->SetIcon("romfs:/FileSystem/NRO.png");
                    else if(ext == "tik") mitm->SetIcon("romfs:/FileSystem/TIK.png");
                    else if(ext == "cert") mitm->SetIcon("romfs:/FileSystem/CERT.png");
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
        if(this->gexp->GetPartition() == gleaf::fs::Partition::SdCard) return true;
        pu::Dialog *dlg = new pu::Dialog("Warning: NAND access", "You are trying to write or delete content within the console's NAND memory.\n\nDeleting or replacing content there can be a risky operation.\nImportant file loss could lead to a bricked NAND, where the console won't boot.\n\nAre you sure you want to perform this operation?", pu::draw::Font::NintendoStandard);
        dlg->AddOption("Yes");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        return (sopt == 0);
    }

    void PartitionBrowserLayout::fsItems_Click()
    {
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        std::string fullitm = this->gexp->FullPathFor(itm);
        if(this->gexp->NavigateForward(itm)) this->UpdateElements();
        else if(gleaf::fs::IsFile(fullitm))
        {
            std::string ext = gleaf::fs::GetExtension(itm);
            std::string msg = "What would you like to do with the selected ";
            if(ext == "nsp") msg += "NSP file";
            else if(ext == "nro") msg += "NRO binary";
            else if(ext == "tik") msg += "ticket file";
            msg += "?";
            pu::Dialog *dlg = new pu::Dialog("File options", msg, pu::draw::Font::NintendoStandard);
            u32 copt = 2;
            if(ext == "nsp")
            {
                dlg->AddOption("Install");
                dlg->AddOption("Install and delete");
                copt = 4;
            }
            else if(ext == "nro")
            {
                dlg->AddOption("Launch");
                copt = 3;
            }
            else if(ext == "tik")
            {
                dlg->AddOption("Import");
                copt = 3;
            }
            dlg->AddOption("Copy");
            dlg->AddOption("Delete");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            u32 sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == copt)) return;
            if(ext == "nsp") switch(sopt)
            {
                case 0:
                case 1:
                    dlg = new pu::Dialog("Select NSP install location", "Which location would you like to install the selected NSP on?", pu::draw::Font::NintendoStandard);
                    dlg->AddOption("SD card");
                    dlg->AddOption("NAND (console memory)");
                    dlg->AddOption("Cancel");
                    mainapp->ShowDialog(dlg);
                    u32 sopt = dlg->GetSelectedIndex();
                    if(dlg->UserCancelled() || ( sopt == 2)) return;
                    gleaf::Destination dst = gleaf::Destination::SdCard;
                    if(sopt == 0) dst = gleaf::Destination::SdCard;
                    else if(sopt == 1) dst = gleaf::Destination::NAND;
                    dlg = new pu::Dialog("Ignore required firmware version", "Should Goldleaf ignore the required firmware version of the NSP?", pu::draw::Font::NintendoStandard);
                    dlg->AddOption("Yes");
                    dlg->AddOption("No");
                    dlg->AddOption("Cancel");
                    mainapp->ShowDialog(dlg);
                    sopt = dlg->GetSelectedIndex();
                    if(dlg->UserCancelled() || (sopt == 2)) return;
                    bool ignorev = (sopt == 0);
                    std::string fullitm = this->gexp->FullPathFor(itm);
                    std::string nspipt = "@Sdcard:" + fullitm.substr(5);
                    gleaf::nsp::Installer inst(dst, nspipt, ignorev);
                    gleaf::nsp::InstallerResult irc = inst.GetLatestResult();
                    if(!irc.IsSuccess())
                    {
                        mainapp->GetNSPInstallLayout()->LogError(irc);
                        return;
                    }
                    bool hasnacp = inst.HasContent(gleaf::ncm::ContentType::Control);
                    std::string info = "No control data was found inside the NSP. (control NCA seems to be missing)";
                    if(hasnacp)
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
                        std::vector<gleaf::ncm::ContentRecord> ncas = inst.GetRecords();
                        info += "\n\nContents (" + std::to_string(ncas.size()) + "): ";
                        for(u32 i = 0; i < ncas.size(); i++)
                        {
                            gleaf::ncm::ContentType t = ncas[i].Type;
                            switch(t)
                            {
                                case gleaf::ncm::ContentType::Control:
                                    info += "Control";
                                    break;
                                case gleaf::ncm::ContentType::Data:
                                    info += "Data";
                                    break;
                                case gleaf::ncm::ContentType::DeltaFragment:
                                    info += "Delta fragment";
                                    break;
                                case gleaf::ncm::ContentType::LegalInformation:
                                    info += "Legal information";
                                    break;
                                case gleaf::ncm::ContentType::Meta:
                                    info += "Meta (CNMT)";
                                    break;
                                case gleaf::ncm::ContentType::OfflineHTML:
                                    info += "Offline HTML";
                                    break;
                                case gleaf::ncm::ContentType::Program:
                                    info += "Program";
                                    break;
                            }
                            if(i != (ncas.size() - 1)) info += ", ";
                        }
                    }
                    info += "\n\n";
                    if(inst.HasTicketAndCert())
                    {
                        info += "This NSP has a ticket and it will be installed. Ticket information:\n\n";
                        gleaf::horizon::TicketData tik = inst.GetTicketData();
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
                                info += " (unknown supported versions?)";
                                break;
                        }
                    }
                    else info += "This NSP doesn't have a ticket. It seems to only have standard crypto.";
                    dlg = new pu::Dialog("Ready to start installing?", info, pu::draw::Font::NintendoStandard);
                    if(hasnacp)
                    {
                        pu::element::Image *img = new pu::element::Image(994, 30, inst.GetExportedIconPath());
                        dlg->SetIcon(img);
                    }
                    dlg->AddOption("Install");
                    dlg->AddOption("Cancel");
                    mainapp->ShowDialog(dlg);
                    sopt = dlg->GetSelectedIndex();
                    if(dlg->UserCancelled() || (sopt == 1)) return;
                    mainapp->LoadLayout(mainapp->GetNSPInstallLayout());
                    mainapp->GetNSPInstallLayout()->StartInstall(&inst, mainapp->GetSDBrowserLayout(), (sopt == 1));
                    if(sopt == 1)
                    {
                        gleaf::fs::DeleteFile(fullitm);
                        this->UpdateElements();
                    }
                    break;
                case 2:
                    UpdateClipboard(fullitm);
                    this->UpdateElements();
                    break;
                case 3:
                    gleaf::fs::DeleteFile(fullitm);
                    mainapp->UpdateFooter("File deleted: \'" + gleaf::fs::GetPathWithoutRoot(fullitm) + "\'.");
                    if(this->WarnNANDWriteAccess()) this->UpdateElements();
                    break;
            }
            else if(ext == "nro") switch(sopt)
            {
                case 0:
                    dlg = new pu::Dialog("NRO launch confirmation", "The selected NRO binary will be launched. (or attempted to be launched)\nGoldleaf has to be closed to proceed with the launch.", pu::draw::Font::NintendoStandard);
                    dlg->AddOption("Launch");
                    dlg->AddOption("Cancel");
                    mainapp->ShowDialog(dlg);
                    if(dlg->GetSelectedIndex() == 0)
                    {
                        envSetNextLoad(fullitm.c_str(), "sdmc:/hbmenu.nro");
                        mainapp->Close();
                    }
                    break;
                case 2:
                    UpdateClipboard(fullitm);
                    this->UpdateElements();
                    break;
                case 3:
                    gleaf::fs::DeleteFile(fullitm);
                    mainapp->UpdateFooter("File deleted: \'" + gleaf::fs::GetPathWithoutRoot(fullitm) + "\'.");
                    if(this->WarnNANDWriteAccess()) this->UpdateElements();
                    break;
            }
            else if(ext == "tik") switch(sopt)
            {
                case 0:
                    std::string tcert = fullitm.substr(0, fullitm.length() - 3) + "cert";
                    if(!gleaf::fs::Exists(tcert))
                    {
                        dlg = new pu::Dialog("Ticket import error", "To be able to import this ticket, both the *.tik and *.cert files are required.\nYou selected the *.cert one, but the *.tik one couldn't be found.\n\nBoth need to have the same name.", pu::draw::Font::NintendoStandard);
                        dlg->AddOption("Ok");
                        mainapp->ShowDialog(dlg);
                    }
                    dlg = new pu::Dialog("Ticket import confirmation", "The selected ticket will be imported.", pu::draw::Font::NintendoStandard);
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
                        if(rc != 0) mainapp->UpdateFooter("An error ocurred trying to install the ticket (error code " + gleaf::horizon::FormatHex(rc) + ")");
                    }
                    break;
                case 2:
                    UpdateClipboard(fullitm);
                    this->UpdateElements();
                    break;
                case 3:
                    gleaf::fs::DeleteFile(fullitm);
                    mainapp->UpdateFooter("File deleted: \'" + gleaf::fs::GetPathWithoutRoot(fullitm) + "\'.");
                    if(this->WarnNANDWriteAccess()) this->UpdateElements();
                    break;
            }
            else switch(sopt)
            {
                case 0:
                    UpdateClipboard(fullitm);
                    this->UpdateElements();
                    break;
                case 1:
                    gleaf::fs::DeleteFile(fullitm);
                    mainapp->UpdateFooter("File deleted: \'" + gleaf::fs::GetPathWithoutRoot(fullitm) + "\'.");
                    if(this->WarnNANDWriteAccess()) this->UpdateElements();
                    break;
            }
        }
    }

    void PartitionBrowserLayout::fsItems_Click_Y()
    {
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        std::string fullitm = this->gexp->FullPathFor(itm);
        if(gleaf::fs::IsDirectory(fullitm))
        {
            pu::Dialog *dlg = new pu::Dialog("Directory options", "What would you like to do with the selected directory?", pu::draw::Font::NintendoStandard);
            dlg->AddOption("Browse");
            dlg->AddOption("Copy");
            dlg->AddOption("Delete");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            u32 sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 3)) return;
            else switch(sopt)
            {
                case 0:
                    if(this->gexp->NavigateForward(itm)) this->UpdateElements();
                    break;
                case 1:
                    UpdateClipboard(fullitm);
                    if(this->WarnNANDWriteAccess()) this->UpdateElements();
                    break;
                case 2:
                    gleaf::fs::DeleteDirectory(fullitm);
                    mainapp->UpdateFooter("Directory deleted: \'" + gleaf::fs::GetPathWithoutRoot(fullitm) + "\'.");
                    if(this->WarnNANDWriteAccess()) this->UpdateElements();
                    break;
            }
        }
    }

    gleaf::fs::Explorer *PartitionBrowserLayout::GetExplorer()
    {
        return this->gexp;
    }

    NSPInstallLayout::NSPInstallLayout()
    {
        this->installText = new pu::element::TextBlock(150, 300, "Starting NSP installation...");
        this->installBar = new pu::element::ProgressBar(490, 335, 300, 50);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void NSPInstallLayout::StartInstall(gleaf::nsp::Installer *Inst, pu::Layout *Prev, bool Delete)
    {
        if(gleaf::IsApplication()) appletBeginBlockingHomeButton(0);
        else appletLockExit();
        this->installText->SetText("Processing title records...");
        mainapp->UpdateFooter("Installing NSP...");
        mainapp->CallForRender();
        gleaf::nsp::InstallerResult rc = Inst->ProcessRecords();
        this->LogError(rc);
        this->installText->SetText("Starting to write contents...");
        mainapp->CallForRender();
        if(rc.IsSuccess()) rc = Inst->WriteContents([&](gleaf::ncm::ContentRecord NCA, u32 Index, u32 Count, int Percentage)
        {
            std::string name = "Writing content \'"  + gleaf::horizon::GetStringFromNCAId(NCA.NCAId);
            if(NCA.Type == gleaf::ncm::ContentType::Meta) name += ".cnmt";
            name += ".nca\'... (NCA " + std::to_string(Index + 1) + " of " + std::to_string(Count) + ")";
            this->installText->SetText(name);
            this->installBar->SetProgress((u8)Percentage);
            mainapp->CallForRender();
        });
        if(!rc.IsSuccess()) this->LogError(rc);
        Inst->Finalize();
        if(gleaf::IsApplication()) appletEndBlockingHomeButton();
        else appletUnlockExit();
        if(rc.IsSuccess())
        {
            mainapp->UpdateFooter("The NSP was successfully installed.");
            if(Delete) mainapp->UpdateFooter("The NSP was successfully installed and deleted.");
        }
        else mainapp->UpdateFooter("An error ocurred installing the NSP.");
        mainapp->LoadLayout(Prev);
        mainapp->CallForRender();
    }

    void NSPInstallLayout::LogError(gleaf::nsp::InstallerResult Res)
    {
        if(!Res.IsSuccess())
        {
            std::string err = "An error ocurred while installing NSP package:\n\n";
            if(Res.Type == gleaf::nsp::InstallerError::Success) return;
            switch(Res.Type)
            {
                case gleaf::nsp::InstallerError::BadNSP:
                    err += "Failed to read from the NSP file.\nThis error could be caused by various reasons: invalid NSP, minimum firmware mismatch...";
                    break;
                case gleaf::nsp::InstallerError::NSPOpen:
                    err += "Failed to open the NSP file. Does it exist?";
                    break;
                case gleaf::nsp::InstallerError::BadCNMTNCA:
                    err += "Failed to read from the meta NCA (CNMT NCA) within the NSP.";
                    break;
                case gleaf::nsp::InstallerError::CNMTMCAOpen:
                    err += "Failed to open the meta NCA (CNMT NCA) within the NSP.";
                    break;
                case gleaf::nsp::InstallerError::BadCNMT:
                    err += "Failed to read from the meta file (CNMT) within the CNMT NCA.";
                    break;
                case gleaf::nsp::InstallerError::CNMTOpen:
                    err += "Failed to open the meta file (CNMT) within the CNMT NCA.";
                    break;
                case gleaf::nsp::InstallerError::BadControlNCA:
                    err += "Failed to open the control NCA within the NSP.";
                    break;
                case gleaf::nsp::InstallerError::MetaDatabaseOpen:
                    err += "Failed to open content meta database for record processing.";
                    break;
                case gleaf::nsp::InstallerError::MetaDatabaseSet:
                    err += "Failed to set in the content meta database for record processing.";
                    break;
                case gleaf::nsp::InstallerError::MetaDatabaseCommit:
                    err += "Failed to commit on the content meta database for record processing.";
                    break;
                case gleaf::nsp::InstallerError::ContentMetaCount:
                    err += "Failed to count content meta for registered application.";
                    break;
                case gleaf::nsp::InstallerError::ContentMetaList:
                    err += "Failed to list content meta for registered application.";
                    break;
                case gleaf::nsp::InstallerError::RecordPush:
                    err += "Failed to push record for application.";
                    break;
                case gleaf::nsp::InstallerError::InstallBadNCA:
                    err += "Failed to find NCA content to write within the NSP.";
                    break;
                default:
                    err += "<undocumented error>";
                    break;
            }
            err += " (error code " + gleaf::horizon::FormatHex(Res.Error) + ")";
            pu::Dialog *dlg = new pu::Dialog("NSP installation error", err, pu::draw::Font::NintendoStandard);
            dlg->AddOption("Ok");
            mainapp->ShowDialog(dlg);
            mainapp->UpdateFooter("An error ocurred installing the NSP (error code " + gleaf::horizon::FormatHex(Res.Error) + ")");
        }
    }

    TitleManagerLayout::TitleManagerLayout()
    {
        this->titlesMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->titlesMenu->SetCooldownEnabled(true);
        this->notTitlesText = new pu::element::TextBlock(450, 400, "No titles were found for this console.");
        this->UpdateElements();
        this->AddChild(this->notTitlesText);
        this->AddChild(this->titlesMenu);
    }

    void TitleManagerLayout::UpdateElements()
    {
        this->titles = gleaf::horizon::GetAllSystemTitles();
        this->titlesMenu->ClearItems();
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
                gleaf::horizon::Title title = this->titles[i];
                pu::element::MenuItem *itm = new pu::element::MenuItem(title.Name);
                switch(title.Location)
                {
                    case gleaf::Storage::GameCart:
                        itm->SetIcon("romfs:/Common/GameCart.png");
                        break;
                    case gleaf::Storage::NAND:
                        itm->SetIcon("romfs:/Common/NAND.png");
                        break;
                    case gleaf::Storage::SdCard:
                        itm->SetIcon("romfs:/Common/SdCard.png");
                        break;
                }
                itm->AddOnClick(std::bind(&TitleManagerLayout::titles_Click, this));
                this->titlesMenu->AddItem(itm);
            }
            this->titlesMenu->SetSelectedIndex(0);
        }
    }

    void TitleManagerLayout::titles_Click()
    {
        gleaf::horizon::Title seltit = this->titles[this->titlesMenu->GetSelectedIndex()];
        if(seltit.Location == gleaf::Storage::GameCart) return;
        std::string info = "Information about selected title:\n\n\n";
        info += "Name: " + seltit.Name;
        info += "\nAuthor: " + seltit.Author;
        info += "\nVersion: " + seltit.Version;
        info += "\nInstall location: ";
        switch(seltit.Location)
        {
            case gleaf::Storage::GameCart:
                info += "Game cart";
                break;
            case gleaf::Storage::NAND:
                info += "NAND (system memory)";
                break;
            case gleaf::Storage::SdCard:
                info += "SD card";
                break;
        }
        info += "\n\nApplication Id: " + gleaf::horizon::FormatApplicationId(seltit.ApplicationId);
        pu::Dialog *dlg = new pu::Dialog("Installed title information", info, pu::draw::Font::NintendoStandard);
        pu::element::Image *img = new pu::element::Image(994, 30, seltit.GetExportedIconPath());
        dlg->SetIcon(img);
        dlg->AddOption("Uninstall");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 1)) return;
        else
        {
            dlg = new pu::Dialog("Title uninstall", "Are you sure you want to uninstall the previously selected title?", pu::draw::Font::NintendoStandard);
            dlg->AddOption("Yes");
            dlg->AddOption("Cancel");
            mainapp->ShowDialog(dlg);
            sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 1)) return;
            else
            {
                Result rc = gleaf::ns::DeleteApplicationCompletely(seltit.ApplicationId);
                std::string resstr = "The title was successfully uninstalled from this console.";
                if(rc != 0) resstr = "The title was not successfully uninstalled (error code " + std::to_string(rc) + ")";
                dlg = new pu::Dialog("Title uninstall", resstr, pu::draw::Font::NintendoStandard);
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                if(rc == 0) this->UpdateElements();
            }
        }
    }

    std::vector<gleaf::horizon::Title> TitleManagerLayout::GetTitles()
    {
        return this->titles;
    }

    TicketManagerLayout::TicketManagerLayout()
    {
        this->ticketsMenu = new pu::element::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->ticketsMenu->SetCooldownEnabled(true);
        this->notTicketsText = new pu::element::TextBlock(450, 400, "No tickets were found for this console.");
        this->UpdateElements();
        this->AddChild(this->notTicketsText);
        this->AddChild(this->ticketsMenu);
    }

    void TicketManagerLayout::UpdateElements()
    {
        this->tickets = gleaf::horizon::GetAllSystemTickets();
        this->ticketsMenu->ClearItems();
        if(this->tickets.empty())
        {
            this->notTicketsText->SetVisible(true);
            this->ticketsMenu->SetVisible(false);
        }
        else
        {
            this->notTicketsText->SetVisible(false);
            std::vector<gleaf::horizon::Title> ots = gleaf::horizon::GetAllSystemTitles();
            for(u32 i = 0; i < this->tickets.size(); i++)
            {
                gleaf::horizon::Ticket ticket = this->tickets[i];
                u64 tappid = ticket.GetApplicationId();
                std::string tname = "Unknown title (" + gleaf::horizon::FormatApplicationId(tappid) + ")";
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
        gleaf::horizon::Ticket seltick = this->tickets[this->ticketsMenu->GetSelectedIndex()];
        std::string info = "Information about selected ticket:\n\n\n";
        info += "Application Id: " + gleaf::horizon::FormatApplicationId(seltick.GetApplicationId());
        info += "\nKey generation: " + std::to_string(seltick.GetKeyGeneration());
        pu::Dialog *dlg = new pu::Dialog("Installed ticket information", info, pu::draw::Font::NintendoStandard);
        dlg->AddOption("Remove ticket");
        dlg->AddOption("Cancel");
        mainapp->ShowDialog(dlg);
        u32 sopt = dlg->GetSelectedIndex();
        if(dlg->UserCancelled() || (sopt == 1)) return;
        else
        {
            dlg = new pu::Dialog("Ticket remove", "Are you sure you want to remove the previously selected ticket?", pu::draw::Font::NintendoStandard);
            dlg->AddOption("Yes");
            dlg->AddOption("cancel");
            mainapp->ShowDialog(dlg);
            sopt = dlg->GetSelectedIndex();
            if(dlg->UserCancelled() || (sopt == 1)) return;
            else
            {
                Result rc = gleaf::es::DeleteTicket(&seltick.RId, sizeof(gleaf::es::RightsId));
                std::string resstr = "The ticket was successfully removed from this console.";
                if(rc != 0) resstr = "The title was not successfully removed (error code " + std::to_string(rc) + ")";
                dlg = new pu::Dialog("Ticket uninstall", resstr, pu::draw::Font::NintendoStandard);
                dlg->AddOption("Ok");
                mainapp->ShowDialog(dlg);
                if(rc == 0) this->UpdateElements();
            }
        }
    }

    SystemInfoLayout::SystemInfoLayout() : pu::Layout()
    {
        gleaf::horizon::FwVersion fwv = gleaf::horizon::GetFwVersion();
        this->fwText = new pu::element::TextBlock(40, 330, "Firmware: " + fwv.ToString() + " (" + fwv.DisplayName + ")");
        this->sdText = new pu::element::TextBlock(420, 490, "SD card:");
        this->modeText = new pu::element::TextBlock(40, 365, (IsApplication() ? "Goldleaf is running as an installed title." : "Goldleaf is running as a homebrew NRO binary."));
        this->sdText->SetFontSize(30);
        this->sdBar = new pu::element::ProgressBar(420, 525, 300, 30);
        this->nandText = new pu::element::TextBlock(850, 250, "NAND partitions:");
        this->nandText->SetFontSize(30);
        this->safeText = new pu::element::TextBlock(850, 310, "NAND Safe:");
        this->safeBar = new pu::element::ProgressBar(850, 345, 300, 30);
        this->systemText = new pu::element::TextBlock(850, 400, "NAND System:");
        this->systemBar = new pu::element::ProgressBar(850, 435, 300, 30);
        this->userText = new pu::element::TextBlock(850, 490, "NAND User:");
        this->userBar = new pu::element::ProgressBar(850, 525, 300, 30);
        this->UpdateElements();
        this->AddChild(this->fwText);
        this->AddChild(this->sdText);
        this->AddChild(this->modeText);
        this->AddChild(this->sdBar);
        this->AddChild(this->nandText);
        this->AddChild(this->safeText);
        this->AddChild(this->safeBar);
        this->AddChild(this->systemText);
        this->AddChild(this->systemBar);
        this->AddChild(this->userText);
        this->AddChild(this->userBar);
    }

    void SystemInfoLayout::UpdateElements()
    {
        u64 sdctotal = gleaf::fs::GetTotalSpaceForPartition(gleaf::fs::Partition::SdCard);
        u64 sdcfree = gleaf::fs::GetFreeSpaceForPartition(gleaf::fs::Partition::SdCard);
        u64 nsftotal = gleaf::fs::GetTotalSpaceForPartition(gleaf::fs::Partition::NANDSafe);
        u64 nsffree = gleaf::fs::GetFreeSpaceForPartition(gleaf::fs::Partition::NANDSafe);
        u64 nsstotal = gleaf::fs::GetTotalSpaceForPartition(gleaf::fs::Partition::NANDSystem);
        u64 nssfree = gleaf::fs::GetFreeSpaceForPartition(gleaf::fs::Partition::NANDSystem);
        u64 nsutotal = gleaf::fs::GetTotalSpaceForPartition(gleaf::fs::Partition::NANDUser);
        u64 nsufree = gleaf::fs::GetFreeSpaceForPartition(gleaf::fs::Partition::NANDUser);
        u8 sdval = ((100 * (sdctotal - sdcfree)) / sdctotal);
        u8 nsfval = ((100 * (nsftotal - nsffree)) / nsftotal);
        u8 nssval = ((100 * (nsstotal - nssfree)) / nsstotal);
        u8 nsuval = ((100 * (nsutotal - nsufree)) / nsutotal);
        this->sdBar->SetProgress(sdval);
        this->safeBar->SetProgress(nsfval);
        this->systemBar->SetProgress(nssval);
        this->userBar->SetProgress(nsuval);
        this->sdText->SetText("SD card: " + fs::FormatSize(sdcfree) + " free");
        this->safeText->SetText("NAND Safe: " + fs::FormatSize(nsffree) + " free");
        this->systemText->SetText("NAND System: " + fs::FormatSize(nssfree) + " free");
        this->userText->SetText("NAND User: " + fs::FormatSize(nsufree) + " free");
    }

    MainApplication::MainApplication() : pu::Application()
    {
        this->SetBackgroundColor({ 235, 235, 235, 255 });
        this->mainMenu = new MainMenuLayout();
        this->sdBrowser = new PartitionBrowserLayout(gleaf::fs::Partition::SdCard);
        this->sdBrowser->SetOnInput(std::bind(&MainApplication::sdBrowser_Input, this, std::placeholders::_1));
        this->nandBrowser = new PartitionBrowserLayout(gleaf::fs::Partition::NANDSystem);
        this->nandBrowser->SetOnInput(std::bind(&MainApplication::nandBrowser_Input, this, std::placeholders::_1));
        this->nspInstall = new NSPInstallLayout();
        this->titleManager = new TitleManagerLayout();
        this->titleManager->SetOnInput(std::bind(&MainApplication::titleManager_Input, this, std::placeholders::_1));
        this->ticketManager = new TicketManagerLayout();
        this->ticketManager->SetOnInput(std::bind(&MainApplication::ticketManager_Input, this, std::placeholders::_1));
        this->sysInfo = new SystemInfoLayout();
        this->sysInfo->SetOnInput(std::bind(&MainApplication::sysInfo_Input, this, std::placeholders::_1));
        this->bannerImage = new pu::element::Image(35, 35, "romfs:/Banner.png");
        this->timeText = new pu::element::TextBlock(1070, 50, gleaf::horizon::GetCurrentTime());
        this->batteryImage = new pu::element::Image(1200, 35, "romfs:/Battery/4.png");
        this->batteryChargeImage = new pu::element::Image(1200, 35, "romfs:/Battery/Charge.png");
        this->UpdateValues();
        this->footerText = new pu::element::TextBlock(20, 685, "Welcome to Goldleaf. You can install NSPs, import tickets, uninstall titles, remove tickets, browse SD and NAND...");
        this->footerText->SetFontSize(20);
        this->mainMenu->AddChild(this->bannerImage);
        this->sdBrowser->AddChild(this->bannerImage);
        this->nandBrowser->AddChild(this->bannerImage);
        this->nspInstall->AddChild(this->bannerImage);
        this->titleManager->AddChild(this->bannerImage);
        this->ticketManager->AddChild(this->bannerImage);
        this->sysInfo->AddChild(this->bannerImage);
        this->mainMenu->AddChild(this->timeText);
        this->sdBrowser->AddChild(this->timeText);
        this->nandBrowser->AddChild(this->timeText);
        this->nspInstall->AddChild(this->timeText);
        this->titleManager->AddChild(this->timeText);
        this->ticketManager->AddChild(this->timeText);
        this->sysInfo->AddChild(this->timeText);
        this->mainMenu->AddChild(this->batteryImage);
        this->sdBrowser->AddChild(this->batteryImage);
        this->nandBrowser->AddChild(this->batteryImage);
        this->nspInstall->AddChild(this->batteryImage);
        this->titleManager->AddChild(this->batteryImage);
        this->ticketManager->AddChild(this->batteryImage);
        this->sysInfo->AddChild(this->batteryImage);
        this->mainMenu->AddChild(this->batteryChargeImage);
        this->sdBrowser->AddChild(this->batteryChargeImage);
        this->nandBrowser->AddChild(this->batteryChargeImage);
        this->nspInstall->AddChild(this->batteryChargeImage);
        this->titleManager->AddChild(this->batteryChargeImage);
        this->ticketManager->AddChild(this->batteryChargeImage);
        this->sysInfo->AddChild(this->batteryChargeImage);
        this->mainMenu->AddChild(this->footerText);
        this->sdBrowser->AddChild(this->footerText);
        this->nandBrowser->AddChild(this->footerText);
        this->nspInstall->AddChild(this->footerText);
        this->titleManager->AddChild(this->footerText);
        this->ticketManager->AddChild(this->footerText);
        this->sysInfo->AddChild(this->footerText);
        this->AddThread(std::bind(&MainApplication::UpdateValues, this));
        this->SetOnInput(std::bind(&MainApplication::OnInput, this, std::placeholders::_1));
        this->LoadLayout(this->mainMenu);
    }

    void MainApplication::UpdateFooter(std::string Text)
    {
        this->footerText->SetText(Text);
    }

    void MainApplication::UpdateValues()
    {
        this->timeText->SetText(gleaf::horizon::GetCurrentTime());
        u32 blv = gleaf::horizon::GetBatteryLevel();
        bool isch = gleaf::horizon::IsCharging();
        if(blv <= 25) this->batteryImage->SetImage("romfs:/Battery/1.png");
        else if((blv > 25) && (blv <= 50)) this->batteryImage->SetImage("romfs:/Battery/2.png");
        else if((blv > 50) && (blv <= 75)) this->batteryImage->SetImage("romfs:/Battery/3.png");
        else if(blv > 75) this->batteryImage->SetImage("romfs:/Battery/4.png");
        if(isch) this->batteryChargeImage->SetVisible(true);
        else this->batteryChargeImage->SetVisible(false);
    }

    void MainApplication::sdBrowser_Input(u64 Input)
    {
        if(Input & KEY_B)
        {
            if(this->sdBrowser->GoBack()) this->sdBrowser->UpdateElements();
            else this->LoadLayout(this->mainMenu);
        }
        else if(Input & KEY_X)
        {
            if(clipboard != "")
            {
                bool cdir = gleaf::fs::IsDirectory(clipboard);
                pu::Dialog *dlg = new pu::Dialog("Clipboard process", "Current clipboard contents path:\n\'" + gleaf::fs::GetPathWithoutRoot(clipboard) + "\'\n\nDo you want to copy clipboard contents to this directory?", pu::draw::Font::NintendoStandard);
                if(cdir) dlg->SetIcon(new pu::element::Image(1150, 30, "romfs:/FileSystem/Directory.png"));
                else dlg->SetIcon(new pu::element::Image(1150, 30, "romfs:/FileSystem/File.png"));
                dlg->AddOption("Yes");
                dlg->AddOption("Cancel");
                mainapp->ShowDialog(dlg);
                u32 sopt = dlg->GetSelectedIndex();
                if(sopt == 0)
                {
                    std::string cname = gleaf::fs::GetFileName(clipboard);
                    if(cdir) gleaf::fs::CopyDirectory(clipboard, this->sdBrowser->GetExplorer()->FullPathFor(cname));
                    else gleaf::fs::CopyFile(clipboard, this->sdBrowser->GetExplorer()->FullPathFor(cname));
                    this->sdBrowser->UpdateElements();
                    mainapp->UpdateFooter("Clipboard was processed and cleaned (file / directory was copied: " + gleaf::fs::GetPathWithoutRoot(clipboard));
                    clipboard = "";
                }
            }
            else mainapp->UpdateFooter("Clipboard is not selected.");
        }
    }

    void MainApplication::nandBrowser_Input(u64 Input)
    {
        if(Input & KEY_B)
        {
            if(this->nandBrowser->GoBack()) this->nandBrowser->UpdateElements();
            else this->LoadLayout(this->mainMenu);
        }
        else if(Input & KEY_X)
        {
            if(clipboard != "")
            {
                bool cdir = gleaf::fs::IsDirectory(clipboard);
                pu::Dialog *dlg = new pu::Dialog("Clipboard process", "Current clipboard contents path:\n\'" + gleaf::fs::GetPathWithoutRoot(clipboard) + "\'\n\nDo you want to copy clipboard contents to this directory?", pu::draw::Font::NintendoStandard);
                if(cdir) dlg->SetIcon(new pu::element::Image(1150, 30, "romfs:/FileSystem/Directory.png"));
                else dlg->SetIcon(new pu::element::Image(1150, 30, "romfs:/FileSystem/File.png"));
                dlg->AddOption("Yes");
                dlg->AddOption("Cancel");
                mainapp->ShowDialog(dlg);
                u32 sopt = dlg->GetSelectedIndex();
                if((sopt == 0) && this->nandBrowser->WarnNANDWriteAccess())
                {
                    std::string cname = gleaf::fs::GetFileName(clipboard);
                    if(cdir) gleaf::fs::CopyDirectory(clipboard, this->nandBrowser->GetExplorer()->FullPathFor(cname));
                    else gleaf::fs::CopyFile(clipboard, this->nandBrowser->GetExplorer()->FullPathFor(cname));
                    this->nandBrowser->UpdateElements();
                    mainapp->UpdateFooter("Clipboard was processed and cleaned (file / directory was copied: " + gleaf::fs::GetPathWithoutRoot(clipboard));
                    clipboard = "";
                }
            }
            else mainapp->UpdateFooter("Clipboard is not selected.");
        }
    }

    void MainApplication::titleManager_Input(u64 Input)
    {
        if(Input & KEY_B) this->LoadLayout(this->mainMenu);
    }

    void MainApplication::ticketManager_Input(u64 Input)
    {
        if(Input & KEY_B) this->LoadLayout(this->mainMenu);
    }

    void MainApplication::sysInfo_Input(u64 Input)
    {
        if(Input & KEY_B) this->LoadLayout(this->mainMenu);
    }

    void MainApplication::OnInput(u64 Input)
    {
        if((Input & KEY_PLUS) || (Input & KEY_MINUS)) if(!gleaf::IsApplication()) this->Close();
    }

    PartitionBrowserLayout *MainApplication::GetSDBrowserLayout()
    {
        return this->sdBrowser;
    }

    PartitionBrowserLayout *MainApplication::GetNANDBrowserLayout()
    {
        return this->nandBrowser;
    }

    NSPInstallLayout *MainApplication::GetNSPInstallLayout()
    {
        return this->nspInstall;
    }

    TitleManagerLayout *MainApplication::GetTitleManagerLayout()
    {
        return this->titleManager;
    }

    TicketManagerLayout *MainApplication::GetTicketManagerLayout()
    {
        return this->ticketManager;
    }

    SystemInfoLayout *MainApplication::GetSystemInfoLayout()
    {
        return this->sysInfo;
    }

    void UpdateClipboard(std::string Path)
    {
        clipboard = Path;
        mainapp->UpdateFooter("Clipboard was set to \'" + gleaf::fs::GetPathWithoutRoot(Path) + "\'.");
    }

    void SetMainApplication(MainApplication *MainApp)
    {
        mainapp = MainApp;
    }
}