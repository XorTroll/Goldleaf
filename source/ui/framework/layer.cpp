#include "ui/framework/layer.hpp"

#include "error.hpp"

namespace tin::ui
{
    Layer::Layer(std::string name) :
        m_name(name)
    {

    }

    void Layer::DrawLayer()
    {
        for (auto& element : m_elements)
        {
            element->DrawElement();
        }
    }

    std::string Layer::GetName()
    {
        return m_name;
    }

    void Layer::AddElement(std::unique_ptr<Element> element)
    {
        m_elements.push_back(std::move(element));
    }
}