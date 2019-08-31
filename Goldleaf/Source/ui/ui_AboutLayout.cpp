#include <ui/ui_AboutLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    AboutLayout::AboutLayout()
    {
        this->logoImage = pu::ui::elm::Image::New(85, 150, gsets.PathForResource("/Logo.png"));
        this->Add(this->logoImage);
    }
}