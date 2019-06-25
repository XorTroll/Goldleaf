#include <nsp/nsp_Builder.hpp>
#include <fs/fs_Explorer.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

namespace nsp
{
    int Build(std::string ContentsDir, std::string Out, std::function<void(u8 Percentage)> Callback)
    {
        struct dirent *cur_dirent = NULL;
        struct stat objstats;
        int ret = 0;
        u64 tmplen = 0;
        u32 pos;
        u32 objcount = 0;
        u32 stringtable_offset=0;
        u64 filedata_reloffset=0;
        PFS0Header header;
        PFS0FileEntry fsentries[0x20];
        PFS0FileEntry *fsentry;
        char objpath[257];
        char stringtable[0x100];
        memset(&header, 0, sizeof(header));
        memset(fsentries, 0, sizeof(fsentries));
        memset(stringtable, 0, sizeof(stringtable));
        DIR *dir = opendir(ContentsDir.c_str());
        if(dir == NULL) return 1;
        FILE *fout = fopen(Out.c_str(), "wb");
        if(fout == NULL)
        {
            closedir(dir);
            return 2;
        }
        while((cur_dirent = readdir(dir)))
        {
            if(strcmp(cur_dirent->d_name, ".") == 0 || strcmp(cur_dirent->d_name, "..") == 0) continue;
            memset(objpath, 0, sizeof(objpath));
            snprintf(objpath, sizeof(objpath)-1, "%s/%s", ContentsDir.c_str(), cur_dirent->d_name);
            if(stat(objpath, &objstats) == -1)
            {
                ret = false;
                break;
            }
            if((objstats.st_mode & S_IFMT) == S_IFREG)
            {
                if(objcount >= 0x10)
                {
                    ret = false;
                    break;
                }
                fsentry = &fsentries[objcount];
                fsentry->Offset = filedata_reloffset;
                fsentry->Size = objstats.st_size;
                filedata_reloffset+= fsentry->Size;
                fsentry->StringTableOffset = stringtable_offset;
                tmplen = strlen(cur_dirent->d_name) + 1;
                if((stringtable_offset + tmplen) > sizeof(stringtable))
                {
                    ret = false;
                    break;
                }
                strncpy(&stringtable[stringtable_offset], cur_dirent->d_name, sizeof(stringtable) - stringtable_offset);
                stringtable_offset+= tmplen;
                objcount++;
            }
        }
        closedir(dir);
        if(ret == 0)
        {
            stringtable_offset = (stringtable_offset + 0x1f) & ~0x1f;
            header.Magic = Magic;
            header.FileCount = objcount;
            header.StringTableSize = stringtable_offset;
            fwrite(&header, 1, sizeof(header), fout);
            fwrite(fsentries, 1, sizeof(PFS0FileEntry) * objcount, fout);
            fwrite(stringtable, 1, stringtable_offset, fout);
            stringtable_offset = 0;
            for(pos = 0; pos < objcount; pos++)
            {
                tmplen = strlen(&stringtable[stringtable_offset]);
                if(tmplen == 0)
                {
                    ret = false;
                    break;
                }
                tmplen++;
                if(stringtable_offset+tmplen > sizeof(stringtable))
                {
                    ret = false;
                    break;
                }
                memset(objpath, 0, sizeof(objpath));
                snprintf(objpath, sizeof(objpath) - 1, "%s/%s", ContentsDir.c_str(), &stringtable[stringtable_offset]);
                stringtable_offset+=tmplen;
                FILE *fin = fopen(objpath, "rb");
                if(fin == NULL)
                {
                    ret = false;
                    break;
                }
                u64 rsize = fs::GetFileSystemOperationsBufferSize();
                u64 szread = 0;
                u64 szrem = fsentries[pos].Size;
                u8 *tmpbuf = fs::GetFileSystemOperationsBuffer();
                while(szrem)
                {
                    u64 rrsize = std::min(rsize, szrem);
                    tmplen = fread(tmpbuf, 1, rrsize, fin);
                    szrem -= tmplen;
                    szread += tmplen;
                    fwrite(tmpbuf, 1, tmplen, fout);
                    u8 pc = (u8)((double)szread / (double)fsentries[pos].Size * 100.0);
                    Callback(pc);
                }
                fclose(fin);
            }
        }
        fclose(fout);
        return ret;
    }
}