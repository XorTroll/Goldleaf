
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

#include <ui/ui_TitleDumperLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern ui::MainApplication::Ref global_app;
extern set::Settings global_settings;

namespace ui
{
    TitleDumperLayout::TitleDumperLayout()
    {
        this->dumpText = pu::ui::elm::TextBlock::New(150, 320, set::GetDictionaryEntry(151));
        this->dumpText->SetHorizontalAlign(pu::ui::elm::HorizontalAlign::Center);
        this->dumpText->SetColor(global_settings.custom_scheme.Text);
        this->ncaBar = pu::ui::elm::ProgressBar::New(340, 360, 600, 30, 100.0f);
        this->ncaBar->SetVisible(false);
        global_settings.ApplyProgressBarColor(this->ncaBar);
        this->Add(this->dumpText);
        this->Add(this->ncaBar);
    }

    void TitleDumperLayout::StartDump(hos::Title &Target, bool HasTicket)
    {
        EnsureDirectories();
        global_app->CallForRender();
        NcmStorageId stid = static_cast<NcmStorageId>(Target.Location);
        String fappid = hos::FormatApplicationId(Target.ApplicationId);
        String outdir = "sdmc:/" + consts::Root + "/dump/title/" + fappid;
        fs::CreateDirectory(outdir);
        this->dumpText->SetText(set::GetDictionaryEntry(192));
        global_app->CallForRender();
        if(HasTicket) dump::GenerateTicketCert(Target.ApplicationId);
        this->dumpText->SetText(set::GetDictionaryEntry(193));
        global_app->CallForRender();
        NcmContentStorage cst;
        Result rc = ncmOpenContentStorage(&cst, stid);
        if(rc != 0)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            global_app->LoadLayout(global_app->GetContentManagerLayout());
            return;
        }
        NcmContentMetaDatabase cmdb;
        rc = ncmOpenContentMetaDatabase(&cmdb, stid);
        if(rc != 0)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            global_app->LoadLayout(global_app->GetContentManagerLayout());
            serviceClose(&cst.s);
            return;
        }
        NcmContentMetaKey mrec = Target.Record;
        NcmContentId meta;
        bool ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Meta, &meta);
        if(!ok)
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
            global_app->LoadLayout(global_app->GetContentManagerLayout());
            serviceClose(&cst.s);
            serviceClose(&cmdb.s);
            return;
        }
        String smeta = dump::GetNCAIdPath(&cst, &meta);
        NcmContentId program;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Program, &program);
        bool hasprogram = ok;
        String sprogram;
        if(ok) sprogram = dump::GetNCAIdPath(&cst, &program);
        NcmContentId control;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Control, &control);
        bool hascontrol = ok;
        String scontrol;
        if(ok) scontrol = dump::GetNCAIdPath(&cst, &control);
        NcmContentId linfo;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::LegalInfo, &linfo);
        bool haslinfo = ok;
        String slinfo;
        if(ok) slinfo = dump::GetNCAIdPath(&cst, &linfo);
        NcmContentId hoff;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::OfflineHtml, &hoff);
        bool hashoff = ok;
        String shoff;
        if(ok) shoff = dump::GetNCAIdPath(&cst, &hoff);

        NcmContentId data;
        ok = dump::GetNCAId(&cmdb, &mrec, Target.ApplicationId, dump::NCAType::Data, &data);
        bool hasdata = ok;
        String sdata;
        if(ok) sdata = dump::GetNCAIdPath(&cst, &data);

        hos::LockAutoSleep();

        String xprogram = sprogram;
        String xmeta = smeta;
        String xcontrol = scontrol;
        String xlinfo = slinfo;
        String xhoff = shoff;
        String xdata = sdata;
        if(stid == NcmStorageId_SdCard)
        {
            this->dumpText->SetText(set::GetDictionaryEntry(194));
            xmeta = outdir + "/" + hos::ContentIdAsString(meta) + ".cnmt.nca";
            fs::CreateConcatenationFile(xmeta);
            this->ncaBar->SetVisible(true);
            dump::DecryptCopyNAX0ToNCA(&cst, meta, xmeta, [&](double Done, double Total)
            {
                this->ncaBar->SetMaxValue(Total);
                this->ncaBar->SetProgress(Done);
                global_app->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            if(hasprogram)
            {
                xprogram = outdir + "/" + hos::ContentIdAsString(program) + ".nca";
                fs::CreateConcatenationFile(xprogram);
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, program, xprogram, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(hascontrol)
            {
                xcontrol = outdir + "/" + hos::ContentIdAsString(control) + ".nca";
                fs::CreateConcatenationFile(xcontrol);
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, control, xcontrol, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(haslinfo)
            {
                xlinfo = outdir + "/" + hos::ContentIdAsString(linfo) + ".nca";
                fs::CreateConcatenationFile(xlinfo);
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, linfo, xlinfo, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(hashoff)
            {
                xhoff = outdir + "/" + hos::ContentIdAsString(hoff) + ".nca";
                fs::CreateConcatenationFile(xhoff);
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, hoff, xhoff, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
            if(hasdata)
            {
                xdata = outdir + "/" + hos::ContentIdAsString(data) + ".nca";
                fs::CreateConcatenationFile(xdata);
                this->ncaBar->SetVisible(true);
                dump::DecryptCopyNAX0ToNCA(&cst, data, xdata, [&](double Done, double Total)
                {
                    this->ncaBar->SetMaxValue(Total);
                    this->ncaBar->SetProgress(Done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
            }
        }
        else
        {
            fs::Explorer *nexp = NULL;
            if(stid == NcmStorageId_BuiltInSystem) nexp = fs::GetNANDSystemExplorer();
            else if(stid == NcmStorageId_BuiltInUser) nexp = fs::GetNANDUserExplorer();
            else
            {
                HandleResult(err::Make(err::ErrorDescription::CouldNotLocateTitleContents), set::GetDictionaryEntry(198));
                global_app->LoadLayout(global_app->GetContentManagerLayout());
                serviceClose(&cst.s);
                serviceClose(&cmdb.s);
                hos::UnlockAutoSleep();
                return;
            }
            this->dumpText->SetText(set::GetDictionaryEntry(195));
            xmeta = nexp->FullPathFor("Contents/" + xmeta.substr(15));
            String txmeta = outdir + "/" + hos::ContentIdAsString(meta) + ".cnmt.nca";
            fs::CreateConcatenationFile(txmeta);
            this->ncaBar->SetVisible(true);
            fs::CopyFileProgress(xmeta, txmeta, [&](double done, double total)
            {
                this->ncaBar->SetMaxValue(total);
                this->ncaBar->SetProgress(done);
                global_app->CallForRender();
            });
            this->ncaBar->SetVisible(false);
            xmeta = txmeta;
            if(hasprogram)
            {
                xprogram = nexp->FullPathFor("Contents/" + xprogram.substr(15));
                String txprogram = outdir + "/" + hos::ContentIdAsString(program) + ".nca";
                fs::CreateConcatenationFile(txprogram);
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xprogram, txprogram, [&](double done, double total)
                {
                    this->ncaBar->SetMaxValue(total);
                    this->ncaBar->SetProgress(done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xprogram = txprogram;
            }
            if(hascontrol)
            {
                xcontrol = nexp->FullPathFor("Contents/" + xcontrol.substr(15));
                String txcontrol = outdir + "/" + hos::ContentIdAsString(control) + ".nca";
                fs::CreateConcatenationFile(txcontrol);
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xcontrol, txcontrol, [&](double done, double total)
                {
                    this->ncaBar->SetMaxValue(total);
                    this->ncaBar->SetProgress(done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xcontrol = txcontrol;
            }
            if(haslinfo)
            {
                xlinfo = nexp->FullPathFor("Contents/" + xlinfo.substr(15));
                String txlinfo = outdir + "/" + hos::ContentIdAsString(linfo) + ".nca";
                fs::CreateConcatenationFile(txlinfo);
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xlinfo, txlinfo, [&](double done, double total)
                {
                    this->ncaBar->SetMaxValue(total);
                    this->ncaBar->SetProgress(done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xlinfo = txlinfo;
            }
            if(hashoff)
            {
                xhoff = nexp->FullPathFor("Contents/" + xhoff.substr(15));
                String txhoff = outdir + "/" + hos::ContentIdAsString(hoff) + ".nca";
                fs::CreateConcatenationFile(txhoff);
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xhoff, txhoff, [&](double done, double total)
                {
                    this->ncaBar->SetMaxValue(total);
                    this->ncaBar->SetProgress(done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xhoff = txhoff;
            }
            if(hasdata)
            {
                xdata = nexp->FullPathFor("Contents/" + xdata.substr(15));
                String txdata = outdir + "/" + hos::ContentIdAsString(data) + ".nca";
                fs::CreateConcatenationFile(txdata);
                this->ncaBar->SetVisible(true);
                fs::CopyFileProgress(xdata, txdata, [&](double done, double total)
                {
                    this->ncaBar->SetMaxValue(total);
                    this->ncaBar->SetProgress(done);
                    global_app->CallForRender();
                });
                this->ncaBar->SetVisible(false);
                xdata = txdata;
            }
        }
        String fout = "sdmc:/" + consts::Root + "/dump/title/" + fappid + ".nsp";
        fs::CreateConcatenationFile(fout);
        this->ncaBar->SetVisible(true);
        this->dumpText->SetText(set::GetDictionaryEntry(196));
        ok = nsp::GenerateFrom(outdir, fout, [&](u64 done, u64 total)
        {
            this->ncaBar->SetMaxValue((double)total);
            this->ncaBar->SetProgress((double)done);
            global_app->CallForRender();
        });
        hos::UnlockAutoSleep();
        fs::DeleteDirectory("sdmc:/" + consts::Root + "/dump/temp");
        fs::DeleteDirectory(outdir);
        if(ok) global_app->ShowNotification(set::GetDictionaryEntry(197) + " '" + fout + "'");
        else
        {
            HandleResult(err::Make(err::ErrorDescription::CouldNotBuildNSP), set::GetDictionaryEntry(198));
            fs::DeleteDirectory("sdmc:/" + consts::Root + "/dump");
            EnsureDirectories();
        }
        serviceClose(&cst.s);
        serviceClose(&cmdb.s);
    }
}