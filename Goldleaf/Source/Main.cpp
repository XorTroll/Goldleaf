
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2020  XorTroll

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

ui::MainApplication::Ref global_app;
cfg::Settings global_settings;
bool global_app_updated = false;

int main()
{
    // Initialize services
    auto rc = Initialize();
    if(R_FAILED(rc)) diagAbortWithResult(rc);

    // Create the UI renderer and the application - it will initialize romfs
    auto renderer = pu::ui::render::Renderer::New(pu::ui::render::RendererInitOptions(SDL_INIT_EVERYTHING, pu::ui::render::RendererHardwareFlags).WithIMG(pu::ui::render::IMGAllFlags).WithMixer(pu::ui::render::MixerAllFlags).WithTTF().WithDefaultFontSize(35).WithRomfs());
    global_app = ui::MainApplication::New(renderer);
    
    // Initialize and load config
    global_settings = cfg::ProcessSettings();

    // Load language strings, now that romfs is initialized
    cfg::LoadStrings();

    // Try to get a selected user if it was selected, and cache its icon if we succeed
    if(acc::SelectFromPreselectedUser()) acc::CacheSelectedUserIcon();

    // Start and loop the application
    global_app->Prepare();
    global_app->ShowWithFadeIn();

    Exit(0);
    return 0;
}