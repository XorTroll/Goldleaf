
/*

    Plutonium library

    @file Element.hpp
    @brief An Element is the base of the UI's content.
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/render/Renderer.hpp>

namespace pu::element
{
    enum class FocusChangeDirection
    {
        Up,
        Down,
        Left,
        Right,
    };

    enum class DirectionController
    {
        JoyStick,
        DPad,
    };

    class Element
    {
        public:
            Element();
            virtual ~Element();
            virtual u32 GetX() = 0;
            virtual u32 GetY() = 0;
            virtual u32 GetWidth() = 0;
            virtual u32 GetHeight() = 0;
            virtual void OnRender(render::Renderer *Drawer) = 0;
            virtual void OnInput(u64 Down, u64 Up, u64 Held, bool Touch, bool Focus) = 0;
            virtual void OnDirectionPress(DirectionController Controller) = 0;
            void ProcessInput(void *Lyt, u64 Down, u64 Up, u64 Held, bool Touch);
            bool IsVisible();
            void SetVisible(bool Visible);
            Element *GetFocusChangeElement(FocusChangeDirection Direction);
            void SetFocusChangeElement(FocusChangeDirection Direction, Element *ToChange);
        protected:
            bool visible;
            Element *fup;
            Element *fdown;
            Element *fleft;
            Element *fright;
    };
}