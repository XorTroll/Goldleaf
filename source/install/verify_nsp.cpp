#include "install/verify_nsp.hpp"

#include <exception>
#include <string>
#include <tuple>

#include "install/simple_filesystem.hpp"
#include "install/verify_nsp.hpp"
#include "nx/fs.hpp"
#include "nx/ncm.hpp"
#include "util/file_util.hpp"
#include "util/title_util.hpp"

namespace tin::install
{
    NSPVerifier::NSPVerifier(std::string nspPath) :
        m_nspPath(nspPath)
    {

    }

    bool NSPVerifier::PerformVerification()
    {
        bool isComplete = true;
        nx::fs::IFileSystem nspFileSystem;

        try
        {
            nspFileSystem.OpenFileSystemWithId(m_nspPath, FsFileSystemType_ApplicationPackage, 0);
        }
        catch (std::exception& e)
        {
            printf("[CRITICAL] NSP is invalid and cannot be opened! Error: %s\n", e.what());
            printf("> NSP Path: %s\n", m_nspPath.c_str());
            return false;
        }
        printf("PFS0 structure is valid\n");

        tin::install::nsp::SimpleFileSystem simpleFS(nspFileSystem, "/", m_nspPath + "/");

        std::string cnmtNCAPath;
        nx::ncm::ContentRecord cnmtContentRecord;
        nx::ncm::ContentMeta contentMeta;

        try
        {
            std::tie(cnmtNCAPath, cnmtContentRecord) = tin::util::GetCNMTNCAInfo(m_nspPath);
        }
        catch (std::exception& e)
        {
            printf("[CRITICAL] Failed to find CNMT NCA. Error: %s\n", e.what());
            return false;
        }
        
        try
        {
            contentMeta = tin::util::GetContentMetaFromNCA(cnmtNCAPath);
        }
        catch (std::exception& e)
        {
            printf("[CRITICAL] Content meta could not be read. Error: %s\n", e.what());
            return false;
        }
        printf("Successfully read content meta.\n");

        if (!simpleFS.HasFile(tin::util::GetNcaIdString(cnmtContentRecord.ncaId) + ".cnmt.xml"))
            printf("[WARNING] CNMT XML is absent!\n");
        else
            printf("CNMT XML is present.\n");

        u64 titleId = contentMeta.GetContentMetaHeader().titleId;

        for (nx::ncm::ContentRecord contentRecord : contentMeta.GetContentRecords())
        {
            std::string ncaIdStr = tin::util::GetNcaIdString(contentRecord.ncaId);
            std::string ncaName = ncaIdStr + ".nca";
            std::string xmlName = ncaIdStr + ".";

            if (simpleFS.HasFile(ncaName))
                printf("%s is present.\n", ncaName.c_str());
            else
            {
                printf("[CRITICAL] %s is missing!\n", ncaName.c_str());
                return false;
            }

            // control, legalinfo

            switch (contentRecord.contentType)
            {
                case nx::ncm::ContentType::PROGRAM:
                    xmlName += "programinfo.xml";
                    break;

                case nx::ncm::ContentType::DATA:
                    xmlName = "";
                    break;

                case nx::ncm::ContentType::CONTROL:
                    try
                    {
                        nx::fs::IFileSystem controlFileSystem;
                        controlFileSystem.OpenFileSystemWithId(m_nspPath + "/" + ncaName, FsFileSystemType_ContentControl, titleId);
                    }
                    catch (std::exception& e)
                    {
                        printf("[CRITICAL] Control NCA could not be read. Error: %s\n", e.what());
                        return false;
                    }
                    printf("Control NCA is valid.\n");

                    xmlName += "nacp.xml";
                    break;

                case nx::ncm::ContentType::HTML_DOCUMENT:
                    xmlName += "htmldocument.xml";
                    break;

                case nx::ncm::ContentType::LEGAL_INFORMATION:
                    try
                    {
                        nx::fs::IFileSystem legalInfoFileSystem;
                        legalInfoFileSystem.OpenFileSystemWithId(m_nspPath + "/" + ncaName, FsFileSystemType_ContentManual, titleId);
                    }
                    catch (std::exception& e)
                    {
                        printf("[CRITICAL] Legal information NCA could not be read. Error: %s\n", e.what());
                        return false;
                    }
                    printf("Legal information NCA is valid.\n");

                    xmlName += "legalinfo.xml";
                    break;

                // We ignore delta fragments (for now) since they aren't all included,
                // and we don't install them
                case nx::ncm::ContentType::DELTA_FRAGMENT:
                    continue;

                default:
                    printf("[CRITICAL] Unrecognized content type!\n");
                    return false;
            }

            if (xmlName == "")
                continue;

            if (simpleFS.HasFile(xmlName))
                printf("%s is present.\n", xmlName.c_str());
            else
            {
                printf("[WARNING] %s is missing!\n", xmlName.c_str());
                isComplete = false;
            }
        }

        if (isComplete)
            printf("NSP is valid.\n");
        else
            printf("NSP is installable, but incomplete.\n");

        return true;
    }
}