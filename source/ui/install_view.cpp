#include "ui/install_view.hpp"

#include <memory>
#include "ui/framework/box_element.hpp"

namespace tin::ui
{
    InstallView::InstallView()
    {
        auto backgroundLayer = std::make_unique<Layer>("background");
        auto foregroundLayer = std::make_unique<Layer>("foreground");

        // Background
        auto backgroundBox = std::make_unique<BoxElement>(1280, 720);
        backgroundBox->SetColour(tin::util::Colour(0x394566));
        backgroundLayer->AddElement(std::move(backgroundBox), 0, 0);

        // Foreground
        auto sidebarBox = std::make_unique<BoxElement>(120, 720);
        sidebarBox->SetColour(tin::util::Colour(0x2B334F));
        foregroundLayer->AddElement(std::move(sidebarBox), 0, 0);

        auto headerBox = std::make_unique<BoxElement>(1280-120, 80);
        headerBox->SetColour(tin::util::Colour(0x556C91));
        foregroundLayer->AddElement(std::move(headerBox), 120, 0);

        this->AddLayer(std::move(backgroundLayer));
        this->AddLayer(std::move(foregroundLayer));
    }
}