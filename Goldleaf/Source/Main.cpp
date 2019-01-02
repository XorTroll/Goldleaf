#include <gleaf/Goldleaf>

int main()
{
    gleaf::Initialize();
    gleaf::ui::MainApplication *mainapp = new gleaf::ui::MainApplication();
    gleaf::ui::SetMainApplication(mainapp);
    mainapp->Show();
    delete mainapp;
    gleaf::Finalize();
    return 0;
}