#pragma once

#include <switch.h>
#include <functional>
#include "ui/framework/layout.hpp"

namespace tin::ui
{
    class TouchHandler
    {
        private:
            static const int MAX_TAP_MOVEMENT = 20;

            Position m_touchAreaPos;
            Dimensions m_touchAreaSize;

        public:
            std::function<void (unsigned int posX, unsigned int posY)> m_onTapped = nullptr;

            TouchHandler(Position touchAreaPos, Dimensions touchAreaSize);
            TouchHandler();

            void Update();

            void SetTouchArea(Position touchAreaPos, Dimensions touchAreaSize);
    };
}