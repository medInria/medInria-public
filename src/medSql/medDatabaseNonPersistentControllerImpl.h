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

public slots:

    /**
    * Read data from nonPersistent storage using the index
    * @params const medDataIndex & index Index to look for
    * @return dtkAbstractData* data
    */
    dtkAbstractData* read(const medDataIndex& index) const;

    /**
    * Store data temporarily referenced by temp index
    * @Note _NOT_IMPLEMENTED_YET
    * @params const dtkAbstractData & data data to be stored
    * @return medDataIndex assigned index
    */
    medDataIndex import(dtkAbstractData *data);


    /**
    * Store data temporarily referenced by temp index
    * @params const QString & file data stored at file path
    * @return medDataIndex - assigned index
    */
    medDataIndex import(const QString& file);


private:
    medDatabaseNonPersistentControllerImplPrivate *d;
};

#endif
