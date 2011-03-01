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

#ifndef MEDDATABASENONPERSISTENTCONTROLLER_H
#define MEDDATABASENONPERSISTENTCONTROLLER_H

#include "medDatabaseNonPersistentItem.h"
#include "medSqlExport.h"

#include <medCore/medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class dtkAbstractData;

class medDatabaseNonPersistentControllerPrivate;

class MEDSQL_EXPORT medDatabaseNonPersistentController : public QObject
{
    Q_OBJECT

public:
    static medDatabaseNonPersistentController *instance(void);

    static int& patientId(void);
    static int&   studyId(void);
    static int&  seriesId(void);
    static int&   imageId(void);

    QList<medDatabaseNonPersistentItem *> items(void);

    void insert(medDataIndex index, medDatabaseNonPersistentItem *item);

    dtkAbstractData *data(const medDataIndex& index);

signals:
    void updated(void);

public slots:
    medDataIndex read(const QString& file);

public:
    static int nonPersistentDataStartingIndex(void);

protected:
     medDatabaseNonPersistentController(void);
    ~medDatabaseNonPersistentController(void);

private:
    static medDatabaseNonPersistentController *s_instance;

private:
    medDatabaseNonPersistentControllerPrivate *d;
};

#endif
