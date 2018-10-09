#include "nx/ipc/usb_new.h"

#include <string.h>
#include <switch/types.h>
#include <switch/result.h>
#include <switch/arm/cache.h>
#include <switch/kernel/ipc.h>
#include <switch/kernel/detect.h>
#include <switch/services/sm.h>
#include <switch/runtime/util/utf.h>

#define TOTAL_INTERFACES 4
#define TOTAL_ENDPOINTS_IN 16
#define TOTAL_ENDPOINTS_OUT 16
#define TOTAL_ENDPOINTS (TOTAL_ENDPOINTS_IN+TOTAL_ENDPOINTS_OUT)

static Service g_usbDsSrv;
static Event g_usbDsStateChangeEvent = {0};

static UsbDsInterface g_usbDsInterfaceTable[TOTAL_INTERFACES];
static UsbDsEndpoint g_usbDsEndpointTable[TOTAL_INTERFACES][TOTAL_ENDPOINTS];

static void _usbDsFreeTables(void);

static Result _usbDsBindDevice(UsbComplexId complexId);
static Result _usbDsBindClientProcess(Handle prochandle);
static Result _usbDsGetEvent(Service* srv, Event* event_out, u64 cmd_id);

Result usbDsInitialize(void)
{
    if (serviceIsActive(&g_usbDsSrv))
        return MAKERESULT(Module_Libnx, LibnxError_AlreadyInitialized);

    Result rc = 0;

    rc = smGetService(&g_usbDsSrv, "usb:ds");

    if (R_SUCCEEDED(rc))
        rc = _usbDsBindDevice(UsbComplexId_Default);
    if (R_SUCCEEDED(rc))
        rc = _usbDsBindClientProcess(CUR_PROCESS_HANDLE);

    // GetStateChangeEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&g_usbDsSrv, &g_usbDsStateChangeEvent, 3);
    
    // Result code doesn't matter here, users can call themselves later, too. This prevents foot shooting.
    if (R_SUCCEEDED(rc))
        usbDsClearDeviceData();

    if (R_FAILED(rc))
    {
        eventClose(&g_usbDsStateChangeEvent);

        serviceClose(&g_usbDsSrv);
    }

    return rc;
}

void usbDsExit(void)
{
    if (!serviceIsActive(&g_usbDsSrv))
        return;
    
    if (kernelAbove500()) {
        usbDsDisable();
    }

    _usbDsFreeTables();

    eventClose(&g_usbDsStateChangeEvent);

    serviceClose(&g_usbDsSrv);
}

Event* usbDsGetStateChangeEvent(void)
{
    return &g_usbDsStateChangeEvent;
}

static UsbDsInterface* _usbDsTryAllocateInterface(u32 num) {
    if (num >= TOTAL_INTERFACES) return NULL;
    UsbDsInterface* ptr = &g_usbDsInterfaceTable[num];
    if (ptr->initialized) return NULL;
    memset(ptr, 0, sizeof(UsbDsInterface));
    ptr->initialized = true;
    ptr->interface_index = num;
    return ptr;
}

static UsbDsInterface* _usbDsAllocateInterface(void)
{
    u32 pos;
    UsbDsInterface* ptr = NULL;

    for(pos=0; pos<TOTAL_INTERFACES; pos++)
    {
        ptr = &g_usbDsInterfaceTable[pos];
        if(ptr->initialized)continue;
        memset(ptr, 0, sizeof(UsbDsInterface));
        ptr->initialized = true;
        ptr->interface_index = pos;
        return ptr;
    }

    return NULL;
}

static UsbDsEndpoint* _usbDsAllocateEndpoint(UsbDsInterface* interface)
{
    u32 pos;
    UsbDsEndpoint* ptr = NULL;

    if(interface->interface_index>TOTAL_INTERFACES)return NULL;

    for(pos=0; pos<TOTAL_ENDPOINTS; pos++)
    {
        ptr = &g_usbDsEndpointTable[interface->interface_index][pos];
        if(ptr->initialized)continue;
        memset(ptr, 0, sizeof(UsbDsEndpoint));
        ptr->initialized = true;
        return ptr;
    }

    return NULL;
}

static void _usbDsFreeEndpoint(UsbDsEndpoint* endpoint)
{
    if (!endpoint->initialized)
        return;
    
    /* Cancel any ongoing transactions. */
    usbDsEndpoint_Cancel(endpoint);

    eventClose(&endpoint->CompletionEvent);

    serviceClose(&endpoint->h);

    endpoint->initialized = false;
}

