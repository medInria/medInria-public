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

const int divider = 1000000;

//-------------------------------------------------------------------------------------------------------

class medDataManagerPrivate
{
public:

    medDataManagerPrivate()
    {
        dbController = NULL;
        nonPersDbController = NULL;
    }
    
    // this is the data cache for persistent and non-persistent data
    typedef QHash<medDataIndex, dtkSmartPointer<dtkAbstractData> > DataCacheContainerType;
    DataCacheContainerType dataCache;
    DataCacheContainerType volatileDataCache;

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

    QList<medAbstractDbController*> controllerStack()
    {
        QList<medAbstractDbController*> stack;
        stack << getDbController();
        stack << getNonPersDbController();
        return stack;
    }

    static QMutex mutex;

private:
    medAbstractDbController* dbController; 
    medAbstractDbController* nonPersDbController; 
    QList<medAbstractDbController*> _controllerStack;
};

QMutex medDataManagerPrivate::mutex;

//-------------------------------------------------------------------------------------------------------

medDataManager *medDataManager::instance(void)
{
    if(!s_instance)
        s_instance = new medDataManager;

    return s_instance;
}

//-------------------------------------------------------------------------------------------------------

dtkSmartPointer<dtkAbstractData> medDataManager::data(const medDataIndex& index)
{
    dtkSmartPointer<dtkAbstractData>  dtkdata;

    // try to get it from cache first
    if ( d->dataCache.contains(index) || d->volatileDataCache.contains(index) )
    {
        qDebug() << "Reading from cache";

        if (d->dataCache.contains(index))
            dtkdata = d->dataCache.value(index);
        else
            dtkdata = d->volatileDataCache.value(index);
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

            if (dtkdata)
            {
                d->dataCache[index] = dtkdata;
            }
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

                if (dtkdata)
                {
                    d->volatileDataCache[index] = dtkdata;
                }
            }
        }

    }

    if (!dtkdata)
        qWarning() << "unable to open images with index:" << index.asString();

    return dtkdata;

}

//-------------------------------------------------------------------------------------------------------

medDataManager::medDataManager(void) : d(new medDataManagerPrivate)
{

}

//-------------------------------------------------------------------------------------------------------

