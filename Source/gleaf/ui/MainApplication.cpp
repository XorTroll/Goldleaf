#include <gleaf/ui/MainApplication.hpp>

namespace gleaf::ui
{
    MainApplication *mainapp;

    MainMenuLayout::MainMenuLayout() : pn::Layout()
    {
        this->optionMenu = new pn::fw::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->optionMenu->SetOnSelectionChanged(std::bind(&MainMenuLayout::optionMenu_SelectionChanged, this));
        this->sdcardMenuItem = new pn::fw::MenuItem("Browse SD card");
        this->sdcardMenuItem->SetIcon("romfs:/Common/SdCard.png");
        this->sdcardMenuItem->SetOnClick(std::bind(&MainMenuLayout::sdcardMenuItem_Click, this));
        this->nandMenuItem = new pn::fw::MenuItem("Browse system NAND");
        this->nandMenuItem->SetIcon("romfs:/Common/NAND.png");
        this->nandMenuItem->SetOnClick(std::bind(&MainMenuLayout::nandMenuItem_Click, this));
        this->remoteMenuItem = new pn::fw::MenuItem("Remote installation");
        this->remoteMenuItem->SetIcon("romfs:/Common/USB.png");
        this->titleMenuItem = new pn::fw::MenuItem("Manage installed titles");
        this->titleMenuItem->SetIcon("romfs:/Common/Storage.png");
        this->titleMenuItem->SetOnClick(std::bind(&MainMenuLayout::titleMenuItem_Click, this));
        this->ticketMenuItem = new pn::fw::MenuItem("Manage installed tickets");
        this->ticketMenuItem->SetIcon("romfs:/Common/Ticket.png");
        this->ticketMenuItem->SetOnClick(std::bind(&MainMenuLayout::ticketMenuItem_Click, this));
        this->sysinfoMenuItem = new pn::fw::MenuItem("Show system information");
        this->sysinfoMenuItem->SetIcon("romfs:/Common/Settings.png");
        this->sysinfoMenuItem->SetOnClick(std::bind(&MainMenuLayout::sysinfoMenuItem_Click, this));
        this->aboutMenuItem = new pn::fw::MenuItem("About this application");
        this->aboutMenuItem->SetIcon("romfs:/Common/Info.png");
        this->optionMenu->AddItem(this->sdcardMenuItem);
        this->optionMenu->AddItem(this->nandMenuItem);
        this->optionMenu->AddItem(this->remoteMenuItem);
        this->optionMenu->AddItem(this->titleMenuItem);
        this->optionMenu->AddItem(this->ticketMenuItem);
        this->optionMenu->AddItem(this->sysinfoMenuItem);
        this->optionMenu->AddItem(this->aboutMenuItem);
        this->optionMenu->SetCooldownEnabled(false);
        this->AddChild(this->optionMenu);
    }

    void MainMenuLayout::optionMenu_SelectionChanged()
    {
        std::string info = "Unknown option";
        pn::fw::MenuItem *isel = this->optionMenu->GetSelectedItem();
        if(isel == this->sdcardMenuItem) info = "Browse the SD card's files and folders. You can install NSPs from here.";
        else if(isel == this->nandMenuItem) info = "Browse the NAND's files and folders. You can export them to the SD card. Writing (copying) is not allowed.";
        else if(isel == this->remoteMenuItem) info = "Install NSPs remotely, via network or USB communications.";
        else if(isel == this->titleMenuItem) info = "Browse currently installed titles. You can view their information and/or uninstall them.";
        else if(isel == this->ticketMenuItem) info = "Browse currently installed tickets. You can view their information and/or remove them.";
        else if(isel == this->sysinfoMenuItem) info = "Show information about this Nintendo Switch: current firmware and used space in NAND and SD card.";
        else if(isel == this->aboutMenuItem) info = "Show information about Goldleaf: author, version, credits...";
        mainapp->UpdateFooter(info);
    }

