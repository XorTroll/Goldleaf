    
#pragma once
#include <string>
#include <switch.h>
#include <pu/draw.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

namespace pu::render
{
    typedef SDL_Window *NativeWindow;
    typedef SDL_Renderer *NativeRenderer;
    typedef SDL_Surface *NativeSurface;
    typedef SDL_Texture *NativeTexture;
    typedef TTF_Font *NativeFont;

    enum class SharedFont
    {
        Standard,
        ChineseSimplified,
        ExtendedChineseSimplified,
        ChineseTraditional,
        Korean,
        NintendoExtended,
    };

    NativeTexture ConvertToTexture(NativeSurface Surface);
    NativeTexture RenderText(NativeFont Font, std::string Text, draw::Color Color);
    NativeTexture LoadImage(std::string Path);
    NativeFont LoadSharedFont(SharedFont Type, u32 Size);
    NativeFont LoadFont(std::string Path, u32 Size);
    u32 GetTextureWidth(NativeTexture Texture);
    u32 GetTextureHeight(NativeTexture Texture);
    u32 GetTextWidth(NativeFont Font, std::string Text);
    u32 GetTextHeight(NativeFont Font, std::string Text);
    void SetAlphaValue(NativeTexture Texture, u8 Alpha);
    void DeleteFont(NativeFont Font);
    void DeleteTexture(NativeTexture Texture);
}