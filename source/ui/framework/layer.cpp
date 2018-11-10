#include "ui/framework/layer.hpp"

#include "error.hpp"

namespace tin::ui
{
    Layer::Layer(std::string name) :
        m_name(name)
    {

    }

    void Layer::UpdateLayer()
    {
        for (auto& entry : m_elementEntries)
        {
            entry->element->Update(entry->position);
        }
    }

    void Layer::DrawLayer()
    {
        for (auto& entry : m_elementEntries)
        {
            entry->element->Draw(Canvas().Intersect(entry->position, entry->element->GetDimensions()), entry->position);
        }
    }

    std::string Layer::GetName()
    {
        return m_name;
    }

    void Layer::AddElement(std::unique_ptr<Element> element, unsigned int x, unsigned int y)
    {
        auto entry = std::make_unique<ElementEntry>(std::move(element), Position(x, y));
        m_elementEntries.push_back(std::move(entry));
    }
}