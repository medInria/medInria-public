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

#ifndef MEDDATABASENONPERSITENTCONTROLLER_H
#define MEDDATABASENONPERSITENTCONTROLLER_H

#include "medDatabaseNonPersitentItem.h"
#include "medSqlExport.h"

#include <medCore/medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class dtkAbstractData;

class medDatabaseNonPersitentControllerPrivate;

class medDatabaseNonPersitentController : public QObject
{
    Q_OBJECT

public:
    static medDatabaseNonPersitentController *instance(void);

    static int& patientId(void);
    static int&   studyId(void);
    static int&  seriesId(void);
    static int&   imageId(void);

    QList<medDatabaseNonPersitentItem *> items(void);

    void insert(medDataIndex index, medDatabaseNonPersitentItem *item);

    dtkAbstractData *data(const medDataIndex& index);

signals:
    void updated(void);

public slots:
    medDataIndex read(const QString& file);

public:
    static int nonPersitentDataStartingIndex(void);

protected:
     medDatabaseNonPersitentController(void);
    ~medDatabaseNonPersitentController(void);

private:
    static medDatabaseNonPersitentController *s_instance;

private:
    medDatabaseNonPersitentControllerPrivate *d;
};

#endif
