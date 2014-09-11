/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabaseNonPersistentItem.h>
#include <medDatabaseNonPersistentItem_p.h>

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

void medDatabaseNonPersistentItem::setName(const QString& name)
{
    d->name = name;
}

const QString& medDatabaseNonPersistentItem::birthdate () const
{
    return d->birthdate;
}

void medDatabaseNonPersistentItem::setBirthdate(const QString& birthdate)
{
    d->birthdate = birthdate;
}

const QString& medDatabaseNonPersistentItem::patientId () const
{
  return d->patientId;
}

void medDatabaseNonPersistentItem::setPatientId(const QString& patientId)
{
    d->patientId = patientId;
}

const QString& medDatabaseNonPersistentItem::studyName(void) const
{
    return d->studyName;
}

void medDatabaseNonPersistentItem::setStudyName(const QString& studyName)
{
    d->studyName = studyName;
}

const QString& medDatabaseNonPersistentItem::studyId () const
{
  return d->studyId;
}

void medDatabaseNonPersistentItem::setStudyId(const QString& studyId)
{
    d->studyId = studyId;
}

const QString& medDatabaseNonPersistentItem::studyUid () const
{
  return d->studyUid;
}

void medDatabaseNonPersistentItem::setStudyUid(const QString& studyUid)
{
    d->studyUid = studyUid;
}

const QString& medDatabaseNonPersistentItem::seriesName() const
{
    return d->seriesName;
}

void medDatabaseNonPersistentItem::setSeriesName(const QString& seriesName)
{
    d->seriesName = seriesName;
}

const QString& medDatabaseNonPersistentItem::seriesId () const
{
    return d->seriesId;
}

void medDatabaseNonPersistentItem::setSeriesId(const QString& seriesId)
{
    d->seriesId = seriesId;
}

const QString& medDatabaseNonPersistentItem::seriesUid () const
{
  return d->seriesUid;
}

void medDatabaseNonPersistentItem::setSeriesUid(const QString& seriesUid)
{
    d->seriesUid = seriesUid;
}

const QString& medDatabaseNonPersistentItem::file() const
{
    return d->file;
}

void medDatabaseNonPersistentItem::setFile(const QString& file)
{
    d->file = file;
}

const QImage& medDatabaseNonPersistentItem::thumb() const
{
    return d->thumb;
}

void medDatabaseNonPersistentItem::setThumb(const QImage& thumb)
{
    d->thumb = thumb;
}

const medDataIndex& medDatabaseNonPersistentItem::index() const
{
    return d->index;
}

void medDatabaseNonPersistentItem::setIndex(const medDataIndex& index)
{
    d->index = index;
}

medAbstractData *medDatabaseNonPersistentItem::data(void)
{
    return d->data;
}


bool medDatabaseNonPersistentItem::Match(medAbstractData *medData)
{
    QString patientName = medMetaDataKeys::PatientName.getFirstValue(medData).simplified();
    QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(medData).simplified();
    QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(medData);
    QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(medData).simplified();
    QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(medData);
    QString orientation = medMetaDataKeys::Orientation.getFirstValue(medData);
    QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(medData);
    QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(medData);
    QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(medData);
    QString rows = medMetaDataKeys::Rows.getFirstValue(medData);
    QString columns = medMetaDataKeys::Columns.getFirstValue(medData);

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
