#include "ui/console_view.hpp"

namespace tin::ui
{
    ConsoleView::ConsoleView() {}

    void ConsoleView::OnPresented()
    {
        consoleClear();
    }

    void ConsoleView::ProcessInput(u64 keys)
    {
        if (keys & KEY_B)
            m_viewManager->Unwind();
    }
    
    void ConsoleView::Update() {}
};