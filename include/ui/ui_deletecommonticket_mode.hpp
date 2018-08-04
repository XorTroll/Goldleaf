#pragma once

#include <switch.h>
#include "ui/ui_mode.hpp"

namespace tin::ui
{
    class DeleteCommonTicketMode : public IMode
    {
        public:
            DeleteCommonTicketMode();

            void OnSelected() override;
            void OnRightsIdSelected();
    };
}