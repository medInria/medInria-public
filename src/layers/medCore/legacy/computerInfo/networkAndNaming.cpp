#include <cstring>
#include <ctype.h>
#include "computerInfo.h"

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <in6addr.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#ifndef IN6_IS_ADDR_V4MAPPED
#define IN6_IS_ADDR_V4MAPPED(a) \
   ((((a)->s6_words[0]) == 0) && \
   (((a)->s6_words[1]) == 0) && \
   (((a)->s6_words[2]) == 0) && \
   (((a)->s6_words[3]) == 0) && \
   (((a)->s6_words[4]) == 0) && \
   (((a)->s6_words[5]) == 0xFFFF))
#endif



namespace medComputerInfo
{
    unsigned char* isIPv4MappedOnIPv6(unsigned char* pi_puchIPv6)
    {
        unsigned char* puchRes = NULL;


#ifdef IN6_IS_ADDR_V4MAPPED
        puchRes = IN6_IS_ADDR_V4MAPPED((in6_addr*)pi_puchIPv6) ? pi_puchIPv6 + 12 : NULL;
#else
        bool bMapped = true;
        for (int i = 0; i < 10; ++i)
        {
            bMapped &= pi_puchIPv6[i] == 0;
        }
        for (int i = 10; i < 12; ++i)
        {
            bMapped &= pi_puchIPv6[i] == 255;
        }
        if (bMapped)
        {
            puchRes = pi_puchIPv6 + 12;
        }
#endif // IN6_IS_ADDR_V4MAPPED

        return puchRes;
    }

