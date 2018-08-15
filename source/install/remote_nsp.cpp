#include "install/remote_nsp.hpp"

#include "error.hpp"
#include "debug.h"

namespace tin::install::nsp
{
    RemoteNSP::RemoteNSP(std::string url) :
        m_download(url)
    {

    }

    // TODO: Do verification: PFS0 magic, sizes not zero
    void RemoteNSP::RetrieveHeader()
    {
        printf("Retrieving remote NSP header...\n");

        // Retrieve the base header
        m_headerBytes.resize(sizeof(PFS0BaseHeader), 0);
        m_download.RequestDataRange(m_headerBytes.data(), 0x0, sizeof(PFS0BaseHeader));

        LOG_DEBUG("Base header: \n");
        printBytes(nxlinkout, m_headerBytes.data(), sizeof(PFS0BaseHeader), true);

        // Retrieve the full header
        size_t remainingHeaderSize = this->GetBaseHeader()->numFiles * sizeof(PFS0FileEntry) + this->GetBaseHeader()->stringTableSize;
        m_headerBytes.resize(sizeof(PFS0BaseHeader) + remainingHeaderSize, 0);
        m_download.RequestDataRange(m_headerBytes.data() + sizeof(PFS0BaseHeader), sizeof(PFS0BaseHeader), remainingHeaderSize);

        LOG_DEBUG("Full header: \n");
        printBytes(nxlinkout, m_headerBytes.data(), m_headerBytes.size(), true);
    }

    void RemoteNSP::RetrieveAndProcessNCA(NcmNcaId ncaId, std::function<void (void* blockBuf, size_t bufSize, size_t blockStartOffset, size_t ncaSize)> processBlockFunc)
    {

    }

    const PFS0BaseHeader* RemoteNSP::GetBaseHeader()
    {
        return reinterpret_cast<PFS0BaseHeader*>(m_headerBytes.data());
    }
}