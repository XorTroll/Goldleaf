#include <gleaf/lyt/BNTX.hpp>
using namespace std;

namespace gleaf::lyt
{
    QuickBntx::QuickBntx(Buffer &Reader)
    {
        if (Reader.readStr(4) != "BNTX")
            throw "Wrong magic";

        Reader.readInt32();
        s32 DataLength = Reader.readInt32();
        u16 ByteOrderMark = Reader.readUInt16();
        u16 FormatRevision = Reader.readUInt16();
        s32 NameAddress = Reader.readInt32();
        s32 StringsAddress = Reader.readInt32() >> 16;
        s32 RelocAddress = Reader.readInt32();
        s32 FileLength = Reader.readInt32();

        if (Reader.readStr(4) != "NX  ")
            throw "Wrong magic";

        u32 TexturesCount = Reader.readUInt32();
        s64 InfoPtrsAddress = Reader.readInt64();
        s64 DataBlkAddress = Reader.readInt64();
        s64 DictAddress = Reader.readInt64();
        u32 StrDictLength = Reader.readUInt32();

        Reader.Position = InfoPtrsAddress;
        auto FirstBrti = (int)Reader.readInt64();
        Reader.Position = 0;
        Head = Reader.readBytes(FirstBrti);

        for (u32 Index = 0; Index < TexturesCount; Index++)
        {
            Reader.Position = InfoPtrsAddress + Index * 8;
            Reader.Position = Reader.readInt64();

            Textures.push_back(BRTI(Reader));
        }

        Reader.Position = RelocAddress;
        Rlt = Reader.readBytes((u32)(Reader.Length() - Reader.Position));
    }

    vector<u8> QuickBntx::Write() 
    {
        Buffer bin;
        bin.ByteOrder = Endianness::LittleEndian;
        bin.Write(Head);
        vector<s64> TexPositions;
        for (auto t : Textures)
        {
            TexPositions.push_back(bin.Position);
            bin.Write(t.Write());
        }
        auto DataStart = bin.Position;
        vector<s64> TexDataPositions;
        bin.WriteAlign(0x10);
        bin.Write("BRTD");
        bin.Write((s32)0);
        bin.Write((s32)0);
        bin.Write((s32)0);
        for(auto t : Textures)
        {
            TexDataPositions.push_back(bin.Position);
            bin.Write(t.Data);
            bin.WriteAlign(0x10);
        }
        bin.WriteAlign(0x1000);
        u32 rltPos = (u32)bin.Position;
        bin.Write(Rlt);
        //Update offsets
        bin.Position = 0x18;
        bin.Write((u32)rltPos);
        bin.Write((u32)bin.Length());
        bin.Position = TexDataPositions[0] - 8;
        bin.Write((long long int)(rltPos - (TexDataPositions[0] - 0x10)));
        for (u32 i = 0; i < TexPositions.size(); i++)
        {
            bin.Position = TexPositions[i] + 0x2A0;
            bin.Write((long long int)TexDataPositions[i]);
        }
        bin.Position = rltPos + 4;
        bin.Write(rltPos);
        return bin.getBuffer();
    }

    BRTI* QuickBntx::FindTex(const string &name)
    {
        for (u32 i = 0; i < Textures.size(); i++)
            if (Textures[i].Name() == name) return &Textures[i];
        return nullptr;
    }

    void QuickBntx::ReplaceTex(const string &name, const lyt::DDSLoadResult &tex) 
    {
        auto target = FindTex(name);
        if (target == nullptr)
            throw "Couldn't find texture";
        target->Data = lyt::EncodeTex(tex);
        target->TextureType = (s32)TextureType::Image2D;
        target->Format = 0x00001a01;
        target->ChannelTypes = 0x05040302;
        target->Width = tex.width;
        target->Height = tex.height;
        target->TileMode = 0;
        target->SwizzleSize = 0;
        target->Reversed1A = 0;
        target->Reserved4C = 0;
        target->Reserved48 = 0;
        target->Reserved44 = 0;
        target->Reserved40 = 0;
        target->Reserved3C = 0;
        target->Reserved38 = 0x00010007;
        target->MipmapCount = 1;
        target->Flags = 0x01;
        target->Depth = 1;
        target->BlockHeightLog2 = 4;
        target->Alignment = 0x200;
        target->AccessFlags = 0x20;
    }
}