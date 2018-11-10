#pragma once

#include <switch.h>
#include "mode/mode.hpp"

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