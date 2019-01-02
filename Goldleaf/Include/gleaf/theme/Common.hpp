
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright © 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from exelix's source

*/

#pragma once
#include <string>
#include <vector>
#include <gleaf/sarc.hpp>
#include <gleaf/lyt.hpp>

namespace gleaf::theme
{
	extern const std::string CoreVer;
	std::string GeneratePatchListString(const std::vector <lyt::PatchTemplate>& templates);
	lyt::BflytFile::PatchResult PatchLayouts(sarc::SARC::SarcData &sarc, const std::vector<lyt::LayoutFilePatch>& layout);
	lyt::BflytFile::PatchResult PatchBgLayouts(sarc::SARC::SarcData &sarc, const lyt::PatchTemplate& layout);
	lyt::BflytFile::PatchResult PatchBntx(sarc::SARC::SarcData &sarc, const std::vector<u8> &DDS, const lyt::PatchTemplate &targetPatch);
	lyt::PatchTemplate DetectSarc(const sarc::SARC::SarcData &sarc);
}