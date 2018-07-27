#include "install/install.hpp"

namespace tin::install
{
    IInstallTask::IInstallTask(FsStorageId destStorageId) :
        m_destStorageId(destStorageId)
    {}
}