static void _usbDsFreeInterface(UsbDsInterface* interface)
{
    if (!interface->initialized)
        return;
    
    /* Disable interface. */
    usbDsInterface_DisableInterface(interface);
    
    /* Close endpoints. */
    for (u32 ep = 0; ep < TOTAL_ENDPOINTS; ep++) {
        _usbDsFreeEndpoint(&g_usbDsEndpointTable[interface->interface_index][ep]);
    }

    eventClose(&interface->CtrlOutCompletionEvent);
    eventClose(&interface->CtrlInCompletionEvent);
    eventClose(&interface->SetupEvent);

    serviceClose(&interface->h);

    interface->initialized = false;
}

static void _usbDsFreeTables(void)
{
    for (u32 intf = 0; intf < TOTAL_INTERFACES; intf++) {
        _usbDsFreeInterface(&g_usbDsInterfaceTable[intf]);
    }
}

static Result _usbDsBindDevice(UsbComplexId complexId) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 complexId;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 0;
    raw->complexId = complexId;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}

static Result _usbDsBindClientProcess(Handle prochandle) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    ipcSendHandleCopy(&c, prochandle);

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}

static Result _usbDsGetEvent(Service* srv, Event* event_out, u64 cmd_id) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            eventLoadRemote(event_out, r.Handles[0], false);
        }
    }

    return rc;
}

Result usbDsGetState(u32 *out) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 4;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 out;
        } *resp = r.Raw;

        rc = resp->result;
        if (R_SUCCEEDED(rc) && out)*out = resp->out;
    }

    return rc;
}

Result usbDsWaitReady(u64 timeout) {
    Result rc;
    u32 state = 0;

    rc = usbDsGetState(&state);
    if (R_FAILED(rc)) return rc;

    while (R_SUCCEEDED(rc) && state != 5)
    {
        eventWait(&g_usbDsStateChangeEvent, timeout);
        eventClear(&g_usbDsStateChangeEvent);
        rc = usbDsGetState(&state);
    }

    return rc;
}

Result usbDsParseReportData(UsbDsReportData *reportdata, u32 urbId, u32 *requestedSize, u32 *transferredSize) {
    Result rc = 0;
    u32 pos;
    u32 count = reportdata->report_count;
    UsbDsReportEntry *entry = NULL;
    if(count>8)count = 8;

    for(pos=0; pos<count; pos++) {
        entry = &reportdata->report[pos];
        if (entry->id == urbId) break;
    }

    if (pos == count) return MAKERESULT(Module_Libnx, LibnxError_NotFound);

    switch(entry->urb_status) {
	    case 0x3:
            rc = 0;
        break;

        case 0x4:
            rc = 0x828c;
        break;

        case 0x5:
            rc = 0x748c;
        break;

        default:
            rc = 0x108c;
        break;
    }

    if (R_SUCCEEDED(rc)) {
        if (requestedSize) *requestedSize = entry->requestedSize;
        if (transferredSize) *transferredSize = entry->transferredSize;
    }

    return rc;
}

Result usbDsSetVidPidBcd(const UsbDsDeviceInfo* deviceinfo) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    ipcAddSendBuffer(&c, deviceinfo, sizeof(UsbDsDeviceInfo), 0);

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 5;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}

static Result _usbDsGetSession(Service* srv, Service* srv_out, u64 cmd_id, const void* buf0, size_t buf0size, const void* buf1, size_t buf1size) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    if (buf0 && buf0size)
        ipcAddSendBuffer(&c, buf0, buf0size, 0);
    if (buf1 && buf1size)
        ipcAddSendBuffer(&c, buf1, buf1size, 0);

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(srv_out, r.Handles[0]);
        }
    }

    return rc;
}

static Result _usbDsCmdNoParams(Service* srv, u64 cmd_id) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    return rc;
}

static Result _usbDsPostBuffer(Service* srv, u64 cmd_id, void* buffer, size_t size, u32 *urbId) {
    armDCacheFlush(buffer, size);

    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 size;
        u32 padding;
        u64 buffer;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;
    raw->size = (u32)size;
    raw->padding = 0;
    raw->buffer = (u64)buffer;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u32 urbId;
        } *resp = r.Raw;

        rc = resp->result;
        if (R_SUCCEEDED(rc) && urbId)*urbId = resp->urbId;
    }

    return rc;
}

static Result _usbDsGetReport(Service* srv, u64 cmd_id, UsbDsReportData *out) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = cmd_id;

    Result rc = serviceIpcDispatch(srv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            UsbDsReportData out;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc) && out)
            memcpy(out, &resp->out, sizeof(resp->out));
    }

    return rc;
}

Result usbDsGetDsInterface(UsbDsInterface** interface, struct usb_interface_descriptor* descriptor, const char *interface_name)
{
    UsbDsInterface* ptr = _usbDsAllocateInterface();
    if(ptr == NULL)
        return MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);

    Result rc = _usbDsGetSession(&g_usbDsSrv, &ptr->h, 2, descriptor, sizeof(struct usb_interface_descriptor), interface_name, strlen(interface_name)+1);

    // GetSetupEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&ptr->h, &ptr->SetupEvent, 1);
    // GetCtrlInCompletionEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&ptr->h, &ptr->CtrlInCompletionEvent, 7);
    // GetCtrlOutCompletionEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&ptr->h, &ptr->CtrlOutCompletionEvent, 9);

    if (R_FAILED(rc))
        _usbDsFreeInterface(ptr);

    if (R_SUCCEEDED(rc))
        *interface = ptr;

    return rc;
}

