
/*

    Plutonium library

    @file Menu.hpp
    @brief A Menu is a very useful Element for option browsing or selecting.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element/Element.hpp>
#include <vector>
#include <functional>

namespace pu::element
{
    class MenuItem
    {
        public:
            MenuItem(std::string Name);
            std::string GetName();
            void SetName(std::string Name);
            void SetOnClick(std::function<void()> Callback);
            std::function<void()> GetCallback();
            std::string GetIcon();
            void SetIcon(std::string Icon);
            bool HasIcon();
        private:
            std::string name;
            bool hasicon;
            std::string icon;
            std::function<void()> cb;
    };

    class Menu : public Element
    {
        public:
            Menu(u32 X, u32 Y, u32 Width, draw::Color OptionColor, u32 ItemSize, u32 ItemsToShow);
            u32 GetX();
            void SetX(u32 X);
            u32 GetY();
            void SetY(u32 Y);
            u32 GetWidth();
            void SetWidth(u32 Width);
            u32 GetHeight();
            u32 GetItemSize();
            void SetItemSize(u32 ItemSize);
            u32 GetNumberOfItemsToShow();
            void SetNumberOfItemsToShow(u32 ItemsToShow);
            draw::Font GetFont();
            void SetFont(draw::Font Font);
            u32 GetFontSize();
            void SetFontSize(u32 Size);
            draw::Color GetColor();
            void SetColor(draw::Color OptionColor);
            draw::Color GetScrollbarColor();
            void SetScrollbarColor(draw::Color ScrollbarColor);
            void SetOnSelectionChanged(std::function<void()> Callback);
            void AddItem(MenuItem *Item);
            void ClearItems();
            void SetCooldownEnabled(bool Cooldown);
            MenuItem *GetSelectedItem();
            u32 GetSelectedIndex();
            void SetSelectedIndex(u32 Index);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Input);
        private:
            u32 x;
            u32 y;
            u32 w;
            u32 isize;
            u32 ishow;
            u32 previsel;
            u32 fisel;
            u32 isel;
            s32 pselfact;
            s32 selfact;
            draw::Color scb;
            draw::Color clr;
            draw::Font fnt;
            u32 fsize;
            bool icdown;
            std::function<void()> onselch;
            std::vector<MenuItem*> itms;
    };
}