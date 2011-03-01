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

    dtkAbstractData* data(const medDataIndex& index);

    dtkAbstractData* read(const medDataIndex& index);

signals:
    void updated(void);

public slots:
    medDataIndex read(const QString& file);

private:
    medDatabaseNonPersistentControllerImplPrivate *d;
};

#endif
