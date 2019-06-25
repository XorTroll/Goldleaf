#include <ui/ui_AboutLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication *mainapp;

    AboutLayout::AboutLayout()
    {
        this->logoImage = new pu::element::Image(85, 150, gsets.PathForResource("/Logo.png"));
        this->Add(this->logoImage);
    }

    AboutLayout::~AboutLayout()
    {
        delete this->logoImage;
    }
}