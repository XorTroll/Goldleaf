#include <gleaf/lyt/BFLYT.hpp>
#include <stdexcept>
using namespace std;

namespace gleaf::lyt
{
    string BasePane::ToString() { return "Pane " + name + " " + to_string(length); }

    BasePane::BasePane(const string &_name, u32 len)  : name(_name), length(len), data(len) {}

    BasePane::BasePane(const BasePane &ref) : name(ref.name), length(ref.length), data(ref.data) {}

    BasePane::BasePane(const string &_name, Buffer &reader) : name(_name)
    {
        length = reader.readInt32();
        data = reader.readBytes(length - 8);
    }

    void BasePane::WritePane(Buffer &writer)
    {
        writer.Write(name);
        length = data.size() + 8;
        writer.Write(length);
        writer.Write(data);
    }

    string PropertyEditablePane::ToString() { return "Pane " + name + " " + PaneName; }
    bool PropertyEditablePane::GetVisible() {return (_flag1 & 0x1) == 0x1;}

    void PropertyEditablePane::SetVisible(bool value)
    {
        if (value)
            _flag1 |= 0x1;
        else
            _flag1 &= 0xFE;
    }

    Vector3 ReadVec3(Buffer &buf)
    {
        Vector3 res;
        res.X = buf.readFloat();
        res.Y = buf.readFloat();
        res.Z = buf.readFloat();
        return res;
    }

    Vector2 ReadVec2(Buffer &buf)
    {
        Vector2 res;
        res.X = buf.readFloat();
        res.Y = buf.readFloat();
        return res;
    }

    PropertyEditablePane::PropertyEditablePane(const BasePane &p) : BasePane(p)
    {
        Buffer buf(p.data);
        buf.ByteOrder = Endianness::LittleEndian;
        _flag1 = buf.readUInt8();
        buf.Position += 3;
        PaneName = buf.readStr_NullTerm(0x18);
        buf.Position = 0x2c - 8;
        Position = ReadVec3(buf);
        Rotation = ReadVec3(buf);
        Scale = ReadVec2(buf);
        Size = ReadVec2(buf);
        if (name == "pic1")
        {
            buf.Position = 0x54 - 8;
            ColorData = buf.ReadU32Array(4);
        }
    }

    #define WriteVec3(_x) bin.Write(_x.X);bin.Write(_x.Y);bin.Write(_x.Z);
    #define WriteVec2(_x) bin.Write(_x.X);bin.Write(_x.Y);

    void PropertyEditablePane::ApplyChanges() 
    {
        Buffer bin(data);
        bin.Position = 0;
        bin.Write(_flag1);
        bin.Position = 0x2C - 8;
        WriteVec3(Position)
        WriteVec3(Rotation)
        WriteVec2(Scale)
        WriteVec2(Size)
        if (name == "pic1")
        {
            bin.Position = 0x54 - 8;
            bin.WriteU32Array(ColorData);
        }
        data = bin.getBuffer();
    }

    void PropertyEditablePane::WritePane(Buffer &writer) 
    {
        ApplyChanges();
        BasePane::WritePane(writer);
    }

    TextureSection::TextureSection(Buffer &buf) : BasePane("txl1",buf)
    {
        Buffer rd(data);
        rd.ByteOrder = Endianness::LittleEndian;
        int texCount = rd.readInt32();
        u32 BaseOff = rd.Position;
        auto Offsets = rd.ReadS32Array(texCount);
        for (auto off : Offsets)
        {
            rd.Position = BaseOff + off;
            Textures.push_back(rd.readStr_NullTerm());
        }
    }

    TextureSection::TextureSection() : BasePane("txl1", 8) {}

    void TextureSection::WritePane(Buffer &writer)
    {
        Buffer dataWriter;
        dataWriter.ByteOrder = writer.ByteOrder;
        dataWriter.Write((s32)Textures.size());
        for (int i = 0; i < Textures.size(); i++)
            dataWriter.Write((s32)0);
        for (int i = 0; i < Textures.size(); i++)
        {
            u32 off = dataWriter.Position;
            dataWriter.Write(Textures[i], Buffer::BinaryString::NullTerminated);
            dataWriter.WriteAlign(4);
            u32 endPos = dataWriter.Position;
            dataWriter.Position = 4 + i * 4;
            dataWriter.Write(off - 4);
            dataWriter.Position = endPos;
        }
        data = dataWriter.getBuffer();
        BasePane::WritePane(writer);
    }

