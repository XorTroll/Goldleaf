#include "util/network_util.hpp"

namespace tin::network
{
    size_t WaitReceiveNetworkData(int sockfd, void* buf, size_t len)
    {
        size_t blockSizeRead = 0;
        size_t sizeRead = 0;

        while ((sizeRead += (blockSizeRead = recv(sockfd, (u8*)buf + sizeRead, len - sizeRead, 0)) < len) && (blockSizeRead > 0 || errno == EAGAIN) && !(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B))
        {
            errno = 0;
        }

        return sizeRead;
    }

    size_t WaitSendNetworkData(int sockfd, void* buf, size_t len)
    {
        size_t blockSizeWritten = 0;
        size_t sizeWritten = 0;

        while ((sizeWritten += (blockSizeWritten = send(sockfd, (u8*)buf + sizeWritten, len - sizeWritten, 0)) < len) && (blockSizeWritten > 0 || errno == EAGAIN) && !(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_B))
        {
            errno = 0;
        }

        return sizeWritten;
    }
}