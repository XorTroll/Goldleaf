#include <gleaf/lyt/DDS.hpp>
#include <algorithm>
#include <cstring>
using namespace std;

//Utils

s32 DIV_ROUND_UP(s32 n, s32 d) {return (n + d - 1) / d; }

s32 round_up(s32 x, s32 y) { return ((x - 1) | (y - 1)) + 1; }

s32 pow2_round_up(s32 x)
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

s32 Log2(s32 v)
{
	s32 r = 0xFFFF - v >> 31 & 0x10;
	v >>= r;
	s32 shift = 0xFF - v >> 31 & 0x8;
	v >>= shift;
	r |= shift;
	shift = 0xF - v >> 31 & 0x4;
	v >>= shift;
	r |= shift;
	shift = 0x3 - v >> 31 & 0x2;
	v >>= shift;
	r |= shift;
	r |= (v >> 1);
	return r;
}

s32 getBlockHeight(s32 height)
{
	s32 blockHeight = pow2_round_up(height / 8);

	if (blockHeight > 16)
		blockHeight = 16;

	return blockHeight;
}

s32 getAddrBlockLinear(s32 x, s32 y, s32 image_width, s32 bytes_per_pixel, s32 base_address, s32 blockHeight)
{
	auto image_width_in_gobs = DIV_ROUND_UP(image_width * bytes_per_pixel, 64);
	auto GOB_address = (base_address
		+ (y / (8 * blockHeight)) * 512 * blockHeight * image_width_in_gobs
		+ (x * bytes_per_pixel / 64) * 512 * blockHeight
		+ (y % (8 * blockHeight) / 8) * 512);
	x *= bytes_per_pixel;
	return (GOB_address + ((x % 64) / 32) * 256 + ((y % 8) / 2) * 64 + ((x % 32) / 16) * 32 + (y % 2) * 16 + (x % 16));
}

vector<u8> swizzle(s32 width, s32 height, s32 blkWidth, s32 blkHeight, bool roundPitch, s32 bpp, s32 tileMode, s32 blockHeightLog2, const vector<u8> &data, s32 toSwizzle)
{
	auto blockHeight = 1 << blockHeightLog2;
	width = DIV_ROUND_UP(width, blkWidth);
	height = DIV_ROUND_UP(height, blkHeight);

	auto pitch = -1;
	auto surfSize = -1;
	if (tileMode == 1)
	{
		pitch = width * bpp;
		if (roundPitch)
			pitch = round_up(pitch, 32);
		surfSize = pitch * height;
	}
	else
	{
		pitch = round_up(width * bpp, 64);
		surfSize = pitch * round_up(height, blockHeight * 8);
	}
	vector<u8> res(surfSize);
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			auto pos = -1;
			if (tileMode == 1)
				pos = y * pitch + x * bpp;
			else
				pos = getAddrBlockLinear(x, y, width, bpp, 0, blockHeight);
			auto pos_ = (y * width + x) * bpp;
			if (pos + bpp <= surfSize)
			{
				if (toSwizzle)
					memcpy(res.data() + pos, data.data() + pos_, bpp);
				else
					memcpy(res.data() + pos_, data.data() + pos, bpp);
			}
		}
	return res;
}

tuple<s32, s32> getCurrentMipOffset_Size(s32 width, s32 height, s32 blkWidth, s32 blkHeight, s32 bpp, s32 currLevel)
{
    s32 offset = 0;
    s32 width_ = 0;
    s32 height_ = 0;

    for (int mipLevel = 0; mipLevel < currLevel; mipLevel++)
    {
        width_ = DIV_ROUND_UP(max(1, width >> mipLevel), blkWidth);
        height_ = DIV_ROUND_UP(max(1, height >> mipLevel), blkHeight);
        offset += width_ * height_ * bpp;
    }

    width_ = DIV_ROUND_UP(max(1, width >> currLevel), blkWidth);
    height_ = DIV_ROUND_UP(max(1, height >> currLevel), blkHeight);
    auto size = width_ * height_ * bpp;

    return tuple<s32, s32>(offset, size);
}

s32 ToInt32(const u8* ptr)
{
    union { s32 val; u8 chars[4]; } p;
    p.chars[0] = ptr[0];
    p.chars[1] = ptr[1];
    p.chars[2] = ptr[2];
    p.chars[3] = ptr[3];
    return p.val;
}

//lyt namespace

namespace gleaf::lyt
{
    DDSLoadResult LoadDDS(const vector<u8> &inb) 
    {
        if (!(inb[0x54] == 'D' && inb[0x55] == 'X' && inb[0x56] == 'T' && inb[0x57] == '1'))
            throw "Unsupported format : only DXT1 encoding is supported for DDS";

        auto format_ = 0x1a06;
        auto bpp = 8;
        auto width = ToInt32(inb.data() + 0x10);
        auto height = ToInt32(inb.data() + 0xC);
        auto size = ((width + 3) >> 2) * ((height + 3) >> 2) * bpp;
        auto numMips = 0;
        auto mipSize = 0;
        vector<u8> res(size + mipSize);
        memcpy(res.data(), inb.data() + 0x80, size + mipSize);
        return DDSLoadResult{
            width,	height,	format_,
            "DXT1",	size,	{ 2, 3, 4, 5 },
            numMips,	res
        };
    }

    vector<u8> EncodeTex(const DDSLoadResult &img)
    {
        auto numMips = 1;
        auto alignment = 512;
        auto blkWidth = 4;
        auto blkHeight = 4;
        auto bpp = 8;
        auto blockHeight = getBlockHeight(DIV_ROUND_UP(img.height, blkHeight));
        auto blockHeightLog2 = Log2(blockHeight);
        auto linesPerBlockHeight = blockHeight * 8;
        
        auto surfSize = 0;
        auto blockHeightShift = 0;

        vector<s32> mipOfsets;
        vector<u8> res;

        for (s32 mipLevel = 0; mipLevel < numMips; mipLevel++)
        {
            auto offSize = getCurrentMipOffset_Size(img.width, img.height, blkWidth, blkHeight, bpp, mipLevel);
            vector<u8> data(get<1>(offSize));
            memcpy(data.data(), img.data.data() + get<0>(offSize), get<1>(offSize));
            auto width_ = max(1, img.width >> mipLevel);
            auto height_ = max(1, img.height >> mipLevel);
            auto width__ = DIV_ROUND_UP(width_, blkWidth);
            auto height__ = DIV_ROUND_UP(height_, blkHeight);
            int dataAlignBytes = round_up(surfSize, alignment) - surfSize;
            surfSize += dataAlignBytes;
            mipOfsets.push_back(surfSize);
            if (pow2_round_up(height__) < linesPerBlockHeight)
                blockHeightShift += 1;
            auto pitch = round_up(width__ * bpp, 64);
            surfSize += pitch * round_up(height__, max(1, blockHeight >> blockHeightShift) * 8);

            if (dataAlignBytes != 0) 
            {
                for (int i = 0; i < dataAlignBytes; i++)
                    res.push_back(0);
            }
            auto tmpVec = swizzle(width_, height_, blkWidth, blkHeight, true, bpp, 0, max(0, blockHeightLog2 - blockHeightShift), data, true);
            res.insert(res.end(), tmpVec.begin(), tmpVec.end());
        }

        return res;
    }
}