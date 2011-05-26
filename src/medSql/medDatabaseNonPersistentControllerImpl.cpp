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
#include "medDatabaseNonPersistentControllerImpl.h"
#include "medDatabaseNonPersistentReader.h"
#include "medDatabaseNonPersistentImporter.h"

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

class medDatabaseNonPersistentControllerImplPrivate
{
public:
    int pt_index;
    int st_index;
    int se_index;
    int im_index;
    QHash<medDataIndex, medDatabaseNonPersistentItem *> items;
};

// /////////////////////////////////////////////////////////////////
// medDatabaseNonPersitentController
// /////////////////////////////////////////////////////////////////

int medDatabaseNonPersistentControllerImpl::patientId(bool increment)
{
    if (increment)
        return d->pt_index++;
    else
        return d->pt_index;
}

int medDatabaseNonPersistentControllerImpl::studyId(bool increment)
{
    if (increment)
        return d->st_index++;
    else
        return d->st_index;
}

int medDatabaseNonPersistentControllerImpl::seriesId(bool increment)
{
    if (increment)
        return d->se_index++;
    else
        return d->se_index;
}

int medDatabaseNonPersistentControllerImpl::imageId(bool increment)
{
    if (increment)
        return d->im_index++;
    else
        return d->im_index;
}

QList<medDatabaseNonPersistentItem *> medDatabaseNonPersistentControllerImpl::items(void)
{
    return d->items.values();
}

void medDatabaseNonPersistentControllerImpl::insert(medDataIndex index, medDatabaseNonPersistentItem *item)
{
  d->items.insert(index, item);
}

medDataIndex medDatabaseNonPersistentControllerImpl::import(const QString& file)
{
    medDatabaseNonPersistentReader *reader = new medDatabaseNonPersistentReader(file);

    connect(reader, SIGNAL(progressed(int)), medMessageController::instance(), SLOT(setProgress(int)));
    connect(reader, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(reader, SIGNAL(success(QObject *)), reader, SLOT(deleteLater()));
    connect(reader, SIGNAL(failure(QObject *)), reader, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(reader, "Opening file item");

    medDataIndex index = reader->run();

    emit updated(index);

    return index;
}

QSharedPointer<dtkAbstractData> medDatabaseNonPersistentControllerImpl::read( const medDataIndex& index ) const
{
    QSharedPointer<dtkAbstractData> ret(d->items.value(index)->data());

    return ret;
}

int medDatabaseNonPersistentControllerImpl::nonPersistentDataStartingIndex(void)
{
    return 100000000;
}

medDatabaseNonPersistentControllerImpl::medDatabaseNonPersistentControllerImpl(void): d(new medDatabaseNonPersistentControllerImplPrivate)
{
    d->pt_index = nonPersistentDataStartingIndex();
    d->st_index = nonPersistentDataStartingIndex();
    d->se_index = nonPersistentDataStartingIndex();
    d->im_index = nonPersistentDataStartingIndex();
}

medDatabaseNonPersistentControllerImpl::~medDatabaseNonPersistentControllerImpl(void)
{
    foreach(medDatabaseNonPersistentItem *item, d->items)
        delete item;

    delete d;

    d = NULL;
}

bool medDatabaseNonPersistentControllerImpl::isConnected()
{
    // always connected as there is no database to control
    return true;
}

medDataIndex medDatabaseNonPersistentControllerImpl::import(dtkAbstractData *data)
{
    medDatabaseNonPersistentImporter *importer = new medDatabaseNonPersistentImporter(data);

    connect(importer, SIGNAL(progressed(int)),    medMessageController::instance(), SLOT(setProgress(int)));
    connect(importer, SIGNAL(success(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(importer, SIGNAL(failure(QObject *)), medMessageController::instance(), SLOT(remove(QObject *)));
    connect(importer, SIGNAL(success(QObject *)), importer, SLOT(deleteLater()));
    connect(importer, SIGNAL(failure(QObject *)), importer, SLOT(deleteLater()));

    medMessageController::instance()->showProgress(importer, "Importing data item");

    medDataIndex index = importer->run();

    emit updated(index);

    return index;
}

void medDatabaseNonPersistentControllerImpl::clear(void)
{
    // since we are not managing memory, no deletion should be made here
    // as we don't know if the data is still in use
    /*
    foreach (medDatabaseNonPersistentItem *item, d->items) {
      dtkAbstractData *data = item->data();
      if (data)
	  data->deleteLater();
    }
    */
    
    d->items.clear();
    d->pt_index = nonPersistentDataStartingIndex();
    d->st_index = nonPersistentDataStartingIndex();
    d->se_index = nonPersistentDataStartingIndex();
    d->im_index = nonPersistentDataStartingIndex();
}

qint64 medDatabaseNonPersistentControllerImpl::getEstimatedSize( const medDataIndex& index ) const
{
    Q_UNUSED(index);
    return 0;
}
