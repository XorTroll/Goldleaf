
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

    enum class HorizontalAlign
    {
        Left,
        Center,
        Right,
    };

    enum class VerticalAlign
    {
        Up,
        Center,
        Down,
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
            void ProcessInput(void *Lyt, u64 Down, u64 Up, u64 Held, bool Touch);
            bool IsVisible();
            void SetVisible(bool Visible);
            bool IsAffectedByFocus();
            void SetAffectedByFocus(bool Affected);
            Element *GetFocusChangeElement(FocusChangeDirection Direction);
            void SetFocusChangeElement(FocusChangeDirection Direction, Element *ToChange);
            void SetParent(void *Base);
            void *GetParent();
            void SetHorizontalAlign(HorizontalAlign Align);
            HorizontalAlign GetHorizontalAlign();
            void SetVerticalAlign(VerticalAlign Align);
            VerticalAlign GetVerticalAlign();
            bool HasParent();
            u32 GetProcessedX();
            u32 GetProcessedY();
        protected:
            bool visible;
            bool afocus;
            Element *fup;
            Element *fdown;
            Element *fleft;
            Element *fright;
            HorizontalAlign halign;
            VerticalAlign valign;
            void *parent;
    };
}