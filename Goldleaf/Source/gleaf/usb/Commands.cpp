#include <gleaf/usb/Commands.hpp>
#include <gleaf/usb/Communications.hpp>
#include <gleaf/ui.hpp>
#include <malloc.h>
#include <algorithm>

namespace gleaf::ui
{
    extern gleaf::ui::MainApplication *mainapp;
}

namespace gleaf::usb
{
    bool Command::MagicOk()
    {
        return (this->Magic == GLUC);
    }

    bool Command::IsCommandId(usb::CommandId Id)
    {
        return (this->CommandId == static_cast<u32>(Id));
    }

    Command MakeCommand(CommandId Id)
    {
        Command cmd;
        cmd.Magic = GLUC;
        cmd.CommandId = static_cast<u32>(Id);
        return cmd;
    }

    int OnContentRead(void *Args)
    {
        ContentThreadArguments *args = reinterpret_cast<ContentThreadArguments*>(Args);
        Command cmd = MakeCommand(CommandId::NSPContent);
        WriteCommand(cmd);
        Write32(args->Index);
        u64 rsize = 0x800000;
        u8 *data = (u8*)memalign(0x1000, rsize);
        u64 szrem = args->Size;
        size_t tmpread = 0;
        while(szrem)
        {
            tmpread = usbCommsRead(data, std::min(szrem, rsize));
            szrem -= tmpread;
            while(!args->WriterRef->CanAppendData(tmpread));
            args->WriterRef->AppendData(data, tmpread);
        }
        free(data);
        return 0;
    }

    int OnContentAppend(void *Args)
    {
        ContentThreadArguments *args = reinterpret_cast<ContentThreadArguments*>(Args);
        while(!args->WriterRef->IsPlaceHolderComplete()) if(args->WriterRef->CanWriteSegmentToPlaceHolder()) args->WriterRef->WriteSegmentToPlaceHolder();
        return 0;
    }
}