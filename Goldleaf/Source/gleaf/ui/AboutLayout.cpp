#include <gleaf/ui.hpp>

extern gleaf::set::Settings gsets;

namespace gleaf::ui
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