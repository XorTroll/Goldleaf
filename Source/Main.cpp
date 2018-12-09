#include <gleaf/gleaf.hpp>

int main()
{
    gleaf::Initialize();
    gleaf::ui::MainApplication *mainapp = new gleaf::ui::MainApplication();
    gleaf::ui::SetMainApplication(mainapp);
    mainapp->Show();
    gleaf::Finalize();
    return 0;
}