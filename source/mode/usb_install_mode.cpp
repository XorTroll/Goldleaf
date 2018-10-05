#include "mode/usb_install_mode.hpp"

#include "ui/framework/console_view.hpp"
#include "nx/ipc/usb_comms_new.h"
#include "error.hpp"

namespace tin::ui
{
    USBInstallMode::USBInstallMode() :
        IMode("USB Install NSP")
    {

    }

    USBInstallMode::~USBInstallMode()
    {
    }

    void USBInstallMode::OnUnwound()
    {
        usbCommsExit();
    }

    void USBInstallMode::OnSelected()
    {
        tin::ui::ViewManager& manager = tin::ui::ViewManager::Instance();
        auto view = std::make_unique<tin::ui::ConsoleView>();
        manager.PushView(std::move(view));
    
        printf("Why hello there\n");
    
        ASSERT_OK(usbCommsInitialize(), "Failed to initialized usb comms!\n");

        printf("USB comms initialized\n");
    }
}