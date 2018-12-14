
#pragma once
#include <pn/fw.hpp>

namespace pn
{
    class Layout
    {
        public:
            Layout();
            void AddChild(fw::Element *Child);
            void ClearChildren();
            fw::Element *GetChildAt(u32 Index);
            u32 GetChildCount();
            bool HasChilds();
            void SetOnInput(std::function<void(u64 Input)> Callback);
            std::function<void(u64 Input)> GetOnInput();
        private:
            std::function<void(u64 Input)> onipt;
            std::vector<fw::Element*> chld;
    };
}