/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataManager.h>

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>

#include <medDbControllerFactory.h>
#include <medAbstractDbController.h>
#include <medMetaDataKeys.h>
#include <medMessageController.h>

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
        m_dbController = NULL;
        m_nonPersDbController = NULL;
    }

    // this is the data cache for persistent and non-persistent data
    typedef QHash<medDataIndex, dtkSmartPointer<medAbstractData> > DataCacheContainerType;
    DataCacheContainerType dataCache;
    DataCacheContainerType volatileDataCache;

    QHash<QString, medDataIndex > npDataIndexBeingSaved;

    medAbstractDbController* getDbController()
    {
        if (m_dbController == NULL)
        {
         m_dbController = medDbControllerFactory::instance()->createDbController("DbController");
         if (!m_dbController)
             qWarning() << "No dbController registered!";
        }
        return m_dbController;
    }

    medAbstractDbController* getNonPersDbController()
    {
        if (m_nonPersDbController == NULL)
        {
            m_nonPersDbController  = medDbControllerFactory::instance()->createDbController("NonPersistentDbController");
            if (!m_nonPersDbController)
                qWarning() << "No nonPersistentDbController registered!";
        }
        return m_nonPersDbController ;
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
    medAbstractDbController* m_dbController;
    medAbstractDbController* m_nonPersDbController;
    QList<medAbstractDbController*> m_controllerStack;
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

dtkSmartPointer<medAbstractData> medDataManager::data(const medDataIndex& index)
{
    dtkSmartPointer<medAbstractData>  medData;

    // try to get it from cache first
    if ( d->dataCache.contains(index) || d->volatileDataCache.contains(index) )
    {
        qDebug() << "Reading from cache";

        if (d->dataCache.contains(index))
            medData = d->dataCache.value(index);
        else
            medData = d->volatileDataCache.value(index);
    }
    else
    {

        qDebug() << "Reading from db";

        // try to load the data from db
        medAbstractDbController* db = d->getDbController();

        //checks the index is in the db before trying to read anything.
        //otherwise we get error messages when trying to open non persistent items.
        if (db  && db->contains(index))
        {
            // check available free memory and clean up if necessary
            if (!manageMemoryUsage(index, db))
            {
                qDebug() << "could not free enough memory, aborting data reading";
                return medData;
            }

            medData = db->read(index);
            if (medData.refCount() != 1)
                qWarning() << "RefCount should be 1: " << medData.refCount();
            if (!medData.isNull())
            {
                qDebug() << "adding data to data cachemap";
                d->dataCache[index] = medData;
            }
        }

        //if the data is still invalid we continue in the non-pers db
        if (!medData)
        {
            medAbstractDbController* npDb = d->getNonPersDbController();
            if(npDb && npDb->contains(index))
            {
                // check available free memory and clean up if necessary
                if (!manageMemoryUsage(index, npDb))
                {
                    qDebug() << "could not free enough memory, aborting data reading";
                    return medData;
                }

                medData = npDb->read(index);

                if (medData)
                {
                    qDebug() << "adding data to data non-pers.cachemap";
                    d->volatileDataCache[index] = medData;
                }
            }
        }

    }

    if (!medData)
    {
        qWarning() << "unable to open images with index:" << index.asString();

        // sometimes signals are blocked before calling this functions, like in medWorkspaceArea...
        //medWorkspaceArea doesn't block these signals any more... until we find why
        //it was needed, we won't be doing any black magic signal blocking where not needed.
        //        bool prevState = this->signalsBlocked();
        //        this->blockSignals(false);
        emit failedToOpen(index);
        //        this->blockSignals(prevState);
    } else {
        medData->setDataIndex(index);
    }

    return medData;
}

//-------------------------------------------------------------------------------------------------------

bool medDataManager::setMetaData( const medDataIndex& index, const QString& key, const QString& value )
{
    medAbstractDbController * dbc = controllerForDataSource( index.dataSourceId() );

    bool result =  dbc->setMetaData( index, key, value );
     
    if(result)
    {
        // it's not really a new data but we need the signal to update the view
        emit dataAdded (index);
    }
    
    return result;
}

//-------------------------------------------------------------------------------------------------------

medDataManager::medDataManager(void) : d(new medDataManagerPrivate)
{
    medAbstractDbController* db = d->getDbController();
    medAbstractDbController* npDb = d->getNonPersDbController();

    // Highly dependent on the current implementation of the controllers.
    connect(npDb, SIGNAL(updated(const medDataIndex &,QString)),
            this, SLOT(onNonPersistentDataImported(const medDataIndex &,QString)));
    connect(npDb, SIGNAL(updated(const medDataIndex &)),
            this, SIGNAL(dataRemoved(const medDataIndex &)));

    connect(db,SIGNAL(updated(const medDataIndex &, const QString &)),
            this, SLOT(onSingleNonPersistentDataStored(const medDataIndex &, const QString &)));
    connect(db,SIGNAL(updated(const medDataIndex &)),
            this, SLOT(onPersistentDatabaseUpdated(const medDataIndex &)));
    
    
    //TODO: Is it the best place to do that?
    setWriterPriorities();
    
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

    qDebug() << "****** TRY_FREE_MEM_BEGIN: " << procMem / divider << " to reach: " << memoryLimit / divider;

    int itemsBefore = d->dataCache.count();

    QMutexLocker locker(&d->mutex);

/*
    // iterate over the cache until we reach our threshold or all items are iterated
    foreach(medDataIndex index, d->dataCache.keys())
    {
        // remove reference to free it
        if (d->dataCache.find(index).value().refCount() == 1)
        {
        qDebug() << "object found with refCount 1, removing...";
        d->dataCache.remove(index);

        // check memory usage and stop the loop at the optimal threshold
        if (getProcessMemoryUsage() < memoryLimit)
            break;
    }
*/

    // clear cache
    foreach(medDataIndex index, d->dataCache.keys())
    {
        // remove reference to free it
        if (d->dataCache.find(index).value().refCount() == 1)
        {
            qDebug() << "object found with refCount 1, removing...";
            d->dataCache.remove(index);
        }
    }

    // clear cache
    foreach(medDataIndex index, d->dataCache.keys())
    {
        // remove reference to free it
        if (d->dataCache.find(index).value().refCount() == 0)
            d->dataCache.remove(index);
        else
            qDebug() << "cannot free object, ref count is:" << d->dataCache.find(index).value().refCount();
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
    quint64 processMem = getProcessMemoryUsage();
    quint64 estMem = controller->getEstimatedSize(index);
    quint64 requiredMem = processMem + estMem;
    quint64 optimalMem = getOptimalMemoryThreshold();
    quint64 maxMem = getUpperMemoryThreshold();

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

quint64 medDataManager::getUpperMemoryThreshold()
{
    if ( is32Bit() )
    {
        return 1500000000; //2 gb
    }
    else
    {
        // max virtual address space for 64bit varies on platforms (1TB for Windows)
        return 500000000000ULL;
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::setWriterPriorities()
{ 
    QList<QString> writers = medAbstractDataFactory::instance()->writers();
    QMap<int, QString> writerPriorites;
    
    int startIndex = 0;
    
    // set itkMetaDataImageWriter as the top priority writer
    if(writers.contains("itkMetaDataImageWriter"))
    {
        writerPriorites.insert(0, "itkMetaDataImageWriter");  
        startIndex = writers.removeOne("itkMetaDataImageWriter") ? 1 : 0;
    }
    
    for ( int i=0; i<writers.size(); i++ )
    {
        writerPriorites.insert(startIndex+i, writers[i]);   
    }
    
    medAbstractDataFactory::instance()->setWriterPriorities(writerPriorites);
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::importNonPersistent( medAbstractData *data )
{
    QString uuid = QUuid::createUuid().toString();
    this->importNonPersistent (data, uuid);
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::importNonPersistent(medAbstractData *data, QString uuid)
{
    if (!data)
        return;

    
    foreach (dtkSmartPointer<medAbstractData> medData, d->dataCache) {
        if (data == medData.data()) {
            qWarning() << "data already in manager, skipping";
            return;
        }
    }

    foreach (dtkSmartPointer<medAbstractData> medData, d->volatileDataCache) {
        if (data == medData.data()) {
            qWarning() << "data already in manager, skipping";
            return;
        }
    }

    medAbstractDbController* npDb = d->getNonPersDbController();

    if(npDb)
    {
        npDb->import(data, uuid);
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::onNonPersistentDataImported(const medDataIndex &index, QString uuid)
{   
    if (!index.isValid())
    {
        qWarning() << "Non Persistent Data Imported: Index is not valid";
        return;
    }

    medAbstractDbController* npDb = d->getNonPersDbController();
    dtkSmartPointer<medAbstractData> data = npDb->read(index);

    if (!data.isNull())
    {
        // d->volatileDataCache[index] = data is only done in data(), but we need it at this stage as well to be able to save data
        // in the persistent database, so i try to comment out the following line
        //if (d->volatileDataCache.contains (index))
            d->volatileDataCache[index] = data;

        emit dataAdded (index);
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::importNonPersistent(QString file)
{
    QString uuid = QUuid::createUuid().toString();
    this->importNonPersistent (file, uuid);
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::importNonPersistent( QString file, const QString &uuid )
{
    medAbstractDbController* npDb = d->getNonPersDbController();
    if(npDb)
    {

        npDb->import(file, uuid);
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::exportDataToFile(medAbstractData * data)
{
    if ( ! data) return;

    QList<QString> allWriters = medAbstractDataFactory::instance()->writers();
    QHash<QString, dtkAbstractDataWriter*> possibleWriters;

    foreach(QString writerType, allWriters) {
        dtkAbstractDataWriter * writer = medAbstractDataFactory::instance()->writer(writerType);
        if (writer->handled().contains(data->identifier()))
            possibleWriters[writerType] = writer;
        else
            delete writer;
    }

    if (possibleWriters.isEmpty()) {
        medMessageController::instance()->showError("Sorry, we have no exporter for this format.");
        return;
    }

    QFileDialog * exportDialog = new QFileDialog(0, tr("Exporting : please choose a file name and directory"));
    exportDialog->setOption(QFileDialog::DontUseNativeDialog);
    exportDialog->setAcceptMode(QFileDialog::AcceptSave);

    QComboBox* typesHandled = new QComboBox(exportDialog);
    // we use allWriters as the list of keys to make sure we traverse possibleWriters
    // in the order specified by the writers priorities.
    foreach(QString type, allWriters) {
        if ( ! possibleWriters.contains(type)) continue;
        QStringList extensionList = possibleWriters[type]->supportedFileExtensions();
        QString label = possibleWriters[type]->description() + " (" + extensionList.join(", ") + ")";
        QString extension = (extensionList.isEmpty()) ? QString() : extensionList.first();
        typesHandled->addItem(label, type);
        typesHandled->setItemData(typesHandled->count()-1, extension, Qt::UserRole+1);
        typesHandled->setItemData(typesHandled->count()-1, QVariant::fromValue<QObject*>(exportDialog), Qt::UserRole+2);
    }
    connect(typesHandled, SIGNAL(currentIndexChanged(int)), this, SLOT(exportDialog_updateSuffix(int)));

    QLayout* layout = exportDialog->layout();
    QGridLayout* gridbox = qobject_cast<QGridLayout*>(layout);

    // nasty hack to hide the filter list
    QWidget * filtersLabel = gridbox->itemAtPosition(gridbox->rowCount()-1, 0)->widget();
    QWidget * filtersList = gridbox->itemAtPosition(gridbox->rowCount()-1, 1)->widget();
    filtersLabel->hide(); filtersList->hide();

    if (gridbox) {
        gridbox->addWidget(new QLabel("Export format:", exportDialog), gridbox->rowCount()-1, 0);
        gridbox->addWidget(typesHandled, gridbox->rowCount()-1, 1);
    }

    exportDialog->setLayout(gridbox);

    // Set a default filename based on the series's description

    medAbstractDbController * dbController = controllerForDataSource(data->dataIndex().dataSourceId());
    if (dbController) {
        QString defaultName = dbController->metaData(data->dataIndex(), medMetaDataKeys::SeriesDescription);
        defaultName += typesHandled->itemData(typesHandled->currentIndex(), Qt::UserRole+1).toString();
        exportDialog->selectFile(defaultName);
    }

    if ( exportDialog->exec() ) {
        medAbstractDbController* db = d->getDbController();
        if(db) {
            QString chosenFormat = typesHandled->itemData(typesHandled->currentIndex()).toString();
            db->exportDataToFile(data, exportDialog->selectedFiles().first(), chosenFormat);
        }
    }

    qDeleteAll(possibleWriters);
    delete exportDialog;
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::exportDialog_updateSuffix(int index)
{
    QComboBox * typesHandled = qobject_cast<QComboBox*>(sender());
    if (! typesHandled) return;

    QFileDialog * exportDialog = qobject_cast<QFileDialog*>(typesHandled->itemData(index, Qt::UserRole+2).value<QObject*>());
    QString extension = typesHandled->itemData(index, Qt::UserRole+1).toString();

    QString currentFilename = exportDialog->selectedFiles().first();
    int lastDot = currentFilename.lastIndexOf('.');
    if (lastDot != -1) {
        currentFilename = currentFilename.mid(0, lastDot);
    }
    currentFilename += extension;
    exportDialog->selectFile(currentFilename);
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::storeNonPersistentDataToDatabase( void )
{
    foreach (medDataIndex index, d->volatileDataCache.keys()) {
        this->storeNonPersistentSingleDataToDatabase (index);
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::storeNonPersistentMultipleDataToDatabase( const medDataIndex &index )
{
    typedef medDataManagerPrivate::DataCacheContainerType DataHashMapType;
    typedef QList<medDataIndex> medDataIndexList;
    medDataIndexList indexesToStore;

    for (DataHashMapType::const_iterator it(d->volatileDataCache.begin()); it != d->volatileDataCache.end(); ++it ) {
        if (medDataIndex::isMatch( it.key(), index)) {
            indexesToStore.push_back(it.key());
        }
    }

    foreach(medDataIndex tmpIndex, indexesToStore)
        this->storeNonPersistentSingleDataToDatabase(tmpIndex);
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::storeNonPersistentSingleDataToDatabase( const medDataIndex &index )
{
    if (d->volatileDataCache.count(index) > 0)
    {
        qDebug() << "storing non persistent single data to database";
        dtkSmartPointer<medAbstractData> medData = d->volatileDataCache[index];
        QUuid tmpUid = QUuid::createUuid().toString();
        d->npDataIndexBeingSaved[tmpUid] = index;

        medAbstractDbController* db = d->getDbController();

        if(db)
            db->import(medData.data(),tmpUid);
    }
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::onSingleNonPersistentDataStored( const medDataIndex &index, const QString &uuid )
{
    qDebug() << "onSingleNonPersistentDataStored";
    
    medAbstractDbController* db = d->getDbController();
    medAbstractDbController* npDb = d->getNonPersDbController();

    if ((!db)||(!npDb))
        return;

    npDb->remove(d->npDataIndexBeingSaved[uuid]);
    d->volatileDataCache.remove(d->npDataIndexBeingSaved[uuid]);
    d->npDataIndexBeingSaved.remove(uuid);

    emit dataAdded(index);
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
    quint64 upperValue = getUpperMemoryThreshold();

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

void medDataManager::import( dtkSmartPointer<medAbstractData> &data )
{
    if (!data.data())
        return;

    medAbstractDbController* db = d->getDbController();

    if(db)
        db->import(data.data());
}

//-------------------------------------------------------------------------------------------------------

void medDataManager::import(const QString& file,bool indexWithoutCopying)
{
    qDebug() << "DEBUG : entering medDataManager::import(const QString& file,bool indexWithoutCopying)";

    if(file.isEmpty())
        return;

    medAbstractDbController* db = d->getDbController();

    if(db)
        db->import(file,indexWithoutCopying);
}

//-------------------------------------------------------------------------------------------------------


void medDataManager::onPersistentDatabaseUpdated(const medDataIndex &index)
{
    if (!index.isValid()) {
        qWarning() << "index is not valid";
        return;
    }

    medAbstractDbController* db = d->getDbController();

    // Test if index is in database (to distinguish between remove and add)
    if (db->contains(index))
    {
        emit dataAdded (index);
    }
    else
    {
        // Normally go here only if the data was actually removed from the database
        // Otherwise, reader should have sent an error
        emit dataRemoved (index);
    }

}

//-------------------------------------------------------------------------------------------------------

void medDataManager::removeData( const medDataIndex& index )
{
    // Remove from cache first
    this->removeDataFromCache(index);

    medAbstractDbController * dbc = controllerForDataSource( index.dataSourceId() );
    if (dbc)
    {
        dbc->remove(index);
    }
}

//-------------------------------------------------------------------------------------------------------

QList<medDataIndex> medDataManager::moveStudy(const medDataIndex& indexStudy, const medDataIndex& toPatient)
{
    medAbstractDbController *dbcSource = controllerForDataSource(indexStudy.dataSourceId());
    medAbstractDbController *dbcDest = controllerForDataSource(toPatient.dataSourceId());
    
    QList<medDataIndex> newIndexList;
    
    if(!dbcSource || !dbcDest)
    {
      qWarning() << "Incorrect controllers";
    }
    else if( dbcSource->isPersistent() && !dbcDest->isPersistent() )
    {
      qWarning() << "Move from persistent to non persistent controller not allowed";
    }
    else if( !dbcSource->isPersistent() && dbcDest->isPersistent() )
    {
      qWarning() << "Move from non persistent to persistent controller not allowed. Please save data first.";
      
      // Could be nice to have this feature
      // Would need to call storeNonPersistentSingleDataToDatabase(indexStudy);
      // and then to wait for correct signals since the command is asynchronuous (with QEventLoop),
      // retrieve inserted index and then realize the move      

    } 
    else
    {
      newIndexList = dbcSource->moveStudy(indexStudy,toPatient);
      
        if(!dbcSource->isPersistent())
        {
            foreach(medDataIndex newIndex, newIndexList)
                d->volatileDataCache[newIndex] = dbcSource->read(newIndex);
        }
    }
    
    return newIndexList;

}

//-------------------------------------------------------------------------------------------------------

medDataIndex medDataManager::moveSerie(const medDataIndex& indexSerie, const medDataIndex& toStudy)
{
    medAbstractDbController *dbcSource = controllerForDataSource(indexSerie.dataSourceId());
    medAbstractDbController *dbcDest = controllerForDataSource(toStudy.dataSourceId());
    
    medDataIndex newIndex;
    
    if(!dbcSource || !dbcDest)
    {
      qWarning() << "Incorrect controllers";
    }
    else if( dbcSource->isPersistent() && !dbcDest->isPersistent() )
    {
      qWarning() << "Move from persistent to non persistent controller not allowed";
    }
    else if( !dbcSource->isPersistent() && dbcDest->isPersistent() )
    {
      qWarning() << "Move from non persistent to persistent controller not allowed. Please save data first.";
      
      // Could be cool to have this feature
      // Would need to call storeNonPersistentSingleDataToDatabase(indexSerie);
      // and then to wait for correct signals since the command is asynchronuous (with QEventLoop),
      // retrieve inserted index et then realize the move      

    } 
    else
    {
      newIndex =  dbcSource->moveSerie(indexSerie,toStudy);
      
      if(!dbcSource->isPersistent())
        d->volatileDataCache[newIndex] = dbcSource->read(newIndex);
    }
    
    return newIndex;

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
    quint64 processMem = getProcessMemoryUsage();
    quint64 maxMem = getUpperMemoryThreshold();
    quint64 availableMem = (maxMem - processMem) / divider;

    if( requiredMemoryInKb == 0 )
        qDebug() << "Available memory:" << availableMem << "mb ";
    else
        qDebug() << "Available memory:" << availableMem << "mb "
        << "Required memory:" << (requiredMemoryInKb / divider) << "mb";

}

//-------------------------------------------------------------------------------------------------------

void medDataManager::clearCache()
{
    this->tryFreeMemory( 0 );
}


//-------------------------------------------------------------------------------------------------------


medDataManager *medDataManager::s_instance = NULL;
