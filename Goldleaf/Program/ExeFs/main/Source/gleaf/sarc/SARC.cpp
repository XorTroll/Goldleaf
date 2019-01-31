#include <gleaf/sarc/SARC.hpp>
using namespace std;

namespace gleaf::sarc
{
    class SFAT
    {
    public:
        struct Node 
        {
            u32 Hash;
            u8 fileBool;
            u8 unknown1;
            u16 fileNameOffset;
            u32 nodeOffset;
            u32 EON;
        };

        vector<Node> Nodes;
        u16 chunkSize;
        u16 nodeCount;
        u32 hashMultiplier;
        
        SFAT(Buffer &br)
        {
            br.readInt32(); //Header
            chunkSize = br.readUInt16();
            nodeCount = br.readUInt16();
            hashMultiplier = br.readUInt32();
            for (int i = 0; i < nodeCount; i++)
            {
                Node node;
                node.Hash = br.readUInt32();
                auto attributes = br.readUInt32();
                node.fileBool = (u8)(attributes >> 24);
                node.unknown1 = (u8)((attributes >> 16) & 0xFF);
                node.fileNameOffset = (u16)(attributes & 0xFFFF);
                node.nodeOffset = br.readUInt32();
                node.EON = br.readUInt32();
                Nodes.push_back(node);
            }
        }
    };

    class SFNT
    {
    public:
        vector<string> fileNames;
        u32 chunkID;
        u16 chunkSize;
        u16 unknown1;

        SFNT(Buffer &br, u32 DataStart)
        {
            chunkID = br.readUInt32();
            chunkSize = br.readUInt16();
            unknown1 = br.readUInt16();

            while (DataStart - br.Position > 0)
            {
                if (br.readInt8() == 0)
                    continue;
                br.Position--;
                fileNames.push_back(br.readStr_NullTerm());
            }
        }
    };

    u32 SARC::NameHash(string name)
    {
        u32 result = 0;
        for (int i = 0; i < name.length(); i++)
        {
            result = name[i] + result * 0x00000065;
        }
        return result;
    }

    u32 SARC::StringHashToUint(string name)
    {
        return (u32)std::stoul(name);
    }

    bool FormatMatches(const vector<u8> &f, int startIndex, const string header)
    {
        auto strLen = header.length();
        if (f.size() < strLen + startIndex) return false;
        for (int i = 0; i < strLen; i++)
        {
            if (f[i + startIndex] != header[i]) return false;
        }
        return true;
    }
    bool FormatMatches(const vector<u8> &f, const string header){return FormatMatches(f, 0, header);}

    string SARC::GuessFileExtension(vector<u8> &f)
    {
        string Ext = ".bin";

        if (FormatMatches(f,"SARC")) Ext = ".sarc";
        else if (FormatMatches(f,"Yaz")) Ext = ".szs";
        else if (FormatMatches(f,"YB") || FormatMatches(f,"BY")) Ext = ".byaml";
        else if (FormatMatches(f,"FRES")) Ext = ".bfres";
        else if (FormatMatches(f,"Gfx2")) Ext = ".gtx";
        else if (FormatMatches(f,"FLYT")) Ext = ".bflyt";
        else if (FormatMatches(f,"CLAN")) Ext = ".bclan";
        else if (FormatMatches(f,"CLYT")) Ext = ".bclyt";
        else if (FormatMatches(f,"FLIM")) Ext = ".bclim";
        else if (FormatMatches(f,"FLAN")) Ext = ".bflan";
        else if (FormatMatches(f,"FSEQ")) Ext = ".bfseq";
        else if (FormatMatches(f,"VFXB")) Ext = ".pctl";
        else if (FormatMatches(f,"AAHS")) Ext = ".sharc";
        else if (FormatMatches(f,"BAHS")) Ext = ".sharcb";
        else if (FormatMatches(f,"BNTX")) Ext = ".bntx";
        else if (FormatMatches(f,"BNSH")) Ext = ".bnsh";
        else if (FormatMatches(f,"FSHA")) Ext = ".bfsha";
        else if (FormatMatches(f,"FFNT")) Ext = ".bffnt";
        else if (FormatMatches(f,"CFNT")) Ext = ".bcfnt";
        else if (FormatMatches(f,"CSTM")) Ext = ".bcstm";
        else if (FormatMatches(f,"FSTM")) Ext = ".bfstm";
        else if (FormatMatches(f,"STM")) Ext = ".bfsha";
        else if (FormatMatches(f,"CWAV")) Ext = ".bcwav";
        else if (FormatMatches(f,"FWAV")) Ext = ".bfwav";
        else if (FormatMatches(f,"CTPK")) Ext = ".ctpk";
        else if (FormatMatches(f,"CGFX")) Ext = ".bcres";
        else if (FormatMatches(f,"AAMP")) Ext = ".aamp";
        else if (FormatMatches(f,"MsgStdBn")) Ext = ".msbt";
        else if (FormatMatches(f,"MsgPrjBn")) Ext = ".msbp";
        return Ext;
    }

