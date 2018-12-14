
#pragma once
#include <vector>
#include <functional>
#include <pn/Layout.hpp>

namespace pn
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
            void SetCustomFont(std::string FontPath);
            bool UsesCustomFont();
            std::string GetCustomFont();
            void AddThread(std::function<void()> Callback);
            void ShowDialog(fw::Dialog *Dialog);
            void Show();
            void CallForRender();
            void Close();
        protected:
            draw::Color bgcolor;
            std::string bgimage;
            bool hasimage;
            bool show;
            s32 fact;
            Layout *lyt;
            std::vector<std::function<void()>> thds;
            render::Renderer *rend;
    };
}