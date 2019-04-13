
/*

    Plutonium library

    @file Application.hpp
    @brief An Application is the base to use the UI system of this library.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <vector>
#include <functional>
#include <chrono>
#include <pu/Dialog.hpp>
#include <pu/Layout.hpp>
#include <pu/overlay/Overlay.hpp>

namespace pu
{
    class Application
    {
        public:
            Application();
            ~Application();
            void LoadLayout(Layout *Layout);
            draw::Color GetBackgroundColor();
            void SetBackgroundColor(draw::Color BackColor);
            std::string GetBackgroundImage();
            bool HasBackgroundImage();
            void SetBackgroundImage(std::string Path);
            void DeleteBackgroundImage();
            void AddThread(std::function<void()> Callback);
            void SetOnInput(std::function<void(u64 Down, u64 Up, u64 Held, bool Touch)> Callback);
            u32 ShowDialog(Dialog *ToShow);
            int CreateShowDialog(std::string Title, std::string Content, std::vector<std::string> Options, bool UseLastOptionAsCancel, std::string Icon = "");
            void StartOverlay(overlay::Overlay *Ovl);
            void StartOverlayWithTimeout(overlay::Overlay *Ovl, u64 Milli);
            void EndOverlay();
            void Show();
            bool CallForRender();
            bool CallForRenderWithRenderOver(std::function<bool(render::Renderer *Drawer)> RenderFunc);
            void OnRender();
            void Close();
        protected:
            bool rover;
            std::function<bool(render::Renderer *Drawer)> rof;
            draw::Color bgcolor;
            std::string bgimage;
            render::NativeTexture ntex;
            bool hasimage;
            bool show;
            s32 fact;
            Layout *lyt;
            u64 tmillis;
            std::chrono::steady_clock::time_point tclock;
            bool fovl;
            bool ffovl;
            overlay::Overlay* ovl;
            std::vector<std::function<void()>> thds;
            std::function<void(u64, u64, u64, bool)> cbipt;
            render::Renderer *rend;
    };
}