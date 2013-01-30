/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"

#include <medMetaDataKeys.h>


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

const QString medDatabaseNonPersistentItem::orientation(void) const
{
    return d->orientation;
}

const QString medDatabaseNonPersistentItem::seriesNumber(void) const
{
    return d->seriesNumber;
}

const QString medDatabaseNonPersistentItem::sequenceName(void) const
{
    return d->sequenceName;
}

const QString medDatabaseNonPersistentItem::sliceThickness(void) const
{
    return d->sliceThickness;
}

const QString medDatabaseNonPersistentItem::rows(void) const
{
    return d->rows;
}

const QString medDatabaseNonPersistentItem::columns(void) const
{
    return d->columns;
}

const medDataIndex& medDatabaseNonPersistentItem::index(void) const
{
    return d->index;
}

dtkAbstractData *medDatabaseNonPersistentItem::data(void)
{
    return d->data;
}


bool medDatabaseNonPersistentItem::Match(dtkAbstractData *dtkData)
{
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkData).simplified();
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(dtkData).simplified();
    QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(dtkData);
    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(dtkData).simplified();
    QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkData);
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkData);
    QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkData);
    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkData);
    QString rows = medMetaDataKeys::Rows.getFirstValue(dtkData);
    QString columns = medMetaDataKeys::Columns.getFirstValue(dtkData);
    
    return (
        patientName == d->name &&
        studyName == d->studyName &&
        studyUid == d->studyUid &&
        seriesName == d->seriesName &&
        seriesUid == d->seriesUid &&
        orientation == d->orientation &&
        seriesNumber == d->seriesNumber &&
        sequenceName == d->sequenceName &&
        sliceThickness == d->sliceThickness &&
        rows == d->rows );
}
