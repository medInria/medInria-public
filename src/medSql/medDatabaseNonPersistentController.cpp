/* medDatabaseNonPersitentController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 17:58:04 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 16:30:50 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 99
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentController.h"
#include "medDatabaseNonPersistentReader.h"

#include <medCore/medDataIndex.h>
#include <medCore/medMessageController.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>

#include <QtCore/QHash>

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentControllerPrivate
// /////////////////////////////////////////////////////////////////

class medDatabaseNonPersistentControllerPrivate
{
public:
    static int pt_index;
    static int st_index;
    static int se_index;
    static int im_index;

    QHash<medDataIndex, medDatabaseNonPersistentItem *> items;
};

int medDatabaseNonPersistentControllerPrivate::pt_index = medDatabaseNonPersistentController::nonPersistentDataStartingIndex();
int medDatabaseNonPersistentControllerPrivate::st_index = 0;
int medDatabaseNonPersistentControllerPrivate::se_index = 0;
int medDatabaseNonPersistentControllerPrivate::im_index = 0;

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentController
// /////////////////////////////////////////////////////////////////

medDatabaseNonPersistentController *medDatabaseNonPersistentController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseNonPersistentController;

    return s_instance;
}

int& medDatabaseNonPersistentController::patientId(void)
{
    return medDatabaseNonPersistentControllerPrivate::pt_index;
}

int& medDatabaseNonPersistentController::studyId(void)
{
    return medDatabaseNonPersistentControllerPrivate::st_index;
}

int& medDatabaseNonPersistentController::seriesId(void)
{
    return medDatabaseNonPersistentControllerPrivate::se_index;
}

int& medDatabaseNonPersistentController::imageId(void)
{
    return medDatabaseNonPersistentControllerPrivate::im_index;
}

QList<medDatabaseNonPersistentItem *> medDatabaseNonPersistentController::items(void)
{
    return d->items.values();
}

void medDatabaseNonPersistentController::insert(medDataIndex index, medDatabaseNonPersistentItem *item)
{
    d->items.insert(index, item);
}

dtkAbstractData *medDatabaseNonPersistentController::data(const medDataIndex& index)
{
    if(d->items.keys().contains(index))
        return d->items.value(index)->data();

    return NULL;
}

medDataIndex medDatabaseNonPersistentController::read(const QString& file)
{
    medDatabaseNonPersistentReader *reader = new medDatabaseNonPersistentReader(file);

    connect(reader, SIGNAL(progressed(int)), medMessageController::instance(), SLOT(setProgress(int)));
    connect(reader, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(success(QObject *)), reader, SLOT(deleteLater()));
    connect(reader, SIGNAL(failure(QObject *)), reader, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(reader, "Opening file item");

    medDataIndex index = reader->run();

    emit updated();

    return index;
}

int medDatabaseNonPersistentController::nonPersistentDataStartingIndex(void)
{
    return 10000;
}

medDatabaseNonPersistentController::medDatabaseNonPersistentController(void) : QObject(), d(new medDatabaseNonPersistentControllerPrivate)
{

}

medDatabaseNonPersistentController::~medDatabaseNonPersistentController(void)
{
    foreach(medDatabaseNonPersistentItem *item, d->items)
        delete item;

    delete d;

    d = NULL;
}

medDatabaseNonPersistentController *medDatabaseNonPersistentController::s_instance = NULL;
