#include <ui/ui_FileContentLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    FileContentLayout::FileContentLayout()
    {
        this->cntText = pu::ui::elm::TextBlock::New(40, 180, "");
        this->cntText->SetColor(gsets.CustomScheme.Text);
        this->cntText->SetFont(pu::ui::render::LoadFont(gsets.PathForResource("/FileSystem/FileDataFont.ttf"), 25));
        this->Add(this->cntText);
        this->loffset = 0;
    }

    void FileContentLayout::LoadFile(pu::String PPath, pu::String Path, fs::Explorer *Exp, bool Hex)
    {
        this->pth = Path;
        this->mode = Hex;
        this->gexp = Exp;
        this->loffset = 0;
        this->Update();
    }

    void FileContentLayout::Update()
    {
        std::vector<pu::String> lines;
        if(this->mode) lines = this->gexp->ReadFileFormatHex(this->pth, this->loffset, 19);
        else lines = this->gexp->ReadFileLines(this->pth, this->loffset, 19);
        if(lines.empty())
        {
            this->loffset--;
            return;
        }
        this->rlines = lines.size();
        pu::String alines;
        if(this->rlines > 0) for(u32 i = 0; i < this->rlines; i++)
        {
            if(i > 0) alines += "\n";
            alines += lines[i];
        }
        this->cntText->SetText(alines);
        lines.clear();
    }

    void FileContentLayout::ScrollUp()
    {
        if(this->loffset > 0)
        {
            this->loffset--;
            this->Update();
        }
    }

    void FileContentLayout::ScrollDown()
    {
        this->loffset++;
        this->Update();
    }
}