medDataManager::~medDataManager(void)
{
    tryFreeMemory(getUpperMemoryThreshold());
    delete d;

    d = NULL;
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::destroy( void )
{
    if (s_instance) {
        delete s_instance;
        s_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------------------

bool medDataManager::tryFreeMemory(size_t memoryLimit)
{
    size_t procMem = getProcessMemoryUsage();
    if (procMem < memoryLimit)
        return false;

    int itemsBefore = d->dataCache.count();

    QMutexLocker locker(&d->mutex);

    // iterate over the cache until we reach our threshold or all items are iterated
    foreach(medDataIndex index, d->dataCache.keys())
    {
        // remove reference to free it
        d->dataCache.find(index).value() = NULL;

        // check memory usage and stop the loop at the optimal threshold
        if (getProcessMemoryUsage() < memoryLimit)
            break;
    }

    // clear cache
    foreach(medDataIndex index, d->dataCache.keys())
    {
        // remove reference to free it
        if (d->dataCache.find(index).value().isNull())
            d->dataCache.remove(index);
    }

    int itemsNow = d->dataCache.count();
    if (itemsBefore != itemsNow)
    {
        qDebug() << "Data-cache reduced from:" << itemsBefore << "to" << itemsNow << " items";
        return true;
    }
    else
    {
        qDebug() << "Not possible to free any items, current cache count is: " << itemsNow << "items";
        return false;
    }

}

//-------------------------------------------------------------------------------------------------------

bool medDataManager::manageMemoryUsage(const medDataIndex& index, medAbstractDbController* controller)
{
    bool res = true;
    qint64 processMem = getProcessMemoryUsage();
    qint64 estMem = controller->getEstimatedSize(index);
    qint64 requiredMem = processMem + estMem;
    qint64 optimalMem = getOptimalMemoryThreshold();
    qint64 maxMem = getUpperMemoryThreshold();

    printMemoryStatus(estMem);

    // check against our optimal threshold
    if (optimalMem < requiredMem)
    {
        bool freeingSuccessful;
        if (estMem > optimalMem)
            freeingSuccessful = tryFreeMemory(0); // purge all
        else
            freeingSuccessful = tryFreeMemory(optimalMem); // purge to optimal

        if (freeingSuccessful)
            printMemoryStatus();

        // check again to see if we succeeded
        if (maxMem < requiredMem)
        {
            res = false; 
            qWarning() << "Required memory usage (" << requiredMem / divider 
                << "mb) does not fit boundaries (" << maxMem / divider << " mb)";
        }
    }
    
    return res;

}

//-------------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------------------------------

int medDataManager::ReadStatmFromProcFS( int* size, int* res, int* shared, int* text, int* sharedLibs, int* stack, int* dirtyPages )
{
    int ret = 0;

#ifndef _MSC_VER

    FILE* f;
    f = fopen( "/proc/self/statm", "r" );
    if( f ) {
        size_t ignored = fscanf( f, "%d %d %d %d %d %d %d", size, res, shared, text, sharedLibs, stack, dirtyPages );
        ++ignored;
        fclose( f );
    } else {
        ret = -1;
    }

#endif // _MSC_VER

    return ret;
}

//-------------------------------------------------------------------------------------------------------

size_t medDataManager::getUpperMemoryThreshold()
{
    if ( is32Bit() )
    {
        return 1500000000; //2 gb
    }
    else
    {
        // max virtual address space for 64bit varies on platforms (1TB for Windows)
#ifndef _Wp64
        return 4200000000; // to avoid compiler warnings
#else
        return 500000000000;
#endif
    }
}

//-------------------------------------------------------------------------------------------------------

medDataIndex medDataManager::importNonPersistent( dtkAbstractData *data )
{
    if (!data)
        return medDataIndex();

    foreach (dtkSmartPointer<dtkAbstractData> dtkdata, d->dataCache) {
        if (data == dtkdata.data()) {
            qWarning() << "data already in manager, skipping";
            return medDataIndex();
        }
    }

    foreach (dtkSmartPointer<dtkAbstractData> dtkdata, d->volatileDataCache) {
        if (data == dtkdata.data()) {
            qWarning() << "data already in manager, skipping";
            return medDataIndex();
        }
    }

    medDataIndex index;

    medAbstractDbController* npDb = d->getNonPersDbController();
    if(npDb)
    {
        index = npDb->import(data);
    }

    if (!index.isValid()) {
        qWarning() << "index is not valid";
        return index;
    }

    if (d->volatileDataCache.contains (index)) {
        qWarning() << "index already in manager, skipping";
        return index;
    }

    d->volatileDataCache[index] = dtkSmartPointer<dtkAbstractData>(data);

    emit dataAdded (index);

    return index;
}

//-------------------------------------------------------------------------------------------------------

medDataIndex medDataManager::importNonPersistent( QString file )
{
    medDataIndex index;

    medAbstractDbController* npDb = d->getNonPersDbController();
    if(npDb)
    {
        index = npDb->import(file);
    }

    if (!index.isValid()) {
        qWarning() << "index is not valid";
        return index;
    }

    emit dataAdded (index);

    return index;

}


//-------------------------------------------------------------------------------------------------------

void medDataManager::storeNonPersistentDataToDatabase( void )
{
    foreach (dtkSmartPointer<dtkAbstractData> dtkdata, d->volatileDataCache) {
        this->import (dtkdata);
    }
    
    if (medAbstractDbController* npDb = d->getNonPersDbController())
        npDb->clear();
    
    d->volatileDataCache.clear();
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::storeNonPersistentSingleDataToDatabase( const medDataIndex &index )
{
    if (d->volatileDataCache.count(index) > 0)
    {
        dtkSmartPointer<dtkAbstractData> dtkdata = d->volatileDataCache[index];
        this->import (dtkdata);
        
        medAbstractDbController* npDb = d->getNonPersDbController();
        if (npDb)
            npDb->remove(index);
        
        d->volatileDataCache.remove(index);
    }
}

//-------------------------------------------------------------------------------------------------------

int medDataManager::nonPersistentDataCount( void ) const
{
    return d->volatileDataCache.count();
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::clearNonPersistentData( void )
{
    if (medAbstractDbController* npDb = d->getNonPersDbController())
        npDb->clear();

    d->volatileDataCache.clear();
}

//-------------------------------------------------------------------------------------------------------

bool medDataManager::is32Bit( void )
{
    if ( sizeof(void *) == 4 )
        return true;
    else
        return false;
}

//-------------------------------------------------------------------------------------------------------

size_t medDataManager::getOptimalMemoryThreshold()
{
    size_t optimalValue;
    size_t physicalValue = getTotalSizeOfPhysicalRam();
    size_t upperValue = getUpperMemoryThreshold();

    if ( is32Bit() )
    {
        // for 32bit we try to keep the cache half of the max memory
        // unless we have less physical memory available
        if ( physicalValue < (upperValue / 2) ) 
            optimalValue = physicalValue / 2;
        else
            optimalValue = upperValue / 2;
    }
    else
    {
       // for 64bit we use half of the physical memory
       // should be changed to use the available memory instead
       optimalValue = physicalValue / 2;
    }

    return optimalValue;
}

//-------------------------------------------------------------------------------------------------------

medDataIndex medDataManager::import( dtkSmartPointer<dtkAbstractData> &data )
{
    if (!data.data())
        return medDataIndex();

    medDataIndex index;

    medAbstractDbController* db = d->getDbController();
    if(db)
    {
        index = db->import(data.data());
    }

    if (!index.isValid()) {
        qWarning() << "index is not valid";
        return index;
    }

    d->dataCache[index] = data;

    emit dataAdded (index);

    return index;
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::removeData( const medDataIndex& index )
{

    emit dataRemoved( index );

    // Remove from cache first
    this->removeDataFromCache(index);

    qDebug() << "Reading from db";

    // try to load the data from db
    medAbstractDbController* db = d->getDbController();
    if (db)
    {
        db->remove(index);
    }

    medAbstractDbController* npDb = d->getNonPersDbController();
    if(npDb)
    {
        npDb->remove(index);
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::removeDataFromCache( const medDataIndex &index )
{
    typedef medDataManagerPrivate::DataCacheContainerType DataCacheContainerType;
    typedef QList<medDataIndex>    medDataIndexList;
    medDataIndexList indexesToRemove;

    // Clear the cache items
    for ( DataCacheContainerType::iterator it(d->dataCache.begin()); it != d->dataCache.end(); ++it) {
        if ( medDataIndex::isMatch( it.key(), index ) )
            indexesToRemove.push_back(it.key());
    }

    for ( medDataIndexList::iterator it( indexesToRemove.begin() ); it != indexesToRemove.end(); ++it ) {
        d->dataCache.remove( *it );
    }

    indexesToRemove.clear();

    // Clear the volatile cache items
    for ( DataCacheContainerType::iterator it(d->volatileDataCache.begin()); it != d->volatileDataCache.end(); ++it) {
        if ( medDataIndex::isMatch( it.key(), index ) )
            indexesToRemove.push_back(it.key());
    }

    for ( medDataIndexList::iterator it( indexesToRemove.begin() ); it != indexesToRemove.end(); ++it ) {
        d->volatileDataCache.remove( *it );
    }

}

//-------------------------------------------------------------------------------------------------------

medAbstractDbController * medDataManager::controllerForDataSource( int dataSource )
{
    foreach(medAbstractDbController* controller, d->controllerStack())
    {
        if(controller)
            if (controller->dataSourceId() == dataSource)
                return controller;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------------

const medAbstractDbController * medDataManager::controllerForDataSource( int dataSource ) const
{
    // Cast away constness to re-use same implementation.
    return const_cast<medDataManager*>(this)->controllerForDataSource(dataSource);
}

//-------------------------------------------------------------------------------------------------------

QList<int> medDataManager::dataSourceIds() const
{
    QList<int> sources;
    foreach(medAbstractDbController* controller, d->controllerStack())
    {
        if(controller)
            sources << controller->dataSourceId();
    }

    return sources;
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::printMemoryStatus(size_t requiredMemoryInKb)
{
    qint64 processMem = getProcessMemoryUsage();
    qint64 maxMem = getUpperMemoryThreshold();
    qint64 availableMem = (maxMem - processMem) / divider;
    if (availableMem < 0)
        availableMem = 0;

    if( requiredMemoryInKb == 0 )
        qDebug() << "Available memory:" << availableMem << "mb ";
    else
        qDebug() << "Available memory:" << availableMem << "mb " 
        << "Required memory:" << (requiredMemoryInKb / divider) << "mb";

}

//-------------------------------------------------------------------------------------------------------


medDataManager *medDataManager::s_instance = NULL;
