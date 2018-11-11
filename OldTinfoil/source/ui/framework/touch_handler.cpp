#include "ui/framework/touch_handler.hpp"

#include "error.hpp"

namespace tin::ui
{
    TouchHandler::TouchHandler(Position touchAreaPos, Dimensions touchAreaSize) :
        m_touchAreaPos(touchAreaPos), m_touchAreaSize(touchAreaSize)
    {

    }

    TouchHandler::TouchHandler() :
        TouchHandler(Position(0, 0), Dimensions(1280, 720))
    {

    }

    bool touchesStarted = false;
    touchPosition startTouchPos;
    bool tapping = false;

    void TouchHandler::Update()
    {
        touchPosition touchPos;
        u32 numTouches = hidTouchCount();

        // Start touching
        if (!touchesStarted && numTouches == 1)
        {
            hidTouchRead(&touchPos, 0);

            // The first touch must be within the desired boundaries
            if (!(touchPos.px >= m_touchAreaPos.x && touchPos.py >= m_touchAreaPos.y && touchPos.px <= m_touchAreaPos.x + m_touchAreaSize.width && touchPos.py <= m_touchAreaPos.y + m_touchAreaSize.height))
                return;

            touchesStarted = true;
            startTouchPos = touchPos;
            tapping = true;
            m_onTouchesStarted(startTouchPos.px, startTouchPos.py);
        }
        else if (touchesStarted) // Touching has already been started
        {
            if (numTouches >= 1)
            {
                hidTouchRead(&touchPos, 0);

                if (tapping && (abs(touchPos.px - startTouchPos.px) > MAX_TAP_MOVEMENT || abs(touchPos.py - startTouchPos.py) > MAX_TAP_MOVEMENT))
                    tapping = false;

                if (!tapping)
                    m_onTouchesMoving(startTouchPos.px, startTouchPos.py, touchPos.px - startTouchPos.px, touchPos.py - startTouchPos.py);
            }
            else // Stop touching
            {
                if (tapping)
                {
                    m_onTapped(startTouchPos.px, startTouchPos.py);
                }

                touchesStarted = false;
                tapping = false;
            }
        }
    }

    void TouchHandler::SetTouchArea(Position touchAreaPos, Dimensions touchAreaSize)
    {
        m_touchAreaPos = touchAreaPos;
        m_touchAreaSize = touchAreaSize;
    }

    Position TouchHandler::GetTouchAreaPos()
    {
        return m_touchAreaPos;
    }
}