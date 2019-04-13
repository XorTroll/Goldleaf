
/*

    Plutonium library

    @file Container.hpp
    @brief A Container is a basic object which contains a bunch of Elements.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/element.hpp>
#include <vector>

namespace pu
{
    class Container
    {
        public:
            Container(u32 X, u32 Y, u32 Width, u32 Height);
            void Add(element::Element *Elm);
            element::Element *At(u32 Index);
            bool Has(element::Element *Elm);
            void Clear();
            u32 GetCount();
            void SetX(u32 X);
            u32 GetX();
            void SetY(u32 Y);
            u32 GetY();
            void SetWidth(u32 Width);
            u32 GetWidth();
            void SetHeight(u32 Height);
            u32 GetHeight();
            void PreRender();
        protected:
            u32 x;
            u32 y;
            u32 w;
            u32 h;
            std::vector<element::Element*> elms;
    };
}