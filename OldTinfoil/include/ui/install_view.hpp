#pragma once

#include "ui/framework/layout_view.hpp"

namespace tin::ui
{
    class InstallView : public LayoutView
    {
        public:
            InstallView();

            virtual void Update() override;
    };
}