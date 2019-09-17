#include <nsp/nsp_Builder.hpp>
#include <fs/fs_Explorer.hpp>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

namespace nsp
{
    bool BuildNew(pu::String Input, pu::String Out, std::function<void(u64, u64)> Callback)
    {
        auto exp = fs::GetExplorerForPath(Input);
        auto files = exp->GetFiles(Input);
        PFS0Header header = {};
        header.FileCount = (u32)files.size();
        header.Magic = Magic;
        u8 *strtable = fs::GetFileSystemOperationsBuffer();
        memset(strtable, 0, fs::GetFileSystemOperationsBufferSize());
        size_t strtablesize = 0;
        std::vector<PFS0File> fentries;
        size_t base_offset = 0;
        for(auto &file: files)
        {
            PFS0File entry = {};
            entry.Entry.Offset = base_offset;
            entry.Entry.StringTableOffset = strtablesize;
            auto fsize = exp->GetFileSize(Input + "/" + file);
            entry.Entry.Size = fsize;
            entry.Name = file;
            base_offset += fsize;
            strcpy((char*)&strtable[strtablesize], file.AsUTF8().c_str());
            strtablesize += file.length() + 1; // NUL terminated!
            fentries.push_back(entry);
        }
        strtablesize = (strtablesize + 0x1f) &~ 0x1f;
        size_t offset_afterpfs0 = sizeof(PFS0Header) + (sizeof(PFS0FileEntry) * header.FileCount) + strtablesize;
        header.StringTableSize = strtablesize;
        auto outexp = fs::GetExplorerForPath(Out);
        outexp->WriteFileBlock(Out, (u8*)&header, sizeof(PFS0Header));
        for(auto &entry: fentries)
        {
            outexp->WriteFileBlock(Out, (u8*)&entry.Entry, sizeof(PFS0FileEntry));
        }
        outexp->WriteFileBlock(Out, strtable, strtablesize);
        size_t done = 0;
        for(auto &entry: fentries)
        {
            size_t toread = entry.Entry.Size;
            u8 *buf = fs::GetFileSystemOperationsBuffer();
            size_t readsz = fs::GetFileSystemOperationsBufferSize();
            size_t fdone = 0;
            while(toread)
            {
                auto read = exp->ReadFileBlock(Input + "/" + entry.Name, fdone, std::min(toread, readsz), buf);
                outexp->WriteFileBlock(Out, buf, read);
                fdone += read;
                done += read;
                toread -= read;
                Callback(done, base_offset);
            }
        }
        return true;
    }

    int Build(pu::String ContentsDir, pu::String Out, std::function<void(u64, u64)> Callback)
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
        PFS0FileEntry fsentries[0x400];
        PFS0FileEntry *fsentry;
        char objpath[257];
        char stringtable[0x4000];
        memset(&header, 0, sizeof(header));
        memset(fsentries, 0, sizeof(fsentries));
        memset(stringtable, 0, sizeof(stringtable));
        DIR *dir = opendir(ContentsDir.AsUTF8().c_str());
        if(dir == NULL) return 1;
        FILE *fout = fopen(Out.AsUTF8().c_str(), "wb");
        if(fout == NULL)
        {
            closedir(dir);
            return 2;
        }
        while((cur_dirent = readdir(dir)))
        {
            if(strcmp(cur_dirent->d_name, ".") == 0 || strcmp(cur_dirent->d_name, "..") == 0) continue;
            memset(objpath, 0, sizeof(objpath));
            snprintf(objpath, sizeof(objpath)-1, "%s/%s", ContentsDir.AsUTF8().c_str(), cur_dirent->d_name);
            if(stat(objpath, &objstats) == -1)
            {
                ret = false;
                break;
            }
            if((objstats.st_mode & S_IFMT) == S_IFREG)
            {
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
                snprintf(objpath, sizeof(objpath) - 1, "%s/%s", ContentsDir.AsUTF8().c_str(), &stringtable[stringtable_offset]);
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
                    Callback(szread, fsentries[pos].Size);
                }
                fclose(fin);
            }
        }
        fclose(fout);
        return ret;
    }
}