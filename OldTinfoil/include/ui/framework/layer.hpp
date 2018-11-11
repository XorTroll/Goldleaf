#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ui/framework/element.hpp"
#include "ui/framework/layout.hpp"

namespace tin::ui
{
    struct ElementEntry
    {
        std::unique_ptr<Element> element;
        Position position;

        ElementEntry(std::unique_ptr<Element> element, Position position) :
            element(std::move(element)), position(position)
        {
            
        }
    };

    class Layer
    {
        private:
            std::string m_name;
            std::vector<std::unique_ptr<ElementEntry>> m_elementEntries;

        public:
            Layer(std::string name);

            void UpdateLayer();
            void DrawLayer();

            std::string GetName();
            void AddElement(std::unique_ptr<Element> element, unsigned int x, unsigned int y);
    };
}