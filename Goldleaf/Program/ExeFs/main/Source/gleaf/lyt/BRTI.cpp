#include <gleaf/lyt/BRTI.hpp>
using namespace std;

namespace gleaf::lyt
{
    BRTI::BRTI(Buffer &Reader)
    {
        auto startPos = Reader.Position;
        if (Reader.readStr(4) != "BRTI")
            throw "Wrong magic";

        BRTILength0 = Reader.readInt32();
        BRTILength1 = Reader.readInt64();
        Flags = Reader.readUInt8();
        Dimensions = Reader.readUInt8();
        TileMode = Reader.readUInt16();
        SwizzleSize = Reader.readUInt16();
        MipmapCount = Reader.readUInt16();
        MultiSampleCount = Reader.readUInt16();
        Reversed1A = Reader.readUInt16();
        Format = Reader.readUInt32();
        AccessFlags = Reader.readUInt32();
        Width = Reader.readInt32();
        Height = Reader.readInt32();
        Depth = Reader.readInt32();
        ArrayCount = Reader.readInt32();
        BlockHeightLog2 = Reader.readInt32();
        Reserved38 = Reader.readInt32();
        Reserved3C = Reader.readInt32();
        Reserved40 = Reader.readInt32();
        Reserved44 = Reader.readInt32();
        Reserved48 = Reader.readInt32();
        Reserved4C = Reader.readInt32();
        DataLength = Reader.readInt32();
        Alignment = Reader.readInt32();
        ChannelTypes = Reader.readInt32();
        TextureType = Reader.readInt32();
        NameAddress = Reader.readInt64();
        ParentAddress = Reader.readInt64();
        PtrsAddress = Reader.readInt64();

        ExtraBrtiData = Reader.readBytes((int)(BRTILength1 - (Reader.Position - startPos)));

        Reader.Position = NameAddress;

        _readonly_name = Reader.readStr_U16Prefix();

        vector<u64> MipOffsets;

        Reader.Position = PtrsAddress;

        long BaseOffset = Reader.readInt64();

        for (int Mip = 1; Mip < MipmapCount; Mip++)
            throw "mipmaps are not supported";

        Reader.Position = BaseOffset;

        Data = Reader.readBytes(DataLength);
    }

    vector<u8> BRTI::Write()
    {
        Buffer bin;
        bin.ByteOrder = Endianness::LittleEndian;
        bin.Write("BRTI");
        bin.Write(BRTILength0);
        bin.Write(BRTILength1);
        bin.Write(Flags);
        bin.Write(Dimensions);
        bin.Write(TileMode);
        bin.Write(SwizzleSize);
        bin.Write(MipmapCount);
        bin.Write(MultiSampleCount);
        bin.Write(Reversed1A);
        bin.Write(Format);
        bin.Write(AccessFlags);
        bin.Write(Width);
        bin.Write(Height);
        bin.Write(Depth);
        bin.Write(ArrayCount);
        bin.Write(BlockHeightLog2);
        bin.Write(Reserved38);
        bin.Write(Reserved3C);
        bin.Write(Reserved40);
        bin.Write(Reserved44);
        bin.Write(Reserved48);
        bin.Write(Reserved4C);
        bin.Write((s32)Data.size());
        bin.Write(Alignment);
        bin.Write(ChannelTypes);
        bin.Write(TextureType);
        bin.Write(NameAddress);
        bin.Write((long long int)ParentAddress);
        bin.Write((long long int)PtrsAddress);
        bin.Write(ExtraBrtiData);
        return bin.getBuffer();
    }

    string BRTI::Name() { return _readonly_name; }

    ChannelType BRTI::Channel0Type() { return (ChannelType)((ChannelTypes >> 0) & 0xff); }
    ChannelType BRTI::Channel3Type() { return (ChannelType)((ChannelTypes >> 8) & 0xff); }
    ChannelType BRTI::Channel1Type() { return (ChannelType)((ChannelTypes >> 16) & 0xff); }
    ChannelType BRTI::Channel2Type() { return (ChannelType)((ChannelTypes >> 24) & 0xff); }

    TextureType BRTI::Type() { return (lyt::TextureType)TextureType; }
    TextureFormatType BRTI::FormatType() { return (TextureFormatType)((Format >> 8) & 0xff); }
    TextureFormatVar  BRTI::FormatVariant() { return (TextureFormatVar)((Format >> 0) & 0xff); }
}