    u32 SARC::GuessFileAlignment(vector<u8> &file) 
    {
        if (FormatMatches(file,"SARC")) return 0x2000;
        else if (FormatMatches(file,"Yaz")) return 0x80;
        else if (FormatMatches(file,"YB") || FormatMatches(file,"BY")) return 0x80;
        else if (FormatMatches(file,"FRES") || FormatMatches(file,"Gfx2") || FormatMatches(file,"AAHS") || FormatMatches(file,"BAHS")) return 0x2000;
        else if (FormatMatches(file,"BNTX") || FormatMatches(file,"BNSH") || FormatMatches(file,"FSHA")) return 0x1000;
        else if (FormatMatches(file,"FFNT")) return 0x2000;
        else if (FormatMatches(file,"CFNT")) return 0x80;
        else if (FormatMatches(file,1, "STM") /* *STM */ || FormatMatches(file,1, "WAV") || FormatMatches(file,"FSTP")) return 0x20;
        else if (FormatMatches(file,"CTPK")) return 0x10;
        else if (FormatMatches(file,"CGFX")) return 0x80;
        else if (FormatMatches(file,"AAMP")) return 8;
        else if (FormatMatches(file,"MsgStdBn") || FormatMatches(file,"MsgPrjBn")) return 0x80;
        else return 0x4;
    }

    u32 SARC::GuessAlignment(unordered_map<string, vector<u8>> &files) //From https://github.com/aboood40091/SarcLib/blob/master/src/FileArchive.py#L487
    {
        u32 res = 4;
        for (auto value : files)
        {
            u32 fileRes = GuessFileAlignment(value.second);
            res = fileRes > res ? fileRes : res;
        }
        return res;
    }

    SARC::PackedSarc SARC::Pack(SARC::SarcData &data, s32 _align) 
    {
        int align = _align >= 0 ? _align : (int)GuessAlignment(data.files);
        Buffer bw;
        bw.ByteOrder = data.endianness;
        bw.Write((string)"SARC");
        bw.Write((u16)0x14); // Chunk length
        bw.Write((u16)0xFEFF); // BOM
        bw.Write((u32)0x00); //filesize update later
        bw.Write((u32)0x00); //Beginning of data
        bw.Write((u16)0x100);
        bw.Write((u16)0x00);
        bw.Write((string)"SFAT");
        bw.Write((u16)0xc);
        bw.Write((u16)data.names.size());
        bw.Write((u32)0x00000065);
        vector<u32> offsetToUpdate;
        offsetToUpdate.reserve(data.names.size());
        for (auto f : data.names)
        {
            if (data.HashOnly)
                bw.Write(StringHashToUint(f));
            else
                bw.Write(NameHash(f));
            offsetToUpdate.push_back((u32)bw.Position);
            bw.Write((u32)0);
            bw.Write((u32)0);
            bw.Write((u32)0);
        }
        bw.Write((string)"SFNT");
        bw.Write((u16)0x8);
        bw.Write((u16)0);
        vector<u32> StringOffsets;
        StringOffsets.reserve(data.names.size());
        for (auto f : data.names)
        {
            StringOffsets.push_back((u32)bw.Position);
            bw.Write(f, Buffer::BinaryString::NullTerminated);
            bw.WriteAlign(4);
        }
        bw.WriteAlign(0x1000); //TODO: check if works in odyssey
        vector<u32> FileOffsets;
        FileOffsets.reserve(data.names.size());
        vector<string> OrderedKeys;
        OrderedKeys.reserve(data.names.size());
        for (auto f : data.names)
        {
            bw.WriteAlign((int)GuessFileAlignment(data.files[f]));
            FileOffsets.push_back((u32)bw.Position);
            bw.Write(data.files[f]);
            OrderedKeys.push_back(f);
        }
        for (int i = 0; i < offsetToUpdate.size(); i++)
        {
            bw.Position = offsetToUpdate[i];
            if (!data.HashOnly)
                bw.Write(0x01000000 | ((StringOffsets[i] - StringOffsets[0]) / 4));
            else
                bw.Write((u32)0);
            bw.Write((u32)(FileOffsets[i] - FileOffsets[0]));
            bw.Write((u32)(FileOffsets[i] + data.files[OrderedKeys[i]].size() - FileOffsets[0]));
        }
        bw.Position = 0x08;
        bw.Write((u32)bw.Length());
        bw.Write((u32)FileOffsets[0]);

        return PackedSarc{bw.getBuffer(), (u32)align};
    }

    SARC::SarcData SARC::Unpack(vector<u8> &data) 
    {
        SARC::SarcData res;
        Buffer buf = Buffer(data);
        buf.ByteOrder = Endianness::LittleEndian;
        buf.Position = 6;
        if (buf.readUInt16() == 0xFFFE)
            buf.ByteOrder = Endianness::BigEndian;
        buf.Position = 0;
        if (buf.readStr(4) != "SARC")
            throw std::runtime_error("Wrong magic");

        buf.readUInt16(); // Chunk length
        buf.readUInt16(); // BOM
        buf.readUInt32(); // File size
        u32 startingOff = buf.readUInt32();
        buf.readUInt32(); // Unknown;
        SFAT sfat(buf);
        SFNT sfnt(buf,startingOff);
        
        bool HashOnly = false;
        if (sfat.nodeCount > 0)
        {
            if (sfat.Nodes[0].fileBool != 1) HashOnly = true;
        }

        for (int m = 0; m < sfat.nodeCount; m++)
        {
            buf.Position = sfat.Nodes[m].nodeOffset + startingOff;
            vector<u8> temp;
            if (m == 0)
            {
                temp = buf.readBytes(sfat.Nodes[m].EON);
            }
            else
            {
                u32 tempInt = sfat.Nodes[m].EON - (int)sfat.Nodes[m].nodeOffset;
                temp = buf.readBytes(tempInt);
            }

            string name = "";
            if (sfat.Nodes[m].fileBool == 1)
                name = sfnt.fileNames[m];
            else
                name = std::to_string(sfat.Nodes[m].Hash) + GuessFileExtension(temp);

            res.files[name] = temp;
            res.names.push_back(name);
        }
        res.HashOnly = HashOnly;
        res.endianness = buf.ByteOrder;
        return res;
    }
}