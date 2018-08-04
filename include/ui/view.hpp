#pragma once

#include <switch.h>

#include <memory>
#include <stack>

namespace tin::ui
{
    class ViewManager;

    class View
    {
        friend ViewManager;

        protected:
            ViewManager *m_viewManager;
            View *m_prevView;

            virtual void OnPresented() = 0;

            virtual void ProcessInput(u64 keys) = 0;
            virtual void Update() = 0;
    };

    class ViewManager final
    {
        private:
            std::stack<std::unique_ptr<View>> m_views;

        public:
            PrintConsole* m_printConsole;

            ViewManager();
            ViewManager(ViewManager const&) = delete;
            void operator=(ViewManager const&)  = delete;

            static ViewManager& Instance();

            void PushView(std::unique_ptr<View> view);
            void Unwind();

            void ProcessInput(u64 keys);
            void Update();

            virtual View* GetCurrentView();
    };
}