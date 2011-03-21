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
      * Ask the data-manager to provide the data belonging to this index using it's registered controllers
      * @params const medDataIndex & index medDataIndex for data
      * @return dtkAbstractData * the data
      */
      QSharedPointer<dtkAbstractData> data(const medDataIndex& index);

      /**
      * Use this function to insert data into the database, 
      * Do *not* use the concrete database controller implementation for it
      * The data-manager will take over this task
      * @params const dtkAbstractData & data
      * @return medDataIndex*
      */
      medDataIndex* import(const dtkAbstractData& data);



protected:
     medDataManager(void);
    ~medDataManager(void);

      /**
      * Releases all own references to let all stored smartpointers get out of scope
      * All remaining references will be restored (probably not threadsafe)
      * @return void
      */
      void tryFreeMemory();

      /** 
      * Returns the memory usage of the current process in bytes.
      * On linux, this refers to the virtual memory allocated by 
      * the process (the VIRT column in top).
      * On windows, this refery to the size in bytes of the working 
      * set pages (the "Memory" column in the task manager).
      * Code taken from mitk (bsd)
      */
      size_t getProcessMemoryUsage();

      /**
      * Returns the total size of phyiscal memory in bytes
      */
      size_t getTotalSizeOfPhysicalRam();

      /**
      * Compares the process memory usage with the upper threshold, frees memory to reach lower threshold
      * @return bool success or failure
      */
      bool manageMemoryUsage(const medDataIndex& index, medAbstractDbController* controller);

      /**
       * Helper for linux
       */
      int ReadStatmFromProcFS( int* size, int* res, int* shared, int* text, int* sharedLibs, int* stack, int* dirtyPages );

      size_t getUpperMemoryThreshold();

protected:
    static medDataManager *s_instance;

private:
    medDataManagerPrivate *d;
};

#endif // MEDDATAMANAGER_H
