#include "install/verify_nsp.hpp"

#include <exception>

#include "install/verify_nsp.hpp"
#include "nx/fs.hpp"

namespace tin::install
{
    NSPVerifier::NSPVerifier(std::string nspPath) :
        m_nspPath(nspPath)
    {

    }

    bool NSPVerifier::PerformVerification()
    {
        bool isValid = true;
        nx::fs::IFileSystem nspFileSystem;

        try
        {
            nspFileSystem.OpenFileSystemWithId(m_nspPath, FsFileSystemType_ApplicationPackage, 0);
        }
        catch (std::exception& e)
        {
            printf("NSP is invalid and cannot be opened!\n");
            return false;
        }
        printf("PFS0 structure is valid\n");

        

        if (isValid)
            printf("NSP is valid.\n");

        return isValid;
    }
}