Result usbDsRegisterInterface(UsbDsInterface** interface, u32 intf_num)
{
    UsbDsInterface* ptr = _usbDsTryAllocateInterface(intf_num);
    if(ptr == NULL)
        return MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);

    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 intf_num;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 2;
    raw->intf_num = intf_num;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(&ptr->h, r.Handles[0]);
        }
    }

    // GetSetupEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&ptr->h, &ptr->SetupEvent, 1);
    // GetCtrlInCompletionEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&ptr->h, &ptr->CtrlInCompletionEvent, 7);
    // GetCtrlOutCompletionEvent
    if (R_SUCCEEDED(rc))
        rc = _usbDsGetEvent(&ptr->h, &ptr->CtrlOutCompletionEvent, 9);

    if (R_FAILED(rc))
        _usbDsFreeInterface(ptr);

    if (R_SUCCEEDED(rc))
        *interface = ptr;

    return rc;
}

Result usbDsClearDeviceData(void) {
    return _usbDsCmdNoParams(&g_usbDsSrv, 5);
}

static Result _usbDsAddUsbStringDescriptorRaw(u8 *out_index, struct usb_string_descriptor *descriptor) {
    IpcCommand c;
    ipcInitialize(&c);
    
    ipcAddSendBuffer(&c, descriptor, sizeof(*descriptor), 0);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 6;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u8 index;
        } *resp = r.Raw;

        rc = resp->result;
        if (R_SUCCEEDED(rc) && out_index) {
            *out_index = resp->index;
        }
    }
    
    return rc;
}

Result usbDsAddUsbStringDescriptor(u8* out_index, const char* string) {
    struct usb_string_descriptor descriptor = {
        .bDescriptorType = USB_DT_STRING,
        .wData = {0},
    };
    
    // Convert
    u32 len = (u32)utf8_to_utf16(descriptor.wData, (const uint8_t *)string, sizeof(descriptor.wData)/sizeof(u16) - 1);
    if (len > sizeof(descriptor.wData)/sizeof(u16)) len = sizeof(descriptor.wData)/sizeof(u16);
    
    // Set length
    descriptor.bLength = 2 + 2 * len;
    
    return _usbDsAddUsbStringDescriptorRaw(out_index, &descriptor);
}

Result usbDsAddUsbLanguageStringDescriptor(u8* out_index, const u16* lang_ids, u16 num_langs) {
    if (num_langs > 0x40) num_langs = 0x40;
    
    struct usb_string_descriptor descriptor = {
        .bLength = 2 + 2 * num_langs,
        .bDescriptorType = USB_DT_STRING,
        .wData = {0},
    };
    
    for (u32 i = 0; i < num_langs; i++) {
        descriptor.wData[i] = lang_ids[i];
    }
    
    return _usbDsAddUsbStringDescriptorRaw(out_index, &descriptor);
}

Result usbDsDeleteUsbStringDescriptor(u8 index) {
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u32 index;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 7;
    raw->index = index;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}


Result usbDsSetUsbDeviceDescriptor(UsbDeviceSpeed speed, struct usb_device_descriptor* descriptor) {
    IpcCommand c;
    ipcInitialize(&c);
    
    ipcAddSendBuffer(&c, descriptor, USB_DT_DEVICE_SIZE, 0);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 speed;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 8;
    raw->speed = speed;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}

Result usbDsSetBinaryObjectStore(void* bos, size_t bos_size) {
    IpcCommand c;
    ipcInitialize(&c);
    
    ipcAddSendBuffer(&c, bos, bos_size, 0);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 9;

    Result rc = serviceIpcDispatch(&g_usbDsSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}

Result usbDsEnable(void) {
    return _usbDsCmdNoParams(&g_usbDsSrv, 10);
}

Result usbDsDisable(void) {
    return _usbDsCmdNoParams(&g_usbDsSrv, 11);
}


//IDsInterface

void usbDsInterface_Close(UsbDsInterface* interface)
{
    _usbDsFreeInterface(interface);
}

Result usbDsInterface_GetSetupPacket(UsbDsInterface* interface, void* buffer, size_t size) {
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
    
    IpcCommand c;
    ipcInitialize(&c);
    
    ipcAddRecvBuffer(&c, buffer, size, 0);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 2;

    Result rc = serviceIpcDispatch(&interface->h);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}

Result usbDsInterface_GetDsEndpoint(UsbDsInterface* interface, UsbDsEndpoint** endpoint, struct usb_endpoint_descriptor* descriptor)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    UsbDsEndpoint* ptr = _usbDsAllocateEndpoint(interface);
    if(ptr==NULL)return MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);

    Result rc = _usbDsGetSession(&interface->h, &ptr->h, 0, descriptor, USB_DT_ENDPOINT_SIZE, NULL, 0);

    if (R_SUCCEEDED(rc)) rc = _usbDsGetEvent(&ptr->h, &ptr->CompletionEvent, 2);//GetCompletionEvent

    if (R_FAILED(rc)) _usbDsFreeEndpoint(ptr);

    if (R_SUCCEEDED(rc)) *endpoint = ptr;
    return rc;
}

