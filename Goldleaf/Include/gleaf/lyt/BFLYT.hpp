
/*

    Goldleaf - Nintendo Switch title manager homebrew

    Copyright © 2018 - Goldleaf project, developed by XorTroll
    This project is under the terms of GPLv3 license: https://github.com/XorTroll/Goldleaf/blob/master/LICENSE

    Code ported from exelix's source

*/

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <gleaf/Buffer.hpp>
#include <gleaf/lyt/Patches.hpp>

namespace gleaf::lyt 
{
	class BasePane
	{
        public:
            BasePane(const std::string &_name, u32 len);
            BasePane(const BasePane &ref);
            BasePane(const std::string &_name, Buffer &reader);
            virtual std::string ToString();
            const std::string name;
            s32 length;
            std::vector<u8> data;
            virtual void WritePane(Buffer &writer);
	};

	class PropertyEditablePane : public BasePane 
	{
        public:
            PropertyEditablePane(const BasePane &p);
            std::string ToString() override;
            std::string PaneName;
            Vector3 Position, Rotation;
            Vector2 Scale, Size;
            bool GetVisible();
            void SetVisible(bool);
            std::vector<u32> ColorData;
            void ApplyChanges();
            void WritePane(Buffer &writer) override;
        private:
            u8 _flag1;
	};

	class TextureSection : public BasePane
	{
        public:
            TextureSection(Buffer &reader);
            TextureSection();
            std::vector<std::string> Textures;
            void WritePane(Buffer &writer) override;
	};

	class MaterialsSection : BasePane
	{
	public:
		std::vector<std::vector<u8>> Materials;
		MaterialsSection(Buffer &reader);
		MaterialsSection();

		void WritePane(Buffer &writer) override;
	};

	class PicturePane : public BasePane
	{
        public:
            std::string ToString() override;
            std::string PaneName();

            PicturePane(Buffer &reader);
        private:
            std::string _PaneName;
	};

    class BflytFile 
    {
        public:
            enum class PatchResult : u8 
            {
                AlreadyPatched,
                Fail,
                CorruptedFile,
                OK
            };

            BflytFile(const std::vector<u8>& file);
            ~BflytFile();
            
            static std::string TryGetPanelName(const BasePane *p);

            u32 Version;

            TextureSection* GetTexSection();
            MaterialsSection* GetMatSection();

            std::vector<u8> SaveFile();
            void PatchTextureName(const std::string &original, const std::string &_new);
            std::vector<std::string> GetPaneNames();
            PatchResult ApplyLayoutPatch(const std::vector<PanePatch>& Patches);
            PatchResult PatchBgLayout(const PatchTemplate& patch);
        private:
            BasePane*& operator[] (int index);
            std::vector<BasePane*> Panes;
            PatchResult AddBgPanel(int index, const std::string &TexName, const std::string &Pic1Name);
    };
}