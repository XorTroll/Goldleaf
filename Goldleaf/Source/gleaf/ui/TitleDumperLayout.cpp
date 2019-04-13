#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
{
    extern MainApplication *mainapp;

    TitleDumperLayout::TitleDumperLayout()
    {
        this->dumpText = new pu::element::TextBlock(150, 320, set::GetDictionaryEntry(151));
        this->dumpText->SetHorizontalAlign(pu::element::HorizontalAlign::Center);
        this->dumpText->SetColor(gsets.CustomScheme.Text);
        this->ncaBar = new pu::element::ProgressBar(340, 360, 600, 30, 100.0f);
        this->ncaBar->SetVisible(false);
        this->Add(this->dumpText);
        this->Add(this->ncaBar);
    }

    TitleDumperLayout::~TitleDumperLayout()
    {
        delete this->dumpText;
        delete this->ncaBar;
    }

    void TitleDumperLayout::StartDump(horizon::Title &Target)
    {
        EnsureDirectories();
        mainapp->CallForRender();
        FsStorageId stid = static_cast<FsStorageId>(Target.Location);
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
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            return;
        }
        NcmContentMetaDatabase cmdb;
        rc = ncmOpenContentMetaDatabase(stid, &cmdb);
        if(rc != 0)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            serviceClose(&cst.s);
            return;
        }
        NcmMetaRecord mrec;
        bool ok = dump::GetMetaRecord(&cmdb, Target.ApplicationId, &mrec);
        if(!ok)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            serviceClose(&cst.s);
            serviceClose(&cmdb.s);
            return;
        }
        NcmNcaId program;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Program, &program);
        if(!ok)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            serviceClose(&cst.s);
            serviceClose(&cmdb.s);
            return;
        }
        std::string sprogram = dump::GetNCAIdPath(&cst, &program);
        NcmNcaId meta;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Meta, &meta);
        if(!ok)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            mainapp->LoadLayout(mainapp->GetContentManagerLayout());
            serviceClose(&cst.s);
            serviceClose(&cmdb.s);
            return;
        }
        std::string smeta = dump::GetNCAIdPath(&cst, &meta);
        NcmNcaId control;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Control, &control);
        bool hascontrol = ok;
        std::string scontrol;
        if(ok) scontrol = dump::GetNCAIdPath(&cst, &control);
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

        NcmNcaId data;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Data, &data);
        bool hasdata = ok;
        std::string sdata;
        if(ok) sdata = dump::GetNCAIdPath(&cst, &data);

        std::string xprogram = sprogram;
        std::string xmeta = smeta;
        std::string xcontrol = scontrol;
        std::string xlinfo = slinfo;
        std::string xhoff = shoff;
        std::string xdata = sdata;
        if(stid == FsStorageId_SdCard)
        {
            this->dumpText->SetText(set::GetDictionaryEntry(194));
            xprogram = outdir + "/" + horizon::GetStringFromNCAId(program) + ".nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, program, xprogram, [&](double Done, double Total)
            {
                this->ncaBar->SetMaxValue(Total);
                this->ncaBar->SetProgress(Done);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xmeta = outdir + "/" + horizon::GetStringFromNCAId(meta) + ".cnmt.nca";
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, meta, xmeta, [&](double Done, double Total)
            {
                this->ncaBar->SetMaxValue(Total);
                this->ncaBar->SetProgress(Done);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            if(hascontrol)
            {
                xcontrol = outdir + "/" + horizon::GetStringFromNCAId(control) + ".nca";
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, control, xcontrol, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(haslinfo)
            {
                xlinfo = outdir + "/" + horizon::GetStringFromNCAId(linfo) + ".nca";
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, linfo, xlinfo, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(hashoff)
            {
                xhoff = outdir + "/" + horizon::GetStringFromNCAId(hoff) + ".nca";
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, hoff, xhoff, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(hasdata)
            {
                xdata = outdir + "/" + horizon::GetStringFromNCAId(data) + ".nca";
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, data, xdata, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
        }
        else
        {
            fs::Explorer *nexp = NULL;
            if(stid == FsStorageId_NandSystem) nexp = fs::GetNANDSystemExplorer();
            else if(stid == FsStorageId_NandUser) nexp = fs::GetNANDUserExplorer();
            else
            {
                HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
                mainapp->LoadLayout(mainapp->GetContentManagerLayout());
                serviceClose(&cst.s);
                serviceClose(&cmdb.s);
                return;
            }
            this->dumpText->SetText(set::GetDictionaryEntry(195));
            xprogram = nexp->FullPathFor("Contents/" + xprogram.substr(15));
            std::string txprogram = outdir + "/" + horizon::GetStringFromNCAId(program) + ".nca";
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xprogram, txprogram, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xprogram = txprogram;
            xmeta = nexp->FullPathFor("Contents/" + xmeta.substr(15));
            std::string txmeta = outdir + "/" + horizon::GetStringFromNCAId(meta) + ".cnmt.nca";
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xmeta, txmeta, [&](u8 p)
            {
                this->ncaBar->SetProgress(p);
                mainapp->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xmeta = txmeta;
            if(hascontrol)
            {
                xcontrol = nexp->FullPathFor("Contents/" + xcontrol.substr(15));
                std::string txcontrol = outdir + "/" + horizon::GetStringFromNCAId(control) + ".nca";
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xcontrol, txcontrol, [&](u8 p)
                {
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xcontrol = txcontrol;
            }
            if(haslinfo)
            {
                xlinfo = nexp->FullPathFor("Contents/" + xlinfo.substr(15));
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
                xhoff = nexp->FullPathFor("Contents/" + xhoff.substr(15));
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
            if(hasdata)
            {
                xdata = nexp->FullPathFor("Contents/" + xdata.substr(15));
                std::string txdata = outdir + "/" + horizon::GetStringFromNCAId(data) + ".nca";
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xdata, txdata, [&](u8 p)
                {
                    this->ncaBar->SetProgress(p);
                    mainapp->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xdata = txdata;
            }
        }
        std::string fout = "sdmc:/goldleaf/dump/" + fappid + ".nsp";
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
        if(ok) mainapp->ShowNotification(set::GetDictionaryEntry(197) + " '" + fout + "'");
        else
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotBuildNSP), set::GetDictionaryEntry(198));
            fs::DeleteDirectory("sdmc:/goldleaf/dump");
            EnsureDirectories();
        }
        serviceClose(&cst.s);
        serviceClose(&cmdb.s);
    }
}