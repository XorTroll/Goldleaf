#include <map>
#include <unordered_map>
#include <gleaf/theme/NXTheme.hpp>
#include <gleaf/Buffer.hpp>
#include <gleaf/theme/Common.hpp>
using namespace std;
using json = nlohmann::json;
using namespace gleaf::lyt;
using namespace gleaf::sarc;

namespace gleaf::theme
{
    ThemeFileManifest ParseNXThemeFile(SARC::SarcData &Archive)
    {
        if (!Archive.files.count("info.json") || !Archive.files.count("image.dds"))
        {
            return {-1,"","",""};
        }
        string jsn(reinterpret_cast<char*>((Archive.files["info.json"]).data()),(Archive.files["info.json"]).size());
        auto j = json::parse(jsn);
        
        ThemeFileManifest res = {0};
        if (j.count("Version") && j.count("Target"))
        {
            res.Version = j["Version"].get<int>();
            res.Target = j["Target"].get<string>();
        }
        else 
        {
            res.Version = -1;
            return res;
        }
        if (j.count("Author"))
            res.Author = j["Author"].get<string>();
        if (j.count("ThemeName"))
            res.ThemeName = j["ThemeName"].get<string>();
        if (j.count("LayoutInfo"))
            res.LayoutInfo = j["LayoutInfo"].get<string>();	
        
        if (j.count("UseCommon5X"))
            res.UseCommon5X = j["UseCommon5X"].get<bool>();
        else 
            res.UseCommon5X = true;
        
        return res;
    }
}