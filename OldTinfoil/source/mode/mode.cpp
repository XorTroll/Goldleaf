#include "mode/mode.hpp"
#include "ui/framework/console_options_view.hpp"
#include "nx/fs.hpp"

namespace tin::ui
{
    IMode::IMode(std::string name) :
        m_name(name)
    {

    }

    Category::Category(std::string name) :
        m_name(name)
    {

    }

    void Category::AddMode(std::unique_ptr<IMode> mode)
    {
        m_modes.push_back(std::move(mode));
    }

    void Category::OnSelected()
    {
        m_currentMode = nullptr;

        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleOptionsView>();

        view->AddEntry(m_name, tin::ui::ConsoleEntrySelectType::HEADING, nullptr);
        view->AddEntry("", tin::ui::ConsoleEntrySelectType::NONE, nullptr);

        for (auto& mode : m_modes)
        {
            auto modeSelected = [&] ()
            {
                m_currentMode = mode.get();
                mode->OnSelected();
            };

            view->AddEntry(mode->m_name, tin::ui::ConsoleEntrySelectType::SELECT, modeSelected);
        }

        manager.PushView(std::move(view));
    }
}