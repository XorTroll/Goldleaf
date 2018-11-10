#pragma once

#include "ui/framework/view.hpp"
#include "ui/framework/layer.hpp"

#include <memory>
#include <vector>

namespace tin::ui
{
    class LayoutView : public View
    {
        protected:
            unsigned int m_unwindDistance = 1;
            std::vector<std::unique_ptr<Layer>> m_layers;

            virtual void OnPresented() override;
            virtual void ProcessInput(u64 keys) override;
            virtual void Update() override;
    
            void AddLayer(std::unique_ptr<Layer> layer);

        public:
            LayoutView(unsigned int unwindDistance = 1);
    };
}