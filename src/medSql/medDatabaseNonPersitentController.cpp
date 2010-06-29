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

#include "medDatabaseNonPersitentItem.h"
#include "medDatabaseNonPersitentItem_p.h"
#include "medDatabaseNonPersitentController.h"
#include "medDatabaseNonPersitentReader.h"

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

class medDatabaseNonPersitentControllerPrivate
{
public:
    static int pt_index;
    static int st_index;
    static int se_index;
    static int im_index;

    QHash<medDataIndex, medDatabaseNonPersitentItem *> items;
};

int medDatabaseNonPersitentControllerPrivate::pt_index = medDatabaseNonPersitentController::nonPersitentDataStartingIndex();
int medDatabaseNonPersitentControllerPrivate::st_index = 0;
int medDatabaseNonPersitentControllerPrivate::se_index = 0;
int medDatabaseNonPersitentControllerPrivate::im_index = 0;

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentController
// /////////////////////////////////////////////////////////////////

medDatabaseNonPersitentController *medDatabaseNonPersitentController::instance(void)
{
    if(!s_instance)
        s_instance = new medDatabaseNonPersitentController;

    return s_instance;
}

int& medDatabaseNonPersitentController::patientId(void)
{
    return medDatabaseNonPersitentControllerPrivate::pt_index;
}

int& medDatabaseNonPersitentController::studyId(void)
{
    return medDatabaseNonPersitentControllerPrivate::st_index;
}

int& medDatabaseNonPersitentController::seriesId(void)
{
    return medDatabaseNonPersitentControllerPrivate::se_index;
}

int& medDatabaseNonPersitentController::imageId(void)
{
    return medDatabaseNonPersitentControllerPrivate::im_index;
}

QList<medDatabaseNonPersitentItem *> medDatabaseNonPersitentController::items(void)
{
    return d->items.values();
}

void medDatabaseNonPersitentController::insert(medDataIndex index, medDatabaseNonPersitentItem *item)
{
    d->items.insert(index, item);
}

dtkAbstractData *medDatabaseNonPersitentController::data(const medDataIndex& index)
{
    if(d->items.keys().contains(index))
        return d->items.value(index)->data();

    return NULL;
}

medDataIndex medDatabaseNonPersitentController::read(const QString& file)
{
    medDatabaseNonPersitentReader *reader = new medDatabaseNonPersitentReader(file);

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

int medDatabaseNonPersitentController::nonPersitentDataStartingIndex(void)
{
    return 10000;
}

medDatabaseNonPersitentController::medDatabaseNonPersitentController(void) : QObject(), d(new medDatabaseNonPersitentControllerPrivate)
{

}

medDatabaseNonPersitentController::~medDatabaseNonPersitentController(void)
{
    foreach(medDatabaseNonPersitentItem *item, d->items)
        delete item;

    delete d;

    d = NULL;
}

medDatabaseNonPersitentController *medDatabaseNonPersitentController::s_instance = NULL;
