#include <Goldleaf.hpp>

extern u32 __nx_applet_type = AppletType_SystemApplet;

int main()
{
    // Launches Goldleaf NRO with args: "<Goldleaf NRO> qlaunch", so Goldleaf will be booted as HOME menu.
    ForwarderProcess("qlaunch");
    return 0;
}