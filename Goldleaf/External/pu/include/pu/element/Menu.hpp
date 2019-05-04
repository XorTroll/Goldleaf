
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
#include <chrono>

namespace pu::element
{
    class MenuItem
    {
        public:
            MenuItem(std::string Name);
            ~MenuItem();
            std::string GetName();
            void SetName(std::string Name);
            draw::Color GetColor();
            void SetColor(draw::Color Color);
            void AddOnClick(std::function<void()> Callback, u64 Key = KEY_A);
            u32 GetCallbackCount();
            std::function<void()> GetCallback(u32 Index);
            u64 GetCallbackKey(u32 Index);
            std::string GetIcon();
            void SetIcon(std::string Icon);
            bool HasIcon();
            render::NativeFont GetFont();
            render::NativeTexture GetNameTexture();
            render::NativeTexture GetIconTexture();
        private:
            std::string name;
            draw::Color clr;
            bool hasicon;
            std::string icon;
            std::vector<std::function<void()>> cbs;
            std::vector<u64> cbipts;
            render::NativeFont font;
            render::NativeTexture itex;
            render::NativeTexture ntex;
    };

    class Menu : public Element
    {
        public:
            Menu(u32 X, u32 Y, u32 Width, draw::Color OptionColor, u32 ItemSize, u32 ItemsToShow);
            ~Menu();
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
            draw::Color GetColor();
            void SetColor(draw::Color Color);
            draw::Color GetOnFocusColor();
            void SetOnFocusColor(draw::Color Color);
            draw::Color GetScrollbarColor();
            void SetScrollbarColor(draw::Color Color);
            void SetOnSelectionChanged(std::function<void()> Callback);
            void AddItem(MenuItem *Item);
            void ClearItems();
            void SetCooldownEnabled(bool Cooldown);
            MenuItem *GetSelectedItem();
            u32 GetSelectedIndex();
            void SetSelectedIndex(u32 Index);
            void OnRender(render::Renderer *Drawer);
            void OnInput(u64 Down, u64 Up, u64 Held, bool Touch, bool Focus);
        private:
            bool dtouch;
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
            draw::Color fcs;
            bool icdown;
            std::function<void()> onselch;
            std::vector<MenuItem*> itms;
            bool uptime;
            bool uphold;
            std::chrono::time_point<std::chrono::steady_clock> uptp;
            bool downtime;
            bool downhold;
            std::chrono::time_point<std::chrono::steady_clock> downtp;
    };
}