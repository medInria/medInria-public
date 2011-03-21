/* medDataManager.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 08:34:55 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:48:48 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDataManager.h"

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>

#include "medDbControllerFactory.h"
#include "medAbstractDbController.h"

#include <QtCore>

#if _MSC_VER || __MINGW32__
#include <windows.h>
#include <psapi.h>
#elif defined(__APPLE__)
#include <mach/task.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif 

class medDataManagerPrivate
{
public:

    medDataManagerPrivate()
    {
        dbController = NULL;
        nonPersDbController = NULL;
    }
    
    // this is the view cache
    QHash<medDataIndex, QSharedPointer<dtkAbstractData>> dataCache;
    // this is our temporary weakPtr cache
    QHash<medDataIndex, QWeakPointer<dtkAbstractData>> tempCache;

    medAbstractDbController* getDbController()
    {
        if (dbController == NULL)
        {
         dbController = medDbControllerFactory::instance()->createDbController("DbController");
         if (!dbController)
             qWarning() << "No dbController registered!";
        }
        return dbController;
    }

    medAbstractDbController* getNonPersDbController()
    {
        if (nonPersDbController == NULL)
        {
            nonPersDbController  = medDbControllerFactory::instance()->createDbController("NonPersistentDbController");
            if (!nonPersDbController)
                qWarning() << "No nonPersistentDbController registered!";
        }
        return nonPersDbController ;
    }

private:
    medAbstractDbController* dbController; 
    medAbstractDbController* nonPersDbController; 

};

medDataManager *medDataManager::instance(void)
{
    if(!s_instance)
        s_instance = new medDataManager;

    return s_instance;
}


QSharedPointer<dtkAbstractData> medDataManager::data(const medDataIndex& index)
{
    QSharedPointer<dtkAbstractData>  dtkdata;

    // try to get it from cache first
    if ( d->dataCache.contains(index) )
    {
        qDebug() << "Reading from cache";
        dtkdata = d->dataCache.value(index);
    }
    else
    {

        qDebug() << "Reading from db";

        // try to load the data from db
        medAbstractDbController* db = d->getDbController();
        if (db)
        {
            // check available free memory and clean up if necessary
            if (!manageMemoryUsage(index, db))
            {
                qDebug() << "could not free enough memory, aborting data reading"; 
                return dtkdata;
            }

            dtkdata = db->read(index);
        }

        //if the data is still invalid we continue in the non-pers db
        if (!dtkdata)
        {
            medAbstractDbController* npDb = d->getNonPersDbController();
            if(npDb)
            {
                // check available free memory and clean up if necessary
                if (!manageMemoryUsage(index, npDb))
                {
                    qDebug() << "could not free enough memory, aborting data reading"; 
                    return dtkdata;
                }

                dtkdata = npDb->read(index);
            }
        }

        qDebug() << "Memory after reading:" << getProcessMemoryUsage();

        // store it in the cache
        if (dtkdata)
        {
            d->dataCache[index] = dtkdata;
        }
    }

    if (!dtkdata)
    {
        qWarning() << "unable to open images with index:" << index.asString();
    }
    return dtkdata;

}

medDataManager::medDataManager(void) : d(new medDataManagerPrivate)
{

}

medDataManager::~medDataManager(void)
{
    tryFreeMemory();
    delete d;

    d = NULL;
}

void medDataManager::destroy( void )
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

void medDataManager::tryFreeMemory()
{
    qDebug() << "trying to free memory";

    // clear the temp cache
    d->tempCache.clear();
    int itemsBefore = d->dataCache.count();

    // iterate over the cache and copy strongRefs to weakRefs
    foreach(medDataIndex index, d->dataCache.keys())
    {
        d->tempCache.insert(index,d->dataCache.value(index).toWeakRef());
    }

    // clear our cache (this could be done iteratively)
    d->dataCache.clear();

    // restore the cache with items that survived
    foreach(medDataIndex index, d->tempCache.keys())
    {
        QWeakPointer<dtkAbstractData> weakPtr = d->tempCache.value(index);
        if (!weakPtr.isNull())
            d->dataCache.insert(index, d->tempCache.value(index).toStrongRef());
        else
            qDebug() << "Reference lost, freeing memory...";
    }

    int itemsNow = d->dataCache.count();
    if (itemsBefore != itemsNow)
        qDebug() << "Data-cache reduced from:" << itemsBefore << "to" << itemsNow << " items";
    else
        qDebug() << "Not possible to free any items";

}

bool medDataManager::manageMemoryUsage(const medDataIndex& index, medAbstractDbController* controller)
{
    bool res = true;
    qint64 processMem = getProcessMemoryUsage();
    qint64 dataMem = controller->getEstimatedSize(index);

    qDebug() << "Current memory usage:" << processMem;
    qDebug() << "Estimated memory need:" << processMem + dataMem;

    // check against our threshold
    if (getUpperMemoryThreshold() < (processMem + dataMem))
    {
        tryFreeMemory();
        qDebug() << "new memory usage after cleaning:" << getProcessMemoryUsage();

        // check again to see if we succeeded
        if (getUpperMemoryThreshold() < getProcessMemoryUsage() )
            res = false; //should be set to false, debugging only
    }
    
    return res;

}

size_t medDataManager::getProcessMemoryUsage()
{
#if _MSC_VER || __MINGW32__
    size_t size = 0;
    DWORD pid = GetCurrentProcessId();
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
    if ( hProcess == NULL )
        return 0;
    if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
    {
        size = pmc.WorkingSetSize;
    }
    CloseHandle( hProcess );
    return size;
#elif defined(__APPLE__)
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    task_info(current_task(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
    size_t size = t_info.virtual_size;
    return size;
#else
    int size, res, shared, text, sharedLibs, stack, dirtyPages;
    if ( ! ReadStatmFromProcFS( &size, &res, &shared, &text, &sharedLibs, &stack, &dirtyPages ) )
        return (size_t) size * getpagesize();
    else
        return 0;
#endif 
    return 0;
}

size_t medDataManager::getTotalSizeOfPhysicalRam()
{
#if _MSC_VER || __MINGW32__
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return (size_t) statex.ullTotalPhys;
#elif defined(__APPLE__)
    kern_return_t kr;
    host_basic_info_data_t hostinfo;
    int count = HOST_BASIC_INFO_COUNT;
    kr = host_info(mach_host_self(), HOST_BASIC_INFO, (host_info_t)&hostinfo, (mach_msg_type_number_t*)&count);
    if(kr == KERN_SUCCESS)
        return (size_t)hostinfo.memory_size;
    else
        return 0;
#else
    struct sysinfo info;
    if ( ! sysinfo( &info ) )
        return info.totalram * info.mem_unit;
    else
        return 0;
#endif 
}

int medDataManager::ReadStatmFromProcFS( int* size, int* res, int* shared, int* text, int* sharedLibs, int* stack, int* dirtyPages )
{
    int ret = 0;
    FILE* f;
    f = fopen( "/proc/self/statm", "r" );
    if( f ) {
        size_t ignored = fscanf( f, "%d %d %d %d %d %d %d", size, res, shared, text, sharedLibs, stack, dirtyPages );
        ++ignored;
        fclose( f );
    } else {
        ret = -1;
    }
    return ret;
}

size_t medDataManager::getUpperMemoryThreshold()
{
    return 1100000000; //1.1gb
}






medDataManager *medDataManager::s_instance = NULL;
