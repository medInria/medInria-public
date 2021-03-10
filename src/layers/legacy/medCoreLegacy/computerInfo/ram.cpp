#include "computerInfo.h"


namespace medComputerInfo
{
#ifdef WIN32
#include "windows.h"

    void ramInfo(long long &totalRam, long long &usageRam)
    {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);

        usageRam = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
        totalRam = memInfo.ullTotalPhys;
    }

#elif (defined(__APPLE__))

#include <sys/sysctl.h>
#include <mach/mach_types.h>
#include <mach/mach.h>

    void ramInfo(long long &totalRam, long long &usageRam)
    {
        int mib[] = { CTL_HW, HW_MEMSIZE };
        int64_t value = 0;
        size_t length = sizeof(value);

        if (-1 == sysctl(mib, 2, &value, &length, NULL, 0))
            return;

        totalRam = value;

        mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
        vm_statistics_data_t vmstat;
        if (KERN_SUCCESS != host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count))
            return;

        usageRam = (vmstat.wire_count + vmstat.active_count);
        usageRam *= 4096;
    }

#else
#include "sys/types.h"
#include "sys/sysinfo.h"
    void ramInfo(long long &totalRam, long long &usageRam)
    {
        struct sysinfo memInfo;


        sysinfo(&memInfo);
        totalRam = memInfo.totalram;
        usageRam = memInfo.totalram - memInfo.freeram;
    }

#endif
}