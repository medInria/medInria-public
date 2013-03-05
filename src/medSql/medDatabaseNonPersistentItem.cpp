/* medDatabaseNonPersitentItem.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 18:42:08 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:54:44 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"

medDatabaseNonPersistentItem::medDatabaseNonPersistentItem() : QObject(), d(new medDatabaseNonPersistentItemPrivate)
{
    //d->data = NULL;
}

medDatabaseNonPersistentItem::~medDatabaseNonPersistentItem()
{
    delete d;

    d = NULL;
}

const QString& medDatabaseNonPersistentItem::name() const
{
    return d->name;
}

const QString& medDatabaseNonPersistentItem::birthdate() const
{
    return d->birthdate;
}

const QString& medDatabaseNonPersistentItem::patientId() const
{
  return d->patientId;
}

const QString& medDatabaseNonPersistentItem::studyName() const
{
    return d->studyName;
}

const QString& medDatabaseNonPersistentItem::studyId() const
{
  return d->studyId;
}

const QString& medDatabaseNonPersistentItem::studyUid() const
{
  return d->studyUid;
}


const QString& medDatabaseNonPersistentItem::seriesName() const
{
    return d->seriesName;
}

const QString& medDatabaseNonPersistentItem::seriesId() const
{
    return d->seriesId;
}

const QString& medDatabaseNonPersistentItem::seriesUid() const
{
  return d->seriesUid;
}

const QString& medDatabaseNonPersistentItem::file() const
{
    return d->file;
}

const QImage& medDatabaseNonPersistentItem::thumb() const
{
    return d->thumb;
}

const medDataIndex& medDatabaseNonPersistentItem::index() const
{
    return d->index;
}

dtkAbstractData *medDatabaseNonPersistentItem::data()
{
    return d->data;
}