    char* getIpToStr(const struct sockaddr *pi_pSoket, size_t pi_iStrLen, char *pio_pchOut)
    {
        switch (pi_pSoket->sa_family)
        {
            case AF_INET:
            {
                inet_ntop(AF_INET, &(((struct sockaddr_in *)pi_pSoket)->sin_addr),
                    pio_pchOut, pi_iStrLen);
                break;
            }
            case AF_INET6:
            {
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)pi_pSoket)->sin6_addr),
                    pio_pchOut, pi_iStrLen);
                break;
            }
            default:
            {
#ifdef WIN32
                strncpy_s(pio_pchOut, pi_iStrLen, "Unknown AF", sizeof("Unknown AF"));
#else
                std::strncpy(pio_pchOut, "Unknown AF", pi_iStrLen);
#endif
                return NULL;
            }
        }

        return pio_pchOut;
    }

    bool getIPFromName(char const* pi_pchHostName, char*** pio_pppchIPStrBuf)
    {
        bool bRes = false;
        if (pio_pppchIPStrBuf)
        {
            int iRes = 0;
            int i = 0;
            int iNbIPv4 = 0;
            char** &ppchIpStrBuff = *pio_pppchIPStrBuf;
            char*  ppchTempBuff[128];
            bool   pbIsIPv4[128];

            struct addrinfo *pstAddrInfoRes = NULL;
            struct addrinfo *ptr = NULL;
            struct addrinfo hints;

            memset(ppchTempBuff, 0, sizeof(char*) * 128);
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            iRes = getaddrinfo(pi_pchHostName, NULL, &hints, &pstAddrInfoRes);
            if (iRes == 0)
            {
                ptr = pstAddrInfoRes;
                while (ptr && i < 128)
                {
                    if (ptr->ai_family == AF_INET || ptr->ai_family == AF_INET6)
                    {
                        ppchTempBuff[i] = (char*)malloc(64 * sizeof(char));
                        getIpToStr(ptr->ai_addr, 46, ppchTempBuff[i]);
                        bRes = ppchTempBuff[i][0] != 0;
                        pbIsIPv4[i] = ptr->ai_family == AF_INET;
                        if (bRes && pbIsIPv4[i])
                        {
                            iNbIPv4++;
                        }
                        i++;
                    }
                    ptr = ptr->ai_next;
                }
                if (bRes)
                {
                    ppchIpStrBuff = (char**)malloc((i + 1) * sizeof(char*));
                    ppchIpStrBuff[i] = NULL;
                    int k = 0, l = 0;
                    for (int j = 0; j < i; j++)
                    {
                        if (pbIsIPv4[j])
                        {
                            ppchIpStrBuff[k] = ppchTempBuff[j];
                            k++;
                        }
                        else
                        {
                            ppchIpStrBuff[iNbIPv4 + l] = ppchTempBuff[j];
                            l++;
                        }
                    }
                }
                else
                {
                    for (int j = 0; j < i; j++)
                    {
                        if (ppchTempBuff[j])
                        {
                            free(ppchTempBuff[j]);
                            ppchTempBuff[j] = NULL;
                        }
                    }
                }
            }
            else
            {
                //not found
            }
            freeaddrinfo(pstAddrInfoRes);
        }

        return bRes;
    }

    bool isMyComputer(char const* pi_pchBuffIn)
    {
        bool bRes = false;


        bool bV6 = false;
        bool bV4 = true;
        bool bError = true;
        struct sockaddr_in  saGNI4;
        struct sockaddr_in6 saGNI6;
        char   hostname[NI_MAXHOST];
        char   hostnameRef[NI_MAXHOST];
        u_short port = 0;
        struct sockaddr * pSocket = NULL;
        int iRetval = 0;
        int iBuffLen = static_cast<int>(strlen(pi_pchBuffIn));
        int iSoketSize = 0;
        int i = 0;

        memset(hostname, 0, NI_MAXHOST);
        memset(hostnameRef, 0, NI_MAXHOST);
        memset(&saGNI4, 0, sizeof(saGNI4));
        memset(&saGNI6, 0, sizeof(saGNI6));

        while (!bV6 && i < iBuffLen)
        {
            bV6 |= pi_pchBuffIn[i] == ':';
            bV4 &= (pi_pchBuffIn[i] >= '0' && pi_pchBuffIn[i] <= '9') || pi_pchBuffIn[i] == '.';
            i++;
        }

        if (bV6 || bV4)
        {
            bError = false;
            if (bV6)
            {
                saGNI6.sin6_family = AF_INET6;
                if (inet_pton(AF_INET6, pi_pchBuffIn, &saGNI6.sin6_addr) == 1)
                {
                    unsigned char* puchMapped = isIPv4MappedOnIPv6((unsigned char*)&saGNI6.sin6_addr);
                    if (puchMapped)
                    {
                        memcpy(&saGNI4.sin_addr.s_addr, puchMapped, 4);
                        saGNI4.sin_family = AF_INET;
                        saGNI4.sin_port = htons(port);
                        iSoketSize = sizeof(saGNI4);
                        pSocket = (struct sockaddr *)&saGNI4;
                    }
                    else
                    {
                        saGNI6.sin6_family = AF_INET6;
                        saGNI6.sin6_port = htons(port);
                        iSoketSize = sizeof(saGNI6);
                        pSocket = (struct sockaddr *)&saGNI6;
                    }
                }
                else
                {
                    bError = true;
                }
            }
            else // is IPv4
            {
                if (inet_pton(AF_INET, pi_pchBuffIn, &saGNI4.sin_addr) == 1)
                {
                    saGNI4.sin_family = AF_INET;
                    saGNI4.sin_port = htons(port);
                    iSoketSize = sizeof(saGNI4);
                    pSocket = (struct sockaddr *)&saGNI4;

                }
                else
                {
                    bError = true;
                }
            }

            iRetval = getnameinfo(pSocket, iSoketSize, hostname, NI_MAXHOST, /*servInfo*/NULL, /*NI_MAXSERV*/0, NI_NAMEREQD | NI_NOFQDN);
        }

        if (!bError)
        {
            gethostname(hostnameRef, NI_MAXHOST - 1);

            i = 0;
            while (hostnameRef[i])
            {
                hostnameRef[i] = tolower(hostnameRef[i]);
                i++;
            }

            i = 0;
            while (hostname[i])
            {
                hostname[i] = tolower(hostname[i]);
                i++;
            }

            bRes = strcmp(hostname, hostnameRef) == 0;
            if (!bRes)
            {
                bRes = strcmp(hostname, "localhost") == 0;
            }
        }
        else
        {
            char** ppchBuffIpOut = NULL;
            if (getIPFromName(pi_pchBuffIn, &ppchBuffIpOut))
            {
                if (ppchBuffIpOut[0])
                {
                    bRes = false;
                    int i = 0;
                    while (!bRes && ppchBuffIpOut[i])
                    {
                        bRes = isMyComputer(ppchBuffIpOut[i]);
                        free(ppchBuffIpOut[i]);
                        i++;
                    }
                    while (ppchBuffIpOut[i])
                    {
                        if (ppchBuffIpOut[i])
                        {
                            free(ppchBuffIpOut[i]);
                            i++;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    bRes = false;
                }
                free(ppchBuffIpOut);
            }
        }

        return bRes;
    }
}

