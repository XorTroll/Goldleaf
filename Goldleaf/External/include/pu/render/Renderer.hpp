
/*

    Plutonium library

    @file Renderer.hpp
    @brief A Renderer is the object performing basic rendering. (simply, a SDL2 wrapper)
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/draw/Color.hpp>
#include <pu/draw/Font.hpp>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

namespace pu::render
{
    class Renderer
    {
        public:
            void Initialize();
            void Finalize();
            bool HasInitialized();
            void SetCustomFont(std::string FontPath);
            bool UsesCustomFont();
            std::string GetCustomFont();
            bool HasRomFs();
            void Clear(draw::Color ClearColor);
            void Render();
            void DrawImage(std::string Path, u32 X, u32 Y);
            void DrawImageScaled(std::string Path, u32 X, u32 Y, u32 Width, u32 Height);
            void DrawText(std::string Text, draw::Font Font, u32 Size, u32 X, u32 Y, draw::Color TextColor);
            void DrawRectangle(draw::Color RecColor, u32 X, u32 Y, u32 Width, u32 Height);
            void DrawRectangleFill(draw::Color RecColor, u32 X, u32 Y, u32 Width, u32 Height);
            void DrawCircle(draw::Color CircleColor, u32 X, u32 Y, u32 Radius);
            void DrawHorizontalShadow(u32 X, u32 Y, u32 Width, u32 Height, u32 BaseAlpha);
            u32 GetTextWidth(draw::Font Font, std::string Text, u32 Size);
            u32 GetTextHeight(draw::Font Font, std::string Text, u32 Size);
        private:
            bool okromfs;
            bool initialized;
            bool cfont;
            std::string pfont;
            SDL_Window *rendwd;
            SDL_Renderer *rendrd;
            SDL_Surface *rendsf;
    };
}