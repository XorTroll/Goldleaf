#include "install/nsp.hpp"

#include <memory>
#include "error.hpp"

namespace tin::install::nsp
{
    NSPEnclosedFile::NSPEnclosedFile(std::string name, size_t absoluteNSPFileOffset, size_t fileSize) :
        m_name(name), m_absoluteNSPFileOffset(absoluteNSPFileOffset), m_fileSize(fileSize)
    {

    }

    NSPContainer::NSPContainer() {}

    NSPContainer::~NSPContainer()
    {
        this->CloseContainer();
    }

    Result NSPContainer::OpenContainer(std::string path)
    {
        PROPAGATE_RESULT(fsFsOpenFile(&m_fileSystem, path.c_str(), FS_OPEN_READ, &m_fsFile), "Failed to open file");
        PROPAGATE_RESULT(this->ReadNSPHeader(), "Failed to read NSP header");

        return 0;
    }

    void NSPContainer::CloseContainer()
    {
        fsFileClose(&m_fsFile);
        fsFsClose(&m_fileSystem);
    }

    Result NSPContainer::ReadFile(std::string name, u8* buff, size_t size, size_t offset)
    {
        NSPEnclosedFile *enclosedFile;
        size_t actualReadSize = 0;

        PROPAGATE_RESULT(this->GetEnclosedFile(&enclosedFile, name), "Failed to get enclosed file");
        PROPAGATE_RESULT(fsFileRead(&m_fsFile, enclosedFile->m_absoluteNSPFileOffset + offset, buff, size, &actualReadSize), "Failed to read file into buffer");

        if (actualReadSize != size)
        {
            printf("readNspInstallFile: Size read 0x%lx doesn't match expected size 0x%lx", actualReadSize, size);
            return -1;
        }

        return 0;
    }

    Result NSPContainer::GetFileSize(std::string name, size_t* sizeOut)
    {
        NSPEnclosedFile* enclosedFile;
        PROPAGATE_RESULT(this->GetEnclosedFile(&enclosedFile, name), "Failed to get enclosed file")
        *sizeOut = enclosedFile->m_fileSize;
        return 0;
    }

    bool NSPContainer::HasFile(std::string name)
    {
        NSPEnclosedFile* enclosedFile;
        
        if (R_FAILED(this->GetEnclosedFile(&enclosedFile, name)))
            return false;

        return true;
    }

    std::string NSPContainer::FindFile(std::function<bool (std::string)>& comparator)
    {
        for (auto &enclosedFile : m_enclosedFiles)
        {
            if (comparator(enclosedFile.m_name))
                return enclosedFile.m_name;
        }

        return "";
    }

    Result NSPContainer::ReadNSPHeader()
    {
        size_t actualReadSize = 0;

        struct PFS0StartHeader
        {
            u32 magic;
            u32 numFiles;
            u32 stringTableSize;
            u32 reserved;
        } startHeader;

        struct PFS0FileEntry
        {
            u64 dataOffset;
            u64 fileSize;
            u32 stringTableOff;
            u32 reserved;
        };

        PROPAGATE_RESULT(fsFileRead(&m_fsFile, 0x0, &startHeader, sizeof(PFS0StartHeader), &actualReadSize), "Failed to read nsp header into buffer");

        if (actualReadSize != sizeof(PFS0StartHeader))
        {
            printf("openNspInstallFile: Mismatch between actual size read and start header size\n");
            return -1;
        }

        size_t fullHeaderSize = sizeof(PFS0StartHeader) + startHeader.numFiles * sizeof(PFS0FileEntry) + startHeader.stringTableSize;
        auto fullHeaderBuf = std::make_unique<u8[]>(fullHeaderSize);
        actualReadSize = 0;

        PROPAGATE_RESULT(fsFileRead(&m_fsFile, 0x0, fullHeaderBuf.get(), fullHeaderSize, &actualReadSize), "Failed to read full nsp header into buffer!");

        if (actualReadSize != fullHeaderSize)
        {
            printf("openNspInstallFile: Mismatch between actual size read and start header size\n");
            return -1;
        }

        for (unsigned int i = 0; i < startHeader.numFiles; i++)
        {
            PFS0FileEntry *fileEntry = (PFS0FileEntry *)(fullHeaderBuf.get() + sizeof(PFS0StartHeader) + i * sizeof(PFS0FileEntry));
            char *fileEntryName = (char *)(fullHeaderBuf.get() + sizeof(PFS0StartHeader) + startHeader.numFiles * sizeof(PFS0FileEntry) + fileEntry->stringTableOff);

            m_enclosedFiles.push_back(NSPEnclosedFile(fileEntryName, fullHeaderSize + fileEntry->dataOffset, fileEntry->fileSize));
        }

        return 0;
    }

    Result NSPContainer::GetEnclosedFile(NSPEnclosedFile **out, std::string name)
    {
        for (auto &enclosedFile : m_enclosedFiles)
        {
            if (enclosedFile.m_name == name)
            {
                *out = &enclosedFile;
                return 0;
            }
        }

        return -1;
    }
}

