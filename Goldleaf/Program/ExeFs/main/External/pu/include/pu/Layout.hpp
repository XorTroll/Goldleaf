
/*

    Plutonium library

    @file Layout.hpp
    @brief Contains Layout class, the object used to render specific objects
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element.hpp>

namespace pu
{
    class Layout
    {
        public:
            Layout(bool UseFocus = false);
            ~Layout();
            void AddChild(element::Element *Child);
            void ClearChildren();
            element::Element *GetChildAt(u32 Index);
            u32 GetChildCount();
            bool HasChilds();
            void SetElementOnFocus(element::Element *OnFocus);
            element::Element *GetElementOnFocus();
            void SetOnInput(std::function<void(u64 Down, u64 Up, u64 Held, bool Touch)> Callback);
            std::function<void(u64 Down, u64 Up, u64 Held, bool Touch)> GetOnInput();
            void AddThread(std::function<void()> Callback);
            std::vector<std::function<void()>> GetAllThreads();
            bool UsesFocus();
            void SetUseFocus(bool Focus);
        private:
            bool ufocus;
            std::function<void(u64, u64, u64, bool)> onipt;
            std::vector<std::function<void()>> thds;
            std::vector<element::Element*> chld;
            element::Element *efocus;
    };
}