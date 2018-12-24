
#pragma once
#include <vector>
#include <string>
#include <tuple>
#include <gleaf/Buffer.hpp>

namespace gleaf::lyt 
{
	struct DDSLoadResult 
	{
		s32 width;
		s32 height;
		s32 format_;
		std::string fourcc;
		s32 size;
		std::vector<s32> compSel;
		s32 numMips;
		std::vector<u8> data;
	};

	std::vector<u8> EncodeTex(const DDSLoadResult &img);
	DDSLoadResult LoadDDS(const std::vector<u8> &inb);
}