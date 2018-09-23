#include "ui/framework/list_element.hpp"

namespace tin::ui
{
    RowElement::RowElement(u32 width, u32 height) :
        BoxElement(width, height)
    {
        SubElementLayout rowSubElementLayout;
        rowSubElementLayout.arrangementType = SubElementArrangementType::BOTTOM_TO_TOP;
        this->SetColour(0x394566);
        this->SetSubElementLayout(rowSubElementLayout);

        auto underlineElement = std::make_unique<BoxElement>(m_dimensions.width, 3);
        underlineElement->SetColour(0x556C91);
        this->AddSubElement(std::move(underlineElement));

        auto contentElement = std::make_unique<BoxElement>(m_dimensions.width, m_dimensions.height - 3);
        SubElementLayout contentSubElementLayout;
        contentSubElementLayout.arrangementType = SubElementArrangementType::LEFT_TO_RIGHT;
        contentElement->SetColour(Colour(0, 0, 0, 0));
        contentElement->SetSubElementLayout(contentSubElementLayout);
        this->AddSubElement(std::move(contentElement));
    }

    RowElement::RowElement(u32 width) :
        RowElement(width, DEFAULT_ROW_HEIGHT)
    {

    }
}