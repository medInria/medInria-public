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

    int patientId(bool increment=false);
    int   studyId(bool increment=false);
    int  seriesId(bool increment=false);
    int   imageId(bool increment=false);

    int nonPersistentDataStartingIndex(void);

    QList<medDatabaseNonPersistentItem *> items(void);

    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    bool isConnected();

public slots:
    medDataIndex read(const QString& file);

    /**
     * get the data out of the virtual database (cache)
     */
    dtkAbstractData* data(const medDataIndex& index) const;

    /**
     * satisfies base class, just redirects to data function
     */
    dtkAbstractData* read(const medDataIndex& index) const;

    /**
     * Read and store data temporarily referenced by temp index
     */
    medDataIndex import(const QString& file);

    /**
     * Store data temporarily referenced by temp index
     */
    medDataIndex import(const dtkAbstractData& data);

private:
    medDatabaseNonPersistentControllerImplPrivate *d;
};

#endif
