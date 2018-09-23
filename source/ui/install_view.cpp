#include "ui/install_view.hpp"

#include <memory>
#include "ui/framework/canvas.hpp"
#include "ui/framework/box_element.hpp"
#include "ui/framework/list_element.hpp"
#include "ui/framework/text_element.hpp"
#include "ui/framework/font_renderer.hpp"

namespace tin::ui
{
    InstallView::InstallView()
    {
        auto backgroundLayer = std::make_unique<Layer>("background");
        auto foregroundLayer = std::make_unique<Layer>("foreground");

        // Background
        auto backgroundBox = std::make_unique<BoxElement>(1280, 720);
        backgroundBox->SetColour(tin::ui::Colour(0x394566));
        backgroundLayer->AddElement(std::move(backgroundBox), 0, 0);

        // Foreground
        auto sidebarBox = std::make_unique<BoxElement>(120, 720);
        sidebarBox->SetColour(tin::ui::Colour(0x2B334F));
        foregroundLayer->AddElement(std::move(sidebarBox), 0, 0);

        // Header
        auto headerBox = std::make_unique<BoxElement>(1280-120, 80);
        SubElementLayout headerSubElementLayout;
        headerSubElementLayout.arrangementType = SubElementArrangementType::LEFT_TO_RIGHT;
        headerSubElementLayout.leftInset = 25;
        headerSubElementLayout.bottomInset = 27;
        headerBox->SetColour(tin::ui::Colour(0x556C91));
        headerBox->SetSubElementLayout(headerSubElementLayout);

        auto installApplicationText = std::make_unique<TextElement>(1280-120, 80);
        installApplicationText->SetText("Install Application");
        installApplicationText->SetScale(6);

        headerBox->AddSubElement(std::move(installApplicationText));
        foregroundLayer->AddElement(std::move(headerBox), 120, 0);

        // Game list
        auto gameList = std::make_unique<ListElement>(625, 720-80);
        gameList->SetColour(0x394566);
        gameList->AddRow("Game 1");
        gameList->AddRow("Game 2");
        gameList->AddRow("Game 3");
        gameList->AddRow("Game 4");
        gameList->AddRow("Game 5");
        gameList->AddRow("Game 6");
        gameList->AddRow("Game 7");
        gameList->AddRow("Game 8");
        gameList->AddRow("Game 9");
        gameList->AddRow("Game 10");
        foregroundLayer->AddElement(std::move(gameList), 120+25, 80);

        this->AddLayer(std::move(backgroundLayer));
        this->AddLayer(std::move(foregroundLayer));
    }

    void InstallView::Update()
    {
        LayoutView::Update();
    }
}