#include <gleaf/Goldleaf>

#ifdef GOLDLEAF_QLAUNCH
    extern u32 __nx_applet_type = AppletType_SystemApplet;
#endif

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