    void MainMenuLayout::sdcardMenuItem_Click()
    {
        mainapp->GetSDBrowserLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSDBrowserLayout());
    }

    void MainMenuLayout::nandMenuItem_Click()
    {
        pn::fw::Dialog *dlg = new pn::fw::Dialog("Select NAND partition", "Select NAND partition to explore via Goldleaf.", pn::draw::Font::NintendoStandard);
        pn::fw::DialogOption *optsf = new pn::fw::DialogOption("Safe", pn::fw::DialogResult::Ok);
        pn::fw::DialogOption *optss = new pn::fw::DialogOption("System", pn::fw::DialogResult::Ok);
        pn::fw::DialogOption *optus = new pn::fw::DialogOption("User", pn::fw::DialogResult::Ok);
        pn::fw::DialogOption *optc = new pn::fw::DialogOption("Cancel", pn::fw::DialogResult::Cancel);
        dlg->AddOption(optsf);
        dlg->AddOption(optss);
        dlg->AddOption(optus);
        dlg->AddOption(optc);
        mainapp->ShowDialog(dlg);
        pn::fw::DialogOption *sopt = dlg->GetSelectedOption();
        if(sopt == optc) return;
        else if(sopt == optsf) mainapp->GetNANDBrowserLayout()->ChangePartition(gleaf::fs::Partition::NANDSafe);
        else if(sopt == optss) mainapp->GetNANDBrowserLayout()->ChangePartition(gleaf::fs::Partition::NANDSystem);
        else if(sopt == optus) mainapp->GetNANDBrowserLayout()->ChangePartition(gleaf::fs::Partition::NANDUser);
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
        pn::fw::Dialog *dlg = new pn::fw::Dialog("Removing tickets", "Removing tickets can be dangerous.\nIf tickets from installed apps get removed, the title won't probably work.", pn::draw::Font::NintendoStandard);
        pn::fw::DialogOption *optok = new pn::fw::DialogOption("Ok", pn::fw::DialogResult::Cancel);
        dlg->AddOption(optok);
        mainapp->ShowDialog(dlg);
    }

    void MainMenuLayout::sysinfoMenuItem_Click()
    {
        mainapp->GetSystemInfoLayout()->UpdateElements();
        mainapp->LoadLayout(mainapp->GetSystemInfoLayout());
    }

    PartitionBrowserLayout::PartitionBrowserLayout(gleaf::fs::Partition Partition) : pn::Layout()
    {
        this->gexp = new gleaf::fs::Explorer(Partition);
        this->browseMenu = new pn::fw::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->dirEmptyText = new pn::fw::TextBlock(450, 400, "Directory is empty.");
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
                pn::fw::MenuItem *mitm = new pn::fw::MenuItem(itm);
                if(isdir) mitm->SetIcon("romfs:/FileSystem/Directory.png");
                else
                {
                    std::string ext = gleaf::fs::GetExtension(itm);
                    if(ext == "nsp") mitm->SetIcon("romfs:/FileSystem/NSP.png");
                    else mitm->SetIcon("romfs:/FileSystem/File.png");
                }
                mitm->SetOnClick(std::bind(&PartitionBrowserLayout::fsItems_Click, this));
                this->browseMenu->AddItem(mitm);
            }
            this->browseMenu->SetSelectedIndex(0);
        }
    }

    bool PartitionBrowserLayout::GoBack()
    {
        return this->gexp->NavigateBack();
    }

    void PartitionBrowserLayout::fsItems_Click()
    {
        std::string itm = this->browseMenu->GetSelectedItem()->GetName();
        if(this->gexp->NavigateForward(itm)) this->UpdateElements();
        else
        {
            std::string ext = gleaf::fs::GetExtension(itm);
            if(this->gexp->GetPartition() == gleaf::fs::Partition::SdCard)
            {
                if(ext == "nsp")
                {
                    pn::fw::Dialog *dlg = new pn::fw::Dialog("Select NSP install location", "Which location would you like to install the selected NSP on?", pn::draw::Font::NintendoStandard);
                    pn::fw::DialogOption *optsd = new pn::fw::DialogOption("SD card", pn::fw::DialogResult::Ok);
                    pn::fw::DialogOption *optnand = new pn::fw::DialogOption("NAND (system memory)", pn::fw::DialogResult::Ok);
                    pn::fw::DialogOption *optc = new pn::fw::DialogOption("Cancel", pn::fw::DialogResult::Cancel);
                    dlg->AddOption(optsd);
                    dlg->AddOption(optnand);
                    dlg->AddOption(optc);
                    mainapp->ShowDialog(dlg);
                    pn::fw::DialogOption *sopt = dlg->GetSelectedOption();
                    if(sopt == optc) return;
                    dlg = new pn::fw::Dialog("Ignore required firmware version", "Should Goldleaf ignore the required firmware version of the NSP?", pn::draw::Font::NintendoStandard);
                    pn::fw::DialogOption *opty = new pn::fw::DialogOption("Yes", pn::fw::DialogResult::Ok);
                    pn::fw::DialogOption *optn = new pn::fw::DialogOption("No", pn::fw::DialogResult::Ok);
                    dlg->AddOption(opty);
                    dlg->AddOption(optn);
                    dlg->AddOption(optc);
                    mainapp->ShowDialog(dlg);
                    sopt = dlg->GetSelectedOption();
                    if(sopt == optc) return;
                    bool ignorev = (sopt == opty);
                    gleaf::Destination dst = gleaf::Destination::SdCard;
                    if(sopt == optsd) dst = gleaf::Destination::SdCard;
                    else if(sopt == optnand) dst = gleaf::Destination::NAND;
                    std::string fullitm = this->gexp->FullPathFor(itm);
                    std::string nspipt = "@Sdcard:" + fullitm.substr(5);
                    gleaf::nsp::Installer inst(dst, nspipt, ignorev);
                    bool hasnacp = inst.HasContent(gleaf::ncm::ContentType::Control);
                    std::string info = "No control data was found inside the NSP. (control NCA seems to be missing)";
                    if(hasnacp)
                    {
                        info = "Information about the NSP's control data:\n\n\n";
                        NacpStruct *nacp = inst.GetNACP();
                        info += "Name: ";
                        info += nacp->lang[0].name;
                        info += "\nAuthor: ";
                        info += nacp->lang[0].author;
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
                    if(inst.HasTicketAndCert()) info += "This NSP has a ticket and it will be installed. (otherwise, the title wouldn't work)";
                    else info += "This NSP doesn't have a ticket. It seems to only have standard crypto.";
                    dlg = new pn::fw::Dialog("Ready to start installing?", info, pn::draw::Font::NintendoStandard);
                    if(hasnacp)
                    {
                        pn::fw::Image *img = new pn::fw::Image(994, 30, inst.GetExportedIconPath());
                        dlg->SetIcon(img);
                    }
                    pn::fw::DialogOption *opti = new pn::fw::DialogOption("Install", pn::fw::DialogResult::Ok);
                    dlg->AddOption(opti);
                    dlg->AddOption(optc);
                    mainapp->ShowDialog(dlg);
                    sopt = dlg->GetSelectedOption();
                    if(sopt == optc) return;
                    mainapp->LoadLayout(mainapp->GetNSPInstallLayout());
                    appletBeginBlockingHomeButton(0);
                    mainapp->GetNSPInstallLayout()->StartInstall(&inst, mainapp->GetSDBrowserLayout());
                    appletEndBlockingHomeButton();
                }
            }
            else
            {
                std::string fullitm = this->gexp->FullPathFor(itm);
                if(gleaf::fs::IsFile(fullitm))
                {
                    pn::fw::Dialog *dlg = new pn::fw::Dialog("Export NAND file", "Do you want to export this file to the SD card?", pn::draw::Font::NintendoStandard);
                    pn::fw::DialogOption *opty = new pn::fw::DialogOption("Yes", pn::fw::DialogResult::Ok);
                    pn::fw::DialogOption *optc = new pn::fw::DialogOption("Cancel", pn::fw::DialogResult::Cancel);
                    dlg->AddOption(opty);
                    dlg->AddOption(optc);
                    mainapp->ShowDialog(dlg);
                    pn::fw::DialogOption *sopt = dlg->GetSelectedOption();
                    if(sopt == optc) return;
                    std::ifstream ifs(fullitm);
                    std::string outitm = "sdmc:/switch/.gleaf/out/" + gleaf::fs::GetFileName(fullitm);
                    std::ofstream ofs(outitm);
                    bool ok = false;
                    if(ifs.good()) if(ofs.good())
                    {
                        ok = true;
                        ofs << ifs.rdbuf();
                    }
                    ifs.close();
                    ofs.close();
                    std::string info = "File was successfully exported to \'" + outitm + "\'.";
                    if(!ok) info = "An error ocurred attempting to export the file.";
                    dlg = new pn::fw::Dialog("File export information", info, pn::draw::Font::NintendoStandard);
                    optc = new pn::fw::DialogOption("Ok", pn::fw::DialogResult::Ok);
                    dlg->AddOption(optc);
                    mainapp->ShowDialog(dlg);
                }
            }
        }
    }

    NSPInstallLayout::NSPInstallLayout()
    {
        this->installText = new pn::fw::TextBlock(150, 300, "Starting NSP installation...");
        this->installBar = new pn::fw::ProgressBar(490, 335, 300, 50);
        this->AddChild(this->installText);
        this->AddChild(this->installBar);
    }

    void NSPInstallLayout::StartInstall(gleaf::nsp::Installer *Inst, pn::Layout *Prev)
    {
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
        if(rc.IsSuccess())
        {
            pn::fw::Dialog *dlg = new pn::fw::Dialog("NSP installation finished", "The NSP was successfully installed.\nYou can close this application and the title (should) appear on Home Menu. Enjoy!", pn::draw::Font::NintendoStandard);
            pn::fw::DialogOption *optok = new pn::fw::DialogOption("Ok", pn::fw::DialogResult::Cancel);
            dlg->AddOption(optok);
            mainapp->ShowDialog(dlg);
            mainapp->UpdateFooter("The NSP was successfully installed.");
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
                    err += "Failed to read from the NSP file. Are you sure this console has the correct keygen?";
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
            err += " (error code " + std::to_string(Res.Error) + ")";
            pn::fw::Dialog *dlg = new pn::fw::Dialog("NSP installation error", err, pn::draw::Font::NintendoStandard);
            pn::fw::DialogOption *optok = new pn::fw::DialogOption("Ok", pn::fw::DialogResult::Cancel);
            dlg->AddOption(optok);
            mainapp->ShowDialog(dlg);
        }
    }

    TitleManagerLayout::TitleManagerLayout()
    {
        this->titlesMenu = new pn::fw::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->notTitlesText = new pn::fw::TextBlock(450, 400, "No titles were found for this console.");
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
                pn::fw::MenuItem *itm = new pn::fw::MenuItem(title.Name);
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
                itm->SetOnClick(std::bind(&TitleManagerLayout::titles_Click, this));
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
        pn::fw::Dialog *dlg = new pn::fw::Dialog("Installed title information", info, pn::draw::Font::NintendoStandard);
        pn::fw::Image *img = new pn::fw::Image(994, 30, seltit.GetExportedIconPath());
        dlg->SetIcon(img);
        pn::fw::DialogOption *opty = new pn::fw::DialogOption("Uninstall title", pn::fw::DialogResult::Ok);
        pn::fw::DialogOption *optn = new pn::fw::DialogOption("Back", pn::fw::DialogResult::Cancel);
        dlg->AddOption(opty);
        dlg->AddOption(optn);
        mainapp->ShowDialog(dlg);
        pn::fw::DialogOption *sopt = dlg->GetSelectedOption();
        if(sopt == optn) return;
        else
        {
            dlg = new pn::fw::Dialog("Title uninstall", "Are you sure you want to uninstall the previously selected title?", pn::draw::Font::NintendoStandard);
            opty = new pn::fw::DialogOption("Yes", pn::fw::DialogResult::Ok);
            optn = new pn::fw::DialogOption("Cancel", pn::fw::DialogResult::Cancel);
            dlg->AddOption(opty);
            dlg->AddOption(optn);
            mainapp->ShowDialog(dlg);
            sopt = dlg->GetSelectedOption();
            if(sopt == optn) return;
            else
            {
                Result rc = gleaf::ns::DeleteApplicationCompletely(seltit.ApplicationId);
                std::string resstr = "The title was successfully uninstalled from this console.";
                if(rc != 0) resstr = "The title was not successfully uninstalled (error code " + std::to_string(rc) + ")";
                dlg = new pn::fw::Dialog("Title uninstall", resstr, pn::draw::Font::NintendoStandard);
                optn = new pn::fw::DialogOption("Ok", pn::fw::DialogResult::Ok);
                dlg->AddOption(optn);
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
        this->ticketsMenu = new pn::fw::Menu(0, 170, 1280, { 220, 220, 220, 255 }, 100, 5);
        this->notTicketsText = new pn::fw::TextBlock(450, 400, "No tickets were found for this console.");
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
                pn::fw::MenuItem *itm = new pn::fw::MenuItem(tname);
                itm->SetIcon("romfs:/Common/Ticket.png");
                itm->SetOnClick(std::bind(&TicketManagerLayout::tickets_Click, this));
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
        pn::fw::Dialog *dlg = new pn::fw::Dialog("Installed ticket information", info, pn::draw::Font::NintendoStandard);
        pn::fw::DialogOption *opty = new pn::fw::DialogOption("Remove ticket", pn::fw::DialogResult::Ok);
        pn::fw::DialogOption *optn = new pn::fw::DialogOption("Back", pn::fw::DialogResult::Cancel);
        dlg->AddOption(opty);
        dlg->AddOption(optn);
        mainapp->ShowDialog(dlg);
        pn::fw::DialogOption *sopt = dlg->GetSelectedOption();
        if(sopt == optn) return;
        else
        {
            dlg = new pn::fw::Dialog("Ticket remove", "Are you sure you want to remove the previously selected ticket?", pn::draw::Font::NintendoStandard);
            opty = new pn::fw::DialogOption("Yes", pn::fw::DialogResult::Ok);
            optn = new pn::fw::DialogOption("Cancel", pn::fw::DialogResult::Cancel);
            dlg->AddOption(opty);
            dlg->AddOption(optn);
            mainapp->ShowDialog(dlg);
            sopt = dlg->GetSelectedOption();
            if(sopt == optn) return;
            else
            {
                Result rc = gleaf::es::DeleteTicket(&seltick.RId, sizeof(gleaf::es::RightsId));
                std::string resstr = "The ticket was successfully removed from this console.";
                if(rc != 0) resstr = "The title was not successfully removed (error code " + std::to_string(rc) + ")";
                dlg = new pn::fw::Dialog("Ticket uninstall", resstr, pn::draw::Font::NintendoStandard);
                optn = new pn::fw::DialogOption("Ok", pn::fw::DialogResult::Ok);
                dlg->AddOption(optn);
                mainapp->ShowDialog(dlg);
                if(rc == 0) this->UpdateElements();
            }
        }
    }

    SystemInfoLayout::SystemInfoLayout() : pn::Layout()
    {
        gleaf::horizon::FwVersion fwv = gleaf::horizon::GetFwVersion();
        this->fwText = new pn::fw::TextBlock(225, 600, "Firmware: " + fwv.ToString() + " (" + fwv.DisplayName + ")");
        this->sdText = new pn::fw::TextBlock(250, 370, "SD card:");
        this->sdText->SetFontSize(30);
        this->sdBar = new pn::fw::ProgressBar(250, 415, 300, 30);
        this->nandText = new pn::fw::TextBlock(620, 230, "NAND:");
        this->nandText->SetFontSize(30);
        this->safeText = new pn::fw::TextBlock(620, 290, "Safe partition:");
        this->safeBar = new pn::fw::ProgressBar(620, 325, 300, 30);
        this->systemText = new pn::fw::TextBlock(620, 380, "System partition:");
        this->systemBar = new pn::fw::ProgressBar(620, 415, 300, 30);
        this->userText = new pn::fw::TextBlock(620, 470, "User partition:");
        this->userBar = new pn::fw::ProgressBar(620, 505, 300, 30);
        this->UpdateElements();
        this->AddChild(this->fwText);
        this->AddChild(this->sdText);
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
    }

    MainApplication::MainApplication() : pn::Application()
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
        this->bannerImage = new pn::fw::Image(35, 35, "romfs:/Banner.png");
        this->timeText = new pn::fw::TextBlock(1070, 50, gleaf::horizon::GetCurrentTime());
        this->batteryImage = new pn::fw::Image(1200, 35, "romfs:/Battery/4.png");
        this->batteryChargeImage = new pn::fw::Image(1200, 35, "romfs:/Battery/Charge.png");
        this->UpdateValues();
        this->footerText = new pn::fw::TextBlock(20, 685, "Welcome to Goldleaf. You can install titles or tickets, manage them, check system's information, browse NAND...");
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
    }

    void MainApplication::nandBrowser_Input(u64 Input)
    {
        if(Input & KEY_B)
        {
            if(this->nandBrowser->GoBack()) this->nandBrowser->UpdateElements();
            else this->LoadLayout(this->mainMenu);
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

    void SetMainApplication(MainApplication *MainApp)
    {
        mainapp = MainApp;
    }
}