
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
            Layout();
            void AddChild(element::Element *Child);
            void ClearChildren();
            element::Element *GetChildAt(u32 Index);
            u32 GetChildCount();
            bool HasChilds();
            void SetOnInput(std::function<void(u64 Input)> Callback);
            std::function<void(u64 Input)> GetOnInput();
        private:
            std::function<void(u64 Input)> onipt;
            std::vector<element::Element*> chld;
    };
}