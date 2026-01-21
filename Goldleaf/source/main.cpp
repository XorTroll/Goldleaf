
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright © 2018-2025 XorTroll

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

ui::MainApplication::Ref g_MainApplication = nullptr;
cfg::Settings g_Settings = {};
bool g_UpdatedNeedsRename = false;

int main() {
    // Initialize services
    GLEAF_RC_ASSERT(Initialize());

    GLEAF_LOG_FMT("Hello World from Goldleaf v%s!", GLEAF_VERSION);

    // Create the UI renderer and the application - it will initialize romfs
    auto renderer_opts = pu::ui::render::RendererInitOptions(SDL_INIT_EVERYTHING, pu::ui::render::RendererHardwareFlags);
    renderer_opts.SetPlServiceType(PlServiceType_User);
    renderer_opts.AddExtraDefaultFontSize(35);
    renderer_opts.AddDefaultAllSharedFonts();
    renderer_opts.UseImage(pu::ui::render::ImgAllFlags);
    renderer_opts.AddInputNpadIdType(HidNpadIdType_Handheld); // Make it controllable only by player 1
    renderer_opts.AddInputNpadIdType(HidNpadIdType_No1);
    renderer_opts.AddInputNpadStyleTag(HidNpadStyleSet_NpadStandard);
    auto renderer = pu::ui::render::Renderer::New(renderer_opts);
    g_MainApplication = ui::MainApplication::New(renderer);

    // Initialize and load settings
    g_Settings = {};
    g_Settings.Load();

    // Load language strings, now that romfs is initialized
    cfg::LoadStrings();

    // Try to get the selected user if it was selected, and cache its icon if we succeed
    if(acc::SelectFromPreselectedUser()) {
        acc::ExportSelectedUserIcon();
    }

    // Load the application
    GLEAF_RC_ASSERT(g_MainApplication->Load());

    // Start and loop the application
    g_MainApplication->ShowWithFadeIn();

    GLEAF_LOG_FMT("Exiting Goldleaf...");

    Finalize();
    return 0;
}
