#include "nx/fs.hpp"

#include "error.hpp"

namespace nx::fs
{
    IFile::IFile() {}

    IFile::~IFile()
    {
        fsFileClose(&m_file);
    }

    Result IFile::OpenFile(FsFileSystem& fileSystem, std::string path)
    {
        PROPAGATE_RESULT(fsFsOpenFile(&fileSystem, path.c_str(), FS_OPEN_READ, &m_file), "Failed to open file");
        return 0;
    }

    Result IFile::Read(u64 offset, void* buf, size_t size, size_t* sizeReadOut)
    {
        PROPAGATE_RESULT(fsFileRead(&m_file, offset, buf, size, sizeReadOut), "Failed to read file");
        return 0;
    }

    Result IFile::GetSize(u64* sizeOut)
    {
        PROPAGATE_RESULT(fsFileGetSize(&m_file, sizeOut), "Failed to get file size");
        return 0;
    }
}