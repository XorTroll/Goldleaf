
#pragma once
#include <pu/element.hpp>

namespace pu
{
    class Layout
    {
        public:
            Layout();
            void AddChild(element::Element *Child);
            void ClearChildren();
            element::Element *GetChildAt(u32 Index);
            u32 GetChildCount();
            bool HasChilds();
            void SetOnInput(std::function<void(u64 Input)> Callback);
            std::function<void(u64 Input)> GetOnInput();
        private:
            std::function<void(u64 Input)> onipt;
            std::vector<element::Element*> chld;
    };
}