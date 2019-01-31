
/*

    Goldleaf - Nintendo Switch homebrew multitool, for several purposes and with several features

    Copyright 2018 - 2019 Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from exelix's source

*/

#pragma once
#include <vector>
#include <string>
#include <gleaf/Buffer.hpp>

namespace gleaf::lyt
{
	enum class TextureFormatType : u32
	{
		R5G6B5 = 0x07,
		R8G8 = 0x09,
		R16 = 0x0a,
		R8G8B8A8 = 0x0b,
		R11G11B10 = 0x0f,
		R32 = 0x14,
		BC1 = 0x1a,
		BC2 = 0x1b,
		BC3 = 0x1c,
		BC4 = 0x1d,
		BC5 = 0x1e,
		ASTC4x4 = 0x2d,
		ASTC5x4 = 0x2e,
		ASTC5x5 = 0x2f,
		ASTC6x5 = 0x30,
		ASTC6x6 = 0x31,
		ASTC8x5 = 0x32,
		ASTC8x6 = 0x33,
		ASTC8x8 = 0x34,
		ASTC10x5 = 0x35,
		ASTC10x6 = 0x36,
		ASTC10x8 = 0x37,
		ASTC10x10 = 0x38,
		ASTC12x10 = 0x39,
		ASTC12x12 = 0x3a
	};

	enum class TextureFormatVar : u32
	{
		UNorm = 1,
		SNorm = 2,
		UInt = 3,
		SInt = 4,
		Single = 5,
		SRGB = 6,
		UHalf = 10
	};

	enum class TextureType : u32
	{
		Image1D = 0,
		Image2D = 1,
		Image3D = 2,
		Cube = 3,
		CubeFar = 8
	};

	enum class ChannelType
	{
		Zero,
		One,
		Red,
		Green,
		Blue,
		Alpha
	};

	class BRTI
	{
	public:
		//Header
		s32 BRTILength0;
		long long int BRTILength1;
		u8 Flags;
		u8 Dimensions;
		u16 TileMode;
		u16 SwizzleSize;
		u16 MipmapCount;
		u16 MultiSampleCount;
		u16 Reversed1A;
		u32 Format;
		u32 AccessFlags;
		s32 Width;
		s32 Height;
		s32 Depth;
		s32 ArrayCount;
		s32 BlockHeightLog2;
		s32 Reserved38;
		s32 Reserved3C;
		s32 Reserved40;
		s32 Reserved44;
		s32 Reserved48;
		s32 Reserved4C;
		s32 DataLength;
		s32 Alignment;
		s32 ChannelTypes;
		s32 TextureType;
		long long int NameAddress;
		long long int ParentAddress;
		long long int PtrsAddress;

		std::vector<u8> Data;
		std::vector<u8> ExtraBrtiData;

		std::string Name();

		ChannelType Channel0Type();
		ChannelType Channel3Type();
		ChannelType Channel1Type();
		ChannelType Channel2Type();

		lyt::TextureType Type();
		TextureFormatType FormatType();
		TextureFormatVar  FormatVariant();

		std::vector<u8> Write();
		BRTI(Buffer &Reader);

	private:
		std::string _readonly_name;
	};
}