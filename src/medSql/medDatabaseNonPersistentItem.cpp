/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"

medDatabaseNonPersistentItem::medDatabaseNonPersistentItem(void) : QObject(), d(new medDatabaseNonPersistentItemPrivate)
{
    //d->data = NULL;
}

medDatabaseNonPersistentItem::~medDatabaseNonPersistentItem(void)
{
    delete d;

    d = NULL;
}

const QString& medDatabaseNonPersistentItem::name(void) const
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

const QString& medDatabaseNonPersistentItem::studyName(void) const
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


const QString& medDatabaseNonPersistentItem::seriesName(void) const
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

const QString& medDatabaseNonPersistentItem::file(void) const
{
    return d->file;
}

const QImage& medDatabaseNonPersistentItem::thumb(void) const
{
    return d->thumb;
}

const medDataIndex& medDatabaseNonPersistentItem::index(void) const
{
    return d->index;
}

dtkAbstractData *medDatabaseNonPersistentItem::data(void)
{
    return d->data;
}
