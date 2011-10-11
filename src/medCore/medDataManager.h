/* medDataManager.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 08:34:45 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:25:19 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDDATAMANAGER_H
#define MEDDATAMANAGER_H

#include <QtCore/QObject>

#include <dtkCore/dtkSmartPointer.h>

#include "medCoreExport.h"
#include "medDataIndex.h"

class dtkAbstractData;
class dtkAbstractDataFactory;

class medDataManagerPrivate;
class medAbstractDbController;

/**
 * This class is the global access point to data stored in the database.
 * It tries to use several database-controllers to provide/store data
 * Another role is to cache data to provide faster access (work in progress)
 */
class MEDCORE_EXPORT medDataManager : public QObject
{
    Q_OBJECT

public:
      static medDataManager *instance(void);
      static void destroy(void);

    /**
    * Ask the data-manager to provide the data belonging to this index using its registered controllers.
    * @params const medDataIndex & index medDataIndex for data
    * @return dtkAbstractData * the data
    */
    dtkSmartPointer<dtkAbstractData> data(const medDataIndex& index);

    /**
    * Use this function to insert data into the database,
    * Do *not* use the concrete database controller implementation for it
    * The data-manager will take over this task
    * @params dtkSmartPointer<dtkAbstractData> & data
    */
    void import(dtkSmartPointer<dtkAbstractData> &data);

    /**
    * Use this function to insert data into the non-persistent database,
    * Do *not* use the concrete database controller implementation for it
    * The data-manager will take over this task
    * @params const dtkAbstractData & data
    */
    void importNonPersistent(dtkAbstractData *data);
    void importNonPersistent(dtkAbstractData *data, QString uuid);


    /**
    * Overload to insert data directly from a file into the no-persistent database
    * @params QString file
    */
    void importNonPersistent(QString file);
    void importNonPersistent(QString file, const QString &uuid);


    /**
    * Use this function to save all non-persistent data to the sql database.
    * The list of non-persistent data will then be cleared, and any subsequent
    * access to those data will trigger a reading from the database.
    */
    void storeNonPersistentDataToDatabase (void);


    void storeNonPersitentMultipleDataToDatabase( const medDataIndex &index );

    /**
     * Use this function to save one non-persistent data to the sql database.
     * The data is specified by ots medDataIndex , it is then removed from the non persistent database
     */
    void storeNonPersistentSingleDataToDatabase( const medDataIndex &index );

    /**
    * Returns the number of non-persistent data contained in the data manager
    */
    int nonPersistentDataCount (void) const;

    /**
    * Clear the list of non-persistent data
    */
    void clearNonPersistentData (void);


    /** Remove an item or items from the database
     *  Will remove a whole patient / study / series depending on the index.
     */
    void removeData(const medDataIndex& index);

    /** return the DB controller for given data source. */
    medAbstractDbController *controllerForDataSource( int dataSource );
    const medAbstractDbController *controllerForDataSource( int dataSource ) const;

    /** Return a list of available dataSource Ids.*/
    QList<int> dataSourceIds() const;

    /**
     * Check if the program was compiled using 32bit compiler
     */
    static bool is32Bit(void);

    /**
    * Returns the memory usage of the current process in bytes.
    * On linux, this refers to the virtual memory allocated by
    * the process (the VIRT column in top).
    * On windows, this refers to the size in bytes of the working
    * set pages (the "Memory" column in the task manager).
    * Code taken from mitk (bsd)
    */
    static size_t getProcessMemoryUsage();

    /**
    * Returns the total size of physical memory in bytes
    */
    static size_t getTotalSizeOfPhysicalRam();

    /**
    * Return the hard limit the process can allocate
    * Result depends on the platform
    * If this threshold is crossed the manager will not
    * allocate memory to ensure system stability
    */
    static size_t getUpperMemoryThreshold();

    /**
    * Return the memory limit where the system should try to stay
    * This ensures optimal memory usage to avoid paging
    */
    static size_t getOptimalMemoryThreshold();

    /**
     * Clear all items stored in the data manager
     */
    void clearCache();

signals:
    /**
    * This signal is emitted whenever a data was added in either the persistent
    * or non persistent database by calling import() or importNonPersistentData().
    */
    void dataAdded (const medDataIndex&);

    /**
    * This signal is emitted whenever a data is removed in either the persistent
    * or non persistent database by calling remove().
    */
    void dataRemoved (const medDataIndex&);

    /**
     * @brief Emitted when an image fails to open
     * @param the @medDataIndex of the image
    */
    void failedToOpen(const medDataIndex&);

    /**
     * @brief Emitted when an image fails to import
     * @param index the @medDataIndex of the image
     * @param uuid the identifier linked to this import request
    */
    void importFailed(const medDataIndex& index, QString uuid);

public slots:
    void onNonPersistentDataImported(const medDataIndex &index, QString uuid);
    void onPersistentDataImported(const medDataIndex &index);
    void onSingleNonPersistentDataStored(const medDataIndex &index, const QString &uuid);

protected:
     medDataManager(void);
    ~medDataManager(void);


    /**
    * Compares the process memory usage with the upper threshold, frees memory to reach lower threshold
    * @return bool success or failure
    */
    bool manageMemoryUsage(const medDataIndex& index, medAbstractDbController* controller);

    /**
    * Helper for linux
    */
    static int ReadStatmFromProcFS( int* size, int* res, int* shared, int* text, int* sharedLibs, int* stack, int* dirtyPages );

    /** Remove all matching items from the cache. */
    void removeDataFromCache( const medDataIndex &index);

    /**
     * Print available memory
     */
    void printMemoryStatus(size_t requiredMemoryInKb = 0);

        /**
    * Releases all own references to let all stored smartpointers get out of scope
    * All remaining references will be restored (probably not thread safe)
    * @return void
    */
    bool tryFreeMemory(size_t memoryLimit);


protected:
    static medDataManager *s_instance;

private:
    medDataManagerPrivate *d;
};

#endif // MEDDATAMANAGER_H
