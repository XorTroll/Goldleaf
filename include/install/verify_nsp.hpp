#pragma once

#include <string>

namespace tin::install
{
    enum class VerificationErrorType
    {
        CRITICAL, WARNING
    };

    enum class VerificationLevel
    {
        ERROR_ALL, ERROR_CRITICAL_WARN, ERROR_CRITICAL_IGNORE_WARN, WARN_ALL
    };

    class NSPVerifier
    {
        private:
            std::string m_nspPath;

        public:
            NSPVerifier(std::string nspPath);

            bool PerformVerification();
    };
}