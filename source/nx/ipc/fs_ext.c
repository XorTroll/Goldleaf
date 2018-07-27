#include "nx/ipc/fs_ext.h"

#include <string.h>

Result fsOpenFileSystemWithId(const char* path, u64 fileSystemType, u64 titleId, FsFileSystem* out) 
{
	IpcCommand c;
	char send_path[FS_MAX_PATH+1] = {0};
    ipcInitialize(&c);
	ipcAddSendStatic(&c, send_path, FS_MAX_PATH, 0);

    struct {
        u64 magic;
        u64 cmd_id;
        u64 file_system_type;
		u64 title_id;
    } *raw;
	raw = ipcPrepareHeader(&c, sizeof(*raw));
	
	raw->magic = SFCI_MAGIC;
	raw->cmd_id = 8;
	raw->file_system_type = fileSystemType;
	raw->title_id = titleId;
	
	strncpy(send_path, path, FS_MAX_PATH);
	Result rc = serviceIpcDispatch(fsGetServiceSession());
	
	if (R_SUCCEEDED(rc)) {
		IpcParsedCommand r;
		ipcParse(&r);
		
		struct {
			u64 magic;
			u64 result;
		} *resp = r.Raw;
		
		rc = resp->result;
		
		if (R_SUCCEEDED(rc)) {
			serviceCreate(&out->s, r.Handles[0]);
		}
	}
	
	return rc;
}