
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2022 XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <ui/ui_MainApplication.hpp>

ui::MainApplication::Ref g_MainApplication;
cfg::Settings g_Settings;
bool g_UpdatedNeedsRename = false;

int main() {
    // Initialize services
    GLEAF_RC_ASSERT(Initialize());

    // Create the UI renderer and the application - it will initialize romfs
    auto renderer_opts = pu::ui::render::RendererInitOptions(SDL_INIT_EVERYTHING, pu::ui::render::RendererHardwareFlags);
    renderer_opts.UseImage(pu::ui::render::IMGAllFlags);
    renderer_opts.UseTTF();
    renderer_opts.SetExtraDefaultFontSize(35);
    renderer_opts.UseRomfs();
    auto renderer = pu::ui::render::Renderer::New(renderer_opts);
    g_MainApplication = ui::MainApplication::New(renderer);
    
    // Initialize and load config
    g_Settings = cfg::ProcessSettings();

    // Load language strings, now that romfs is initialized
    cfg::LoadStrings();

    // Try to get a selected user if it was selected, and cache its icon if we succeed
    if(acc::SelectFromPreselectedUser()) {
        acc::CacheSelectedUserIcon();
    }

    // Start and loop the application
    g_MainApplication->Prepare();
    g_MainApplication->ShowWithFadeIn();

    Exit(0);
    return 0;
}