    MaterialsSection::MaterialsSection() : BasePane("mat1", 8) {}
    MaterialsSection::MaterialsSection(Buffer &reader) : BasePane("mat1", reader)
    {
        Buffer dataReader(data);
        dataReader.ByteOrder = reader.ByteOrder;
        int matCount = dataReader.readInt32();
        auto Offsets = dataReader.ReadS32Array(matCount);
        for (int i = 0; i < matCount; i++)
        {
            int matLen = (i == matCount - 1 ? (int)dataReader.Length() : Offsets[i + 1] - 8) - (int)dataReader.Position;
            Materials.push_back(dataReader.readBytes(matLen));
        }
    }

    void MaterialsSection::WritePane(Buffer &writer)
    {
        Buffer dataWriter;
        dataWriter.ByteOrder = writer.ByteOrder;
        dataWriter.Write((s32)Materials.size());
        for (int i = 0; i < Materials.size(); i++)
            dataWriter.Write((s32)0);
        for (int i = 0; i < Materials.size(); i++)
        {
            u32 off = dataWriter.Position;
            dataWriter.Write(Materials[i]);
            u32 endPos = dataWriter.Position;
            dataWriter.Position = 4 + i * 4;
            dataWriter.Write(off + 8);
            dataWriter.Position = endPos;
        }
        data = dataWriter.getBuffer();
        BasePane::WritePane(writer);
    }

    PicturePane::PicturePane(Buffer &buf) : BasePane("pic1",buf)
    {
        _PaneName = BflytFile::TryGetPanelName(this);
    }

    string PicturePane::PaneName() { return _PaneName; }

    string PicturePane::ToString() { return "Picture " + PaneName(); }

    string BflytFile::TryGetPanelName(const BasePane *ptr)
    {
        if (ptr == nullptr || ptr->data.size() < 0x18 + 4) return "";
        Buffer dataReader(ptr->data);
        dataReader.ByteOrder = Endianness::LittleEndian;
        dataReader.readInt32(); //Unknown
        return dataReader.readStr_NullTerm(0x18);
    }

    BasePane*& BflytFile::operator[] (int index)
    {
        return Panes[index];
    }

    BflytFile::BflytFile(const vector<u8>& file) 
    {
        Buffer bin(file);
        bin.ByteOrder = Endianness::LittleEndian;
        if (bin.readStr(4) != "FLYT") throw "Wrong signature";
        bin.readUInt16(); //BOM
        bin.readUInt16(); //HeaderSize
        Version = bin.readUInt32();
        bin.readUInt32(); //File size
        u16 sectionCount = bin.readUInt16();
        bin.readUInt16(); //padding
        for (int i = 0; i < sectionCount; i++)
        {
            string name = bin.readStr(4);
            if (name == "txl1")
                Panes.push_back((BasePane*) new TextureSection(bin));
            else if (name == "mat1")
                Panes.push_back((BasePane*) new MaterialsSection(bin));
            else if (name == "pic1")
                Panes.push_back((BasePane*) new PicturePane(bin));
            else 
                Panes.push_back(new BasePane(name,bin));

            if (i == sectionCount - 1 && bin.Position != bin.Length()) //load sections missing in the section count (my old bflyt patch)
            {
                u8 c = 0;
                while (bin.Position < bin.Length() && (c = bin.readUInt8() == 0)) {}
                if (c != 0)
                    bin.Position--;
                if (bin.Length() - bin.Position >= 8) //min section size
                {
                    sectionCount++;
                }
            }
        }
    }

    BflytFile::~BflytFile() 
    {
        for (auto ptr : Panes)
            delete ptr;
        Panes.clear();
    }

    TextureSection* BflytFile::GetTexSection()
    {
        for (auto ptr : Panes)
        {
            if (ptr->name == "txl1")
                return (TextureSection*)ptr;
        }
        TextureSection *ptr = new TextureSection();
        Panes.insert(Panes.begin() + 2, (BasePane*)ptr);
        return ptr;
    }

