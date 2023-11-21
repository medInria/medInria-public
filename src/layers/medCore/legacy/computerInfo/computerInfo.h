#pragma once

#include <medCoreExport.h>
#include <stdio.h>

struct sockaddr;

namespace medComputerInfo
{
    /******************************************************
    *** CPU
    ******************************************************/
    int MEDCORE_EXPORT getNbOfCores();
    int MEDCORE_EXPORT getLoadAvg(double *loadavg);

    /******************************************************
    *** MEMORY
    ******************************************************/
    MEDCORE_EXPORT void ramInfo(long long &totalRam, long long &usageRam);

    /******************************************************
    *** MACHINE NAME  AND  NETWORK
    ******************************************************/
    MEDCORE_EXPORT bool           isMyComputer(char const* pi_pchBuffIn);
    MEDCORE_EXPORT bool           getIPFromName(char const* pi_pchHostName, char*** pio_pppchIPStrBuf);
    MEDCORE_EXPORT char*          getIpToStr(const struct sockaddr *pi_pSoket, size_t pi_iStrLen, char *pio_pchOut);
    MEDCORE_EXPORT unsigned char* isIPv4MappedOnIPv6(unsigned char* pi_puchIPv6);
}
