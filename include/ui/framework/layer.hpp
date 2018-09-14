#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ui/framework/element.hpp"

namespace tin::ui
{
    class Layer
    {
        private:
            std::string m_name;
            std::vector<std::unique_ptr<Element>> m_elements;

        public:
            Layer(std::string name);

            void DrawLayer();

            std::string GetName();
            void AddElement(std::unique_ptr<Element> element);
    };
}