    MaterialsSection* BflytFile::GetMatSection()
    {
        for (auto ptr : Panes)
        {
            if (ptr->name == "mat1")
                return (MaterialsSection*)ptr;
        }
        MaterialsSection *ptr = new MaterialsSection();
        Panes.insert(Panes.begin() + 3, (BasePane*)ptr);
        return ptr;
    }

    vector<u8> BflytFile::SaveFile() 
    {
        Buffer bin;
        bin.ByteOrder = Endianness::LittleEndian;
        bin.Write("FLYT");
        bin.Write((u8)0xFF);
        bin.Write((u8)0xFE); //Little endian
        bin.Write((u16)0x14); //Header size
        bin.Write(Version);
        bin.Write((s32)0);
        bin.Write((u16)Panes.size());
        bin.Write((u16)0); //padding
        for (auto p : Panes)
            p->WritePane(bin);
        bin.WriteAlign(4);
        bin.Position = 0xC;
        bin.Write((u32)bin.Length());
        bin.Position = bin.Length();
        return bin.getBuffer();
    }

    void BflytFile::PatchTextureName(const string &original, const string &_new) 
    {
        auto texSection = GetTexSection();
        if (texSection == nullptr)
            throw "this layout doesn't have any texture section (?)";
        for (int i = 0; i < texSection->Textures.size(); i++)
        {
            if (texSection->Textures[i] == original)
                texSection->Textures[i] = _new;
        }
    }

    vector<string> BflytFile::GetPaneNames() 
    {
        vector<string> names;
        for (int i = 0; i < Panes.size(); i++)
            names.push_back(TryGetPanelName(Panes[i]));
        return names;
    }

    inline int indexOf(const vector<string>& v, const string& s)
    {
        for (int i = 0; i < v.size(); i++)
            if (v[i] == s) return i;
        return -1;
    }

    BflytFile::PatchResult BflytFile::ApplyLayoutPatch(const vector<PanePatch>& Patches) 
    {
        auto names = GetPaneNames();
        for (int i = 0; i < Patches.size(); i++)
        {
            int index = indexOf(names, Patches[i].PaneName);
            if (index == -1)
                return PatchResult::CorruptedFile;
            auto p = Patches[i];
            auto e = new PropertyEditablePane(*Panes[index]);
            delete Panes[index];
            Panes[index] = (BasePane*)e;
            if (p.ApplyFlags & (u32)PanePatch::Flags::Visible)
                e->SetVisible(p.Visible);

            if (p.ApplyFlags & (u32)PanePatch::Flags::Position)
            {
                e->Position.X = p.Position.X;
                e->Position.Y = p.Position.Y;
                e->Position.Z = p.Position.Z;
            }			
            if (p.ApplyFlags & (u32)PanePatch::Flags::Rotation)
            {
                e->Rotation.X = p.Rotation.X;
                e->Rotation.Y = p.Rotation.Y;
                e->Rotation.Z = p.Rotation.Z;
            }
            if (p.ApplyFlags & (u32)PanePatch::Flags::Scale)
            {
                e->Scale.X = p.Scale.X;
                e->Scale.Y = p.Scale.Y;
            }
            if (p.ApplyFlags & (u32)PanePatch::Flags::Size)
            {
                e->Size.X = p.Size.X;
                e->Size.Y = p.Size.Y;
            }

            if (e->name == "pic1")
            {
                if (p.ApplyFlags & (u32)PanePatch::Flags::ColorTL)
                    e->ColorData[0] = (u32)std::stoul(p.ColorTL, 0, 16);
                if (p.ApplyFlags & (u32)PanePatch::Flags::ColorTR)
                    e->ColorData[1] = (u32)std::stoul(p.ColorTR, 0, 16);
                if (p.ApplyFlags & (u32)PanePatch::Flags::ColorBL)
                    e->ColorData[2] = (u32)std::stoul(p.ColorBL, 0, 16);
                if (p.ApplyFlags & (u32)PanePatch::Flags::ColorBR)
                    e->ColorData[3] = (u32)std::stoul(p.ColorBR, 0, 16);
            }

        }
        return PatchResult::OK;
    }

