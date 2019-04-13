#include <switch.h>
#include <string.h>
#include <stdio.h>
#include "hbl.h"

#define MODULE_HBL 347

const char* g_easterEgg = "Do you mean to tell me that you're thinking seriously of building that way, when and if you are an architect?";

static char g_argv[2048];
static char g_nextArgv[2048];
static char g_nextNroPath[512];
u64  g_nroAddr = 0;
static u64  g_nroSize = 0;
static NroHeader g_nroHeader;
static bool g_isApplication = 0;

static NsApplicationControlData g_applicationControlData;
static bool g_isAutomaticGameplayRecording = 0;
static bool g_smCloseWorkaround = false;

static u8 g_savedTls[0x100];

// Used by trampoline.s
Result g_lastRet = 0;

extern void* __stack_top;//Defined in libnx.
#define STACK_SIZE 0x100000 //Change this if main-thread stack size ever changes.

void __libnx_initheap(void)
{
    static char g_innerheap[0x20000];

    extern char* fake_heap_start;
    extern char* fake_heap_end;

    fake_heap_start = &g_innerheap[0];
    fake_heap_end   = &g_innerheap[sizeof g_innerheap];
}

void __appInit(void)
{
    (void) g_easterEgg[0];

    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 1));

    rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
    }

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 2));

    fsdevMountSdmc();
}

void __appExit(void)
{
    fsdevUnmountAll();
    fsExit();
    smExit();
}

static void*  g_heapAddr;
static size_t g_heapSize;

void setupHbHeap(void)
{
    u64 size = 0;
    void* addr = NULL;
    u64 mem_available = 0, mem_used = 0;
    Result rc=0;

    svcGetInfo(&mem_available, 6, CUR_PROCESS_HANDLE, 0);
    svcGetInfo(&mem_used, 7, CUR_PROCESS_HANDLE, 0);
    if (mem_available > mem_used+0x200000)
        size = (mem_available - mem_used - 0x200000) & ~0x1FFFFF;
    if (size==0)
        size = 0x2000000*16;

    if (size > 0x6000000 && g_isAutomaticGameplayRecording) {
        size -= 0x6000000;
    }

    rc = svcSetHeapSize(&addr, size);

    if (R_FAILED(rc) || addr==NULL)
        fatalSimple(MAKERESULT(MODULE_HBL, 9));

    g_heapAddr = addr;
    g_heapSize = size;
}

static Handle g_port;
static Handle g_procHandle;

void threadFunc(void* ctx)
{
    Handle session;
    Result rc;

    rc = svcWaitSynchronizationSingle(g_port, -1);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 22));

    rc = svcAcceptSession(&session, g_port);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 14));

    s32 idx = 0;
    rc = svcReplyAndReceive(&idx, &session, 1, 0, -1);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 15));

    IpcParsedCommand ipc;
    rc = ipcParse(&ipc);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 16));

    if (ipc.NumHandles != 1)
        fatalSimple(MAKERESULT(MODULE_HBL, 17));

    g_procHandle = ipc.Handles[0];
    svcCloseHandle(session);
}

//Gets the PID of the process with application_type==APPLICATION in the NPDM, then sets g_isApplication if it matches the current PID.
void getIsApplication(void) {
    Result rc=0;
    u64 cur_pid=0, app_pid=0;

    g_isApplication = 0;

    rc = svcGetProcessId(&cur_pid, CUR_PROCESS_HANDLE);
    if (R_FAILED(rc)) return;

    rc = pmshellInitialize();

    if (R_SUCCEEDED(rc)) {
        rc = pmshellGetApplicationPid(&app_pid);
        pmshellExit();
    }

    if (R_SUCCEEDED(rc) && cur_pid == app_pid) g_isApplication = 1;
}

//Gets the control.nacp for the current title id, and then sets g_isAutomaticGameplayRecording if less memory should be allocated.
void getIsAutomaticGameplayRecording(void) {
    if (kernelAbove500() && g_isApplication) {
        Result rc=0;
        u64 cur_tid=0;

        rc = svcGetInfo(&cur_tid, 18, CUR_PROCESS_HANDLE, 0);
        if (R_FAILED(rc)) return;

        g_isAutomaticGameplayRecording = 0;

        rc = nsInitialize();

        if (R_SUCCEEDED(rc)) {
            size_t dummy;
            rc = nsGetApplicationControlData(0x1, cur_tid, &g_applicationControlData, sizeof(g_applicationControlData), &dummy);
            nsExit();
        }

        if (R_SUCCEEDED(rc) && (((g_applicationControlData.nacp.x3034_unk >> 8) & 0xFF) == 2)) g_isAutomaticGameplayRecording = 1;
    }
}

