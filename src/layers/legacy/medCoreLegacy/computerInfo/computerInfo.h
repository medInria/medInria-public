#pragma once

#include <medCoreLegacyExport.h>
#include <stdio.h>

struct sockaddr;

namespace medComputerInfo
{
    /******************************************************
    *** CPU
    ******************************************************/
    int MEDCORELEGACY_EXPORT getNbOfCores();
    int MEDCORELEGACY_EXPORT getLoadAvg(double *loadavg);

    /******************************************************
    *** MEMORY
    ******************************************************/
    MEDCORELEGACY_EXPORT void ramInfo(long long &totalRam, long long &usageRam);

    /******************************************************
    *** MACHINE NAME  AND  NETWORK
    ******************************************************/
    MEDCORELEGACY_EXPORT bool           isMyComputer(char const* pi_pchBuffIn);
    MEDCORELEGACY_EXPORT bool           getIPFromName(char const* pi_pchHostName, char*** pio_pppchIPStrBuf);
    MEDCORELEGACY_EXPORT char*          getIpToStr(const struct sockaddr *pi_pSoket, size_t pi_iStrLen, char *pio_pchOut);
    MEDCORELEGACY_EXPORT unsigned char* isIPv4MappedOnIPv6(unsigned char* pi_puchIPv6);
}