    BflytFile::PatchResult BflytFile::PatchBgLayout(const PatchTemplate& patch) 
    {
        //Detect patch
        for (int i = 0; i < Panes.size(); i++)
        {
            if (Panes[i]->name != "pic1") continue;
            auto p = (PicturePane*)Panes[i];
            if (p->PaneName() == patch.PatchIdentifier) return PatchResult::AlreadyPatched;
            if (p->PaneName() == "3x3lxBG") //Fix old layout
            {
                delete Panes[i];
                Panes.erase(Panes.begin() + i);
                GetTexSection()->Textures[0] = "White1x1^r";
                GetMatSection()->Materials.erase(GetMatSection()->Materials.begin() + 1);
            }
        }
        //Find and remove target panes
        s32 target = INT32_MAX;
        for (int i = 0; i < Panes.size() - 1; i++)
        {
            string name = TryGetPanelName(Panes[i]);
            if (name != "" && indexOf(patch.targetPanels, name) != -1)
            {
                if (i < target) target = i;
                if (!patch.NoRemovePanel)
                {
                    Buffer bin(Panes[i]->data);
                    bin.ByteOrder = Endianness::LittleEndian;
                    bin.Position = 0x24;
                    bin.Write((float)5000.0);
                    bin.Write((float)60000.0);
                    Panes[i]->data = bin.getBuffer();
                }
            }
        }
        if (target == INT32_MAX)
            return PatchResult::Fail;
        return AddBgPanel(target, patch.MaintextureName, patch.PatchIdentifier);
    }

    BflytFile::PatchResult BflytFile::AddBgPanel(int index, const string &TexName, const string &Pic1Name)
    {
        //Add pitcture
        if (Pic1Name.length() > 0x18)
            throw "Pic1Name should not be longer than 24 chars";
        auto BgPanel = new BasePane("pic1", 0x8);
        Panes.insert(Panes.begin() + index, BgPanel);
        auto MatSect = GetMatSection();
        {
            Buffer bin;
            bin.ByteOrder = Endianness::LittleEndian;
            bin.Write((u8)0x01);
            bin.Write((u8)0x00);
            bin.Write((u8)0xFF);
            bin.Write((u8)0x04);
            bin.Write(Pic1Name);
            int zerCount = Pic1Name.length();
            while (zerCount++ < 0x38)
                bin.Write((u8)0x00);
            bin.Write((float)1);
            bin.Write((float)1);
            bin.Write((float)1280);
            bin.Write((float)720);
            bin.Write((u32)0xFFFFFFFF);
            bin.Write((u32)0xFFFFFFFF);
            bin.Write((u32)0xFFFFFFFF);
            bin.Write((u32)0xFFFFFFFF);
            bin.Write((u16)MatSect->Materials.size());
            bin.Write((u16)1);
            bin.Write((u32)0);
            bin.Write((u32)0);
            bin.Write((float)1);
            bin.Write((u32)0);
            bin.Write((u32)0);
            bin.Write((float)1);
            bin.Write((float)1);
            bin.Write((float)1);
            BgPanel->data = bin.getBuffer();
        }
        //Add texture
        auto texSection = GetTexSection();
        int texIndex = indexOf(texSection->Textures, TexName);
        if (texIndex == -1)
        {
            texIndex = texSection->Textures.size();
            texSection->Textures.push_back(TexName);
        }
        //Add material
        {
            Buffer bin;
            bin.ByteOrder = Endianness::LittleEndian;
            bin.Write("P_Custm", Buffer::BinaryString::NullTerminated);
            for (int i = 0; i < 0x14; i++)
                bin.Write((u8)0);
            bin.Write((s32)0x15);
            bin.Write((s32)0x8040200);
            bin.Write((s32)0);
            bin.Write((u32)0xFFFFFFFF);
            bin.Write((u16)texIndex);
            bin.Write((u16)0x0);
            for (int i = 0; i < 0xC; i++)
                bin.Write((u8)0);
            bin.Write((float)1);
            bin.Write((float)1);
            for (int i = 0; i < 0x10; i++)
                bin.Write((u8)0);
            MatSect->Materials.push_back(bin.getBuffer());
        }
        return PatchResult::OK;
    }
}