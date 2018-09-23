#include "ui/framework/list_element.hpp"

#include "ui/framework/text_element.hpp"

namespace tin::ui
{
    RowElement::RowElement(u32 width, u32 height) :
        BoxElement(width, height)
    {
        SubElementLayout rowSubElementLayout;
        rowSubElementLayout.arrangementType = SubElementArrangementType::BOTTOM_TO_TOP;
        this->SetColour(Colour(0, 0, 0, 0));
        this->SetSubElementLayout(rowSubElementLayout);

        auto underlineElement = std::make_unique<BoxElement>(m_dimensions.width, 3);
        underlineElement->SetColour(0x556C91);
        this->AddSubElement(std::move(underlineElement));

        auto contentElement = std::make_unique<BoxElement>(m_dimensions.width, m_dimensions.height - 3);
        SubElementLayout contentSubElementLayout;
        contentSubElementLayout.arrangementType = SubElementArrangementType::LEFT_TO_RIGHT;
        contentElement->SetColour(Colour(0, 0, 0, 0));
        contentElement->SetSubElementLayout(contentSubElementLayout);

        auto textElement = std::make_unique<TextElement>(m_dimensions.width * 0.8, m_dimensions.height);
        textElement->SetText("Some Random Game");
        textElement->SetScale(5);
        textElement->SetColour(0xC3D0DF);
        textElement->SetInsets(0, 28);

        contentElement->AddSubElement(std::move(textElement));
        this->AddSubElement(std::move(contentElement));
    }

    RowElement::RowElement(u32 width) :
        RowElement(width, DEFAULT_ROW_HEIGHT)
    {

    }
}