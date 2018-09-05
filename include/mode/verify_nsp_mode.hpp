#pragma once

#include <string>
#include <switch.h>
#include "mode/mode.hpp"

namespace tin::ui
{
    class VerifyNSPMode : public tin::ui::IMode
    {
        public:
            VerifyNSPMode();

            void OnSelected() override;
    };
}