#pragma once

#include <switch.h>
#include "mode/mode.hpp"

namespace tin::ui
{
    class DeletePersonalizedTicketMode : public IMode
    {
        public:
            DeletePersonalizedTicketMode();

            void OnSelected() override;
            void OnRightsIdSelected();
    };
}