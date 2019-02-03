#include <gleaf/theme/Common.hpp>
using namespace std;
using namespace gleaf::lyt;
using namespace gleaf::sarc;

namespace gleaf::theme
{
    const string CoreVer = "3.4 (C++)";

    string GeneratePatchListString(const vector < PatchTemplate >& templates) 
    {
        string curSection = "";
        string FileList = "";
        for (auto p : templates)
        {
            FileList += "["+ p.FirmName+"] "+ p.TemplateName +" : the file is called "+ p.szsName+" from title "+ p.TitleId +"\n";
        }
        return FileList;
    }

    BflytFile::PatchResult PatchLayouts(SARC::SarcData &sarc, const vector<LayoutFilePatch>& layouts)
    {
        for (auto p : layouts)
        {
            if (!sarc.files.count(p.FileName))
                return BflytFile::PatchResult::Fail;
            BflytFile target(sarc.files[p.FileName]);
            auto res = target.ApplyLayoutPatch(p.Patches);
            if (res != BflytFile::PatchResult::OK)
                return res;
            sarc.files[p.FileName] = target.SaveFile();
        }
        return BflytFile::PatchResult::OK;
    }

    BflytFile::PatchResult PatchBgLayouts(SARC::SarcData &sarc, const PatchTemplate& templ) 
    {
        BflytFile MainFile(sarc.files[templ.MainLayoutName]);
        auto res = MainFile.PatchBgLayout(templ);
        if (res == BflytFile::PatchResult::OK)
        {
            sarc.files[templ.MainLayoutName] = MainFile.SaveFile();
            for (auto f : templ.SecondaryLayouts)
            {
                BflytFile curTarget(sarc.files[f]);
                curTarget.PatchTextureName(templ.MaintextureName, templ.SecondaryTexReplace);
                sarc.files[f] = curTarget.SaveFile();
            }
        }
        return res;
    }

    BflytFile::PatchResult PatchBntx(SARC::SarcData &sarc, const vector<u8> &DDS, const PatchTemplate &targetPatch) 
    {
        Buffer Reader(sarc.files["timg/__Combined.bntx"]);
        QuickBntx q(Reader);
        if (q.Rlt.size() != 0x80)
        {
            return BflytFile::PatchResult::Fail;
        }
        auto dds = LoadDDS(DDS);
        q.ReplaceTex(targetPatch.MaintextureName, dds);
        sarc.files["timg/__Combined.bntx"] = q.Write();
        return BflytFile::PatchResult::OK;
    }

    PatchTemplate DetectSarc(const SARC::SarcData &sarc)
    {
    #define SzsHasKey(_str) (sarc.files.count(_str))

        if (!SzsHasKey("timg/__Combined.bntx"))
            return {};

        for (auto p : DefaultTemplates)
        {
            if (!SzsHasKey(p.MainLayoutName))
                continue;
            bool isTarget = true;
            for (string s : p.SecondaryLayouts)
            {
                if (!SzsHasKey(s))
                {
                    isTarget = false;
                    break;
                }
            }
            if (!isTarget) continue;
            for(string s : p.FnameIdentifier)
            {
                if (!SzsHasKey(s))
                {
                    isTarget = false;
                    break;
                }
            }
            if (!isTarget) continue;
            for (string s : p.FnameNotIdentifier)
            {
                if (SzsHasKey(s))
                {
                    isTarget = false;
                    break;
                }
            }
            if (!isTarget) continue;
            return p;
        }
        return {};
    #undef SzsHasKey
    }
}