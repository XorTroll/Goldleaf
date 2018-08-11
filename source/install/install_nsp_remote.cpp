#include "install/install_nsp_remote.hpp"

namespace tin::install::nsp
{
    NetworkNSPInstallTask::NetworkNSPInstallTask(FsStorageId destStorageId, std::string url) :
        IInstallTask(destStorageId), m_url(url)
    {

    }

    void NetworkNSPInstallTask::PrepareForInstall()
    {
        
    }

    void NetworkNSPInstallTask::Install()
    {

    }
}