Result usbDsInterface_EnableInterface(UsbDsInterface* interface)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsCmdNoParams(&interface->h, 3);
}

Result usbDsInterface_DisableInterface(UsbDsInterface* interface)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsCmdNoParams(&interface->h, 4);
}

Result usbDsInterface_CtrlInPostBufferAsync(UsbDsInterface* interface, void* buffer, size_t size, u32 *urbId)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsPostBuffer(&interface->h, 5, buffer, size, urbId);
}

Result usbDsInterface_CtrlOutPostBufferAsync(UsbDsInterface* interface, void* buffer, size_t size, u32 *urbId)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsPostBuffer(&interface->h, 6, buffer, size, urbId);
}

Result usbDsInterface_GetCtrlInReportData(UsbDsInterface* interface, UsbDsReportData *out)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsGetReport(&interface->h, 8, out);
}

Result usbDsInterface_GetCtrlOutReportData(UsbDsInterface* interface, UsbDsReportData *out)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsGetReport(&interface->h, 10, out);
}

Result usbDsInterface_StallCtrl(UsbDsInterface* interface)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsCmdNoParams(&interface->h, 11);
}

Result usbDsInterface_RegisterEndpoint(UsbDsInterface* interface, UsbDsEndpoint** endpoint, u8 endpoint_address)
{
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    UsbDsEndpoint* ptr = _usbDsAllocateEndpoint(interface);
    if(ptr==NULL)return MAKERESULT(Module_Libnx, LibnxError_OutOfMemory);

    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 ep_addr;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 0;
    raw->ep_addr = endpoint_address;

    Result rc = serviceIpcDispatch(&interface->h);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            serviceCreate(&ptr->h, r.Handles[0]);
        }
    }
    
    if (R_SUCCEEDED(rc)) rc = _usbDsGetEvent(&ptr->h, &ptr->CompletionEvent, 2);//GetCompletionEvent

    if (R_FAILED(rc)) _usbDsFreeEndpoint(ptr);

    if (R_SUCCEEDED(rc)) *endpoint = ptr;
    return rc;
}

Result usbDsInterface_AppendConfigurationData(UsbDsInterface* interface, UsbDeviceSpeed speed, void* buffer, size_t size) {
    if(!interface->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
    
    IpcCommand c;
    ipcInitialize(&c);
    
    ipcAddSendBuffer(&c, buffer, size, 0);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 intf_num;
        u32 speed;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 12;
    raw->intf_num = interface->interface_index;
    raw->speed = speed;

    Result rc = serviceIpcDispatch(&interface->h);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}

//IDsEndpoint

void usbDsEndpoint_Close(UsbDsEndpoint* endpoint)
{
    _usbDsFreeEndpoint(endpoint);
}

Result usbDsEndpoint_PostBufferAsync(UsbDsEndpoint* endpoint, void* buffer, size_t size, u32 *urbId)
{
    if(!endpoint->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsPostBuffer(&endpoint->h, 0, buffer, size, urbId);
}

Result usbDsEndpoint_Cancel(UsbDsEndpoint* endpoint)
{
    if(!endpoint->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsCmdNoParams(&endpoint->h, 1);
}

Result usbDsEndpoint_GetReportData(UsbDsEndpoint* endpoint, UsbDsReportData *out)
{
    if(!endpoint->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsGetReport(&endpoint->h, 3, out);
}

Result usbDsEndpoint_StallCtrl(UsbDsEndpoint* endpoint)
{
    if(!endpoint->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);

    return _usbDsCmdNoParams(&endpoint->h, 4);
}

Result usbDsEndpoint_SetZlt(UsbDsEndpoint* endpoint, bool zlt)
{
    if(!endpoint->initialized)return MAKERESULT(Module_Libnx, LibnxError_NotInitialized);
    
    IpcCommand c;
    ipcInitialize(&c);
    
    struct {
        u64 magic;
        u64 cmd_id;
        u32 zlt;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 5;
    raw->zlt = zlt ? 1 : 0;

    Result rc = serviceIpcDispatch(&endpoint->h);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }
    
    return rc;
}