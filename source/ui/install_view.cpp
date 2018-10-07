#include "ui/install_view.hpp"

#include <memory>
#include "asset/image.hpp"
#include "ui/framework/canvas.hpp"
#include "ui/framework/box_element.hpp"
#include "ui/framework/list_element.hpp"
#include "ui/framework/text_element.hpp"
#include "ui/framework/font_renderer.hpp"
#include "data/byte_buffer.hpp"
#include "data/byte_stream.hpp"

#include "error.hpp"

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
        headerBox->SetColour(tin::ui::Colour(0x556C91));
        headerBox->SetSubElementLayout(headerSubElementLayout);

        auto installApplicationText = std::make_unique<TextElement>(1280-120, 80);
        installApplicationText->SetText("Install Application");
        installApplicationText->SetScale(6);
        installApplicationText->SetInsets(25, 30);

        headerBox->AddSubElement(std::move(installApplicationText));
        foregroundLayer->AddElement(std::move(headerBox), 120, 0);

        // Game list
        // 720 - 80 - 3 to account for the header and to chop off the underline on the bottom row
        auto gameList = std::make_unique<ListElement>(625, 720 - 80 - 3);
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
        gameList->AddRow("Game 11");
        gameList->AddRow("Game 12");
        gameList->AddRow("Game 13");
        gameList->AddRow("Game 14");
        gameList->AddRow("Game 15");
        gameList->AddRow("Game 16");
        gameList->AddRow("Game 17");
        gameList->AddRow("Game 18");
        gameList->AddRow("Game 19");
        gameList->AddRow("Game 20");
        foregroundLayer->AddElement(std::move(gameList), 120+25, 80);

        this->AddLayer(std::move(backgroundLayer));
        this->AddLayer(std::move(foregroundLayer));
    }

    void InstallView::Update()
    {
        LayoutView::Update();
    }
}