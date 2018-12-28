
/*

    Plutonium library

    @file Renderer.hpp
    @brief A Renderer is the object performing basic rendering. (simply, a SDL2 wrapper)
    @author XorTroll

    @copyright Plutonium project - an easy-to-use UI framework for Nintendo Switch homebrew

*/

#pragma once
#include <pu/draw.hpp>
#include <pu/render/SDL2.hpp>
#include <string>
#include <fstream>

namespace pu::render
{
    class Renderer
    {
        public:
            void Initialize();
            void Finalize();
            bool HasInitialized();
            bool HasRomFs();
            void InitializeRender(draw::Color Color);
            void FinalizeRender();
            void RenderTexture(NativeTexture Texture, u32 X, u32 Y);
            void RenderTextureScaled(NativeTexture Texture, u32 X, u32 Y, u32 Width, u32 Height);
            void RenderRectangle(draw::Color Color, u32 X, u32 Y, u32 Width, u32 Height);
            void RenderRectangleFill(draw::Color Color, u32 X, u32 Y, u32 Width, u32 Height);
            void RenderCircle(draw::Color Color, u32 X, u32 Y, u32 Radius);
            void RenderShadowSimple(u32 X, u32 Y, u32 Width, u32 Height, u32 BaseAlpha);
        private:
            bool okromfs;
            bool initialized;
            NativeWindow rendwd;
            NativeSurface rendsf;
    };

    NativeRenderer GetMainRenderer();
}