void getOwnProcessHandle(void)
{
    static Thread t;
    Result rc;

    rc = threadCreate(&t, &threadFunc, NULL, 0x1000, 0x20, 0);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 10));

    rc = smRegisterService(&g_port, "hb:ldr", false, 1);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 12));

    rc = threadStart(&t);
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 13));

    Service srv;
    rc = smGetService(&srv, "hb:ldr");
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 23));

    IpcCommand ipc;
    ipcInitialize(&ipc);
    ipcSendHandleCopy(&ipc, 0xffff8001);

    struct {
        int x, y;
    }* raw;

    raw = ipcPrepareHeader(&ipc, sizeof(*raw));
    raw->x = raw->y = 0;

    rc = serviceIpcDispatch(&srv);

    threadWaitForExit(&t);
    threadClose(&t);

    serviceClose(&srv);
    svcCloseHandle(g_port);

    rc = smUnregisterService("hb:ldr");
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 11));

    smExit();
}

void loadNro(const char *path, const char *argv)
{
    NroHeader* header = NULL;
    size_t rw_size=0;
    Result rc=0;

    if (g_smCloseWorkaround) {
        // For old applications, wait for SM to handle closing the SM session from this process.
        // If we don't do this, smInitialize will fail once eventually used later.
        // This is caused by a bug in old versions of libnx that was fixed in commit 68a77ac950.
        g_smCloseWorkaround = false;
        svcSleepThread(1000000000);
    }

    memcpy((u8*)armGetTls() + 0x100, g_savedTls, 0x100);

    if (g_nroSize > 0)
    {
        // Unmap previous NRO.
        header = &g_nroHeader;
        rw_size = header->segments[2].size + header->bss_size;
        rw_size = (rw_size+0xFFF) & ~0xFFF;

        // .text
        rc = svcUnmapProcessCodeMemory(
            g_procHandle, g_nroAddr + header->segments[0].file_off, ((u64) g_heapAddr) + header->segments[0].file_off, header->segments[0].size);

        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(MODULE_HBL, 24));

        // .rodata
        rc = svcUnmapProcessCodeMemory(
            g_procHandle, g_nroAddr + header->segments[1].file_off, ((u64) g_heapAddr) + header->segments[1].file_off, header->segments[1].size);

        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(MODULE_HBL, 25));

       // .data + .bss
        rc = svcUnmapProcessCodeMemory(
            g_procHandle, g_nroAddr + header->segments[2].file_off, ((u64) g_heapAddr) + header->segments[2].file_off, rw_size);

        if (R_FAILED(rc))
            fatalSimple(MAKERESULT(MODULE_HBL, 26));

        g_nroAddr = g_nroSize = 0;
    }

    if (strlen(g_nextNroPath) == 0)
    {
        strcpy(g_nextNroPath, path);
        strcpy(g_nextArgv, argv);
    }

    memcpy(g_argv, g_nextArgv, sizeof g_argv);

    uint8_t *nrobuf = (uint8_t*) g_heapAddr;

    NroStart*  start  = (NroStart*)  (nrobuf + 0);
    header = (NroHeader*) (nrobuf + sizeof(NroStart));
    uint8_t*   rest   = (uint8_t*)   (nrobuf + sizeof(NroStart) + sizeof(NroHeader));

    FILE* f = fopen(g_nextNroPath, "rb");
    if (f == NULL)
        fatalSimple(MAKERESULT(MODULE_HBL, 3));

    // Reset NRO path to load hbmenu by default next time.
    g_nextNroPath[0] = '\0';

    if (fread(start, sizeof(*start), 1, f) != 1)
        fatalSimple(MAKERESULT(MODULE_HBL, 4));

    if (fread(header, sizeof(*header), 1, f) != 1)
        fatalSimple(MAKERESULT(MODULE_HBL, 4));

    if(header->magic != NROHEADER_MAGIC)
        fatalSimple(MAKERESULT(MODULE_HBL, 5));

    size_t rest_size = header->size - (sizeof(NroStart) + sizeof(NroHeader));
    if (fread(rest, rest_size, 1, f) != 1)
        fatalSimple(MAKERESULT(MODULE_HBL, 7));

    fclose(f);

    size_t total_size = header->size + header->bss_size;
    total_size = (total_size+0xFFF) & ~0xFFF;

    rw_size = header->segments[2].size + header->bss_size;
    rw_size = (rw_size+0xFFF) & ~0xFFF;

    bool has_mod0 = false;
    if (start->mod_offset > 0 && start->mod_offset <= (total_size-0x24)) // Validate MOD0 offset
        has_mod0 = *(uint32_t*)(nrobuf + start->mod_offset) == 0x30444F4D; // Validate MOD0 header

    int i;
    for (i=0; i<3; i++)
    {
        if (header->segments[i].file_off >= header->size || header->segments[i].size > header->size ||
            (header->segments[i].file_off + header->segments[i].size) > header->size)
        {
            fatalSimple(MAKERESULT(MODULE_HBL, 6));
        }
    }

    // todo: Detect whether NRO fits into heap or not.

    // Copy header to elsewhere because we're going to unmap it next.
    memcpy(&g_nroHeader, header, sizeof(g_nroHeader));
    header = &g_nroHeader;

    u64 map_addr;

    do {
        map_addr = randomGet64() & 0xFFFFFF000ull;
        rc = svcMapProcessCodeMemory(g_procHandle, map_addr, (u64)nrobuf, total_size);

    } while (rc == 0xDC01 || rc == 0xD401);

    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 18));

    // .text
    rc = svcSetProcessMemoryPermission(
        g_procHandle, map_addr + header->segments[0].file_off, header->segments[0].size, Perm_R | Perm_X);

    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 19));

    // .rodata
    rc = svcSetProcessMemoryPermission(
        g_procHandle, map_addr + header->segments[1].file_off, header->segments[1].size, Perm_R);

    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 20));

    // .data + .bss
    rc = svcSetProcessMemoryPermission(
        g_procHandle, map_addr + header->segments[2].file_off, rw_size, Perm_Rw);

    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 21));

    u64 nro_size = header->segments[2].file_off + rw_size;
    u64 nro_heap_start = ((u64) g_heapAddr) + nro_size;
    u64 nro_heap_size  = g_heapSize + (u64) g_heapAddr - (u64) nro_heap_start;

    #define M EntryFlag_IsMandatory

    static ConfigEntry entries[] = {
        { EntryType_MainThreadHandle,     0, {0, 0} },
        { EntryType_ProcessHandle,        0, {0, 0} },
        { EntryType_AppletType,           0, {AppletType_LibraryApplet, 0} },
        { EntryType_OverrideHeap,         M, {0, 0} },
        { EntryType_Argv,                 0, {0, 0} },
        { EntryType_NextLoadPath,         0, {0, 0} },
        { EntryType_LastLoadResult,       0, {0, 0} },
        { EntryType_SyscallAvailableHint, 0, {0xffffffffffffffff, 0x9fc1fff0007ffff} },
        { EntryType_RandomSeed,           0, {0, 0} },
        { EntryType_EndOfList,            0, {0, 0} }
    };

    ConfigEntry *entry_AppletType = &entries[2];

    if (g_isApplication) {
        entry_AppletType->Value[0] = AppletType_SystemApplication;
        entry_AppletType->Value[1] = EnvAppletFlags_ApplicationOverride;
    }

    // MainThreadHandle
    entries[0].Value[0] = envGetMainThreadHandle();
    // ProcessHandle
    entries[1].Value[0] = g_procHandle;
    // OverrideHeap
    entries[3].Value[0] = nro_heap_start;
    entries[3].Value[1] = nro_heap_size;
    // Argv
    entries[4].Value[1] = (u64) &g_argv[0];
    // NextLoadPath
    entries[5].Value[0] = (u64) &g_nextNroPath[0];
    entries[5].Value[1] = (u64) &g_nextArgv[0];
    // LastLoadResult
    entries[6].Value[0] = g_lastRet;
    // RandomSeed
    entries[8].Value[0] = randomGet64();
    entries[8].Value[1] = randomGet64();

    u64 entrypoint = map_addr;

    g_nroAddr = map_addr;
    g_nroSize = nro_size;

    memset(__stack_top - STACK_SIZE, 0, STACK_SIZE);

    if (!has_mod0) {
        // Apply sm-close workaround to NROs which do not contain a valid MOD0 header.
        // This heuristic is based on the fact that MOD0 support was added very shortly after
        // the fix for the sm-close bug (in fact, two commits later).
        g_smCloseWorkaround = true;
    }

    extern NORETURN void nroEntrypointTrampoline(u64 entries_ptr, u64 handle, u64 entrypoint);
    nroEntrypointTrampoline((u64) entries, -1, entrypoint);
}

void targetNro(const char *path, const char *argv)
{
    memcpy(g_savedTls, (u8*)armGetTls() + 0x100, 0x100);

    getIsApplication();
    getIsAutomaticGameplayRecording();
    setupHbHeap();
    getOwnProcessHandle();
    loadNro(path, argv);

    fatalSimple(MAKERESULT(MODULE_HBL, 8));
}