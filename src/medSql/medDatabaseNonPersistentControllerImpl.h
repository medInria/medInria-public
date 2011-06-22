/* medDatabaseNonPersitentController.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 17:45:07 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 16:07:23 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 58
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENONPERSISTENTCONTROLLERIMPL_H
#define MEDDATABASENONPERSISTENTCONTROLLERIMPL_H

#include <medCore/medAbstractDbController.h>
#include "medSqlExport.h"
#include <medCore/medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class dtkAbstractData;
class medDatabaseNonPersistentItem;
class medDatabaseNonPersistentControllerImplPrivate;

class MEDSQL_EXPORT medDatabaseNonPersistentControllerImpl: public medAbstractDbController
{
    Q_OBJECT

public:
    
    medDatabaseNonPersistentControllerImpl(void);
    ~medDatabaseNonPersistentControllerImpl(void);

    
    /**
    * Get the table indices 
    * @params bool increment if true auto-increment the counter
    * @return int return the id (before incrementation)
    */
    int patientId(bool increment=false);
    int   studyId(bool increment=false);
    int  seriesId(bool increment=false);
    int   imageId(bool increment=false);

    /**
    * Return offset needed to separate data indices between databases
    * @params void
    * @return int
    */
    int nonPersistentDataStartingIndex(void);

    /* proprietary method, should be avoided */
    QList<medDatabaseNonPersistentItem *> items(void);

    /* proprietary method, should be avoided */
    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    /**
    * Status of connection, should always be true
    * @return bool
    */
    bool isConnected();

    /**
    * return the size that the data behind the medDataIndex in byte
    * @param const medDataIndex& index the index for the data
    * @return estimated size of data
    */
    qint64 getEstimatedSize(const medDataIndex& index) const;

    /** List all the items in the DB */
    QList<medDataIndex> availableItems();

public slots:

    /**
    * Read data from nonPersistent storage using the index
    * @params const medDataIndex & index Index to look for
    * @return dtkAbstractData* data
    */
    dtkSmartPointer<dtkAbstractData> read(const medDataIndex& index) const;

    /**
    * Store data temporarily referenced by temp index
    * @params dtkAbstractData * data data to be stored
    * @return medDataIndex assigned index
    */
    medDataIndex import(dtkAbstractData *data);
    
    /**
     * Store data temporarily referenced by temp index
     * @params const QString & file data stored at file path
     * @return medDataIndex - assigned index
     */
    medDataIndex import(const QString& file);
    
    /**
     * Remove data referenced by index from temporary database
     * @params const medDataIndex & index : data index
     */
    void remove(const medDataIndex& index);
    
    /**
    * Removes any reference to non-persistent data. Do not actually free memory.
    */
    void clear (void);

    /** true if the given data index matches one in our db*/
    bool contains( const medDataIndex& index) const;

private:
    medDatabaseNonPersistentControllerImplPrivate *d;
};

#endif
