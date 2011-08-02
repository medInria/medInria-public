/* medDataIndex.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 09:29:18 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar  4 10:51:50 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 26
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDataIndex.h"

class medDataIndexPrivate
{
public:
    int patientId;
    int studyId;
    int seriesId;
    int imageId;
};

medDataIndex::medDataIndex(int patientId, int studyId, int seriesId, int imageId) : d(new medDataIndexPrivate)
{
    d->patientId = patientId;
    d->studyId = studyId;
    d->seriesId = seriesId;
    d->imageId = imageId;
}

medDataIndex::medDataIndex(const medDataIndex& index) : d(new medDataIndexPrivate)
{
    d->patientId = index.d->patientId;
    d->studyId = index.d->studyId;
    d->seriesId = index.d->seriesId;
    d->imageId = index.d->imageId;
}

medDataIndex::~medDataIndex(void)
{
    delete d;

    d = NULL;
}

bool medDataIndex::isValid(void) const
{
    return (d->patientId >= 0);
}

bool medDataIndex::isValidForPatient(void) const
{
    return (d->patientId >= 0);
}

bool medDataIndex::isValidForStudy(void) const
{
    return ((d->patientId >= 0) && (d->studyId >= 0));
}

bool medDataIndex::isValidForSeries(void) const
{
    return ((d->patientId >= 0) && (d->studyId >= 0) && (d->seriesId >= 0));
}

bool medDataIndex::isValidForImage(void) const
{
    return ((d->patientId >= 0) && (d->studyId >= 0) && (d->seriesId >= 0) && (d->imageId >= 0));
}

void medDataIndex::setPatientId (int id)
{
    d->patientId = id;
}

void medDataIndex::setStudyId (int id)
{
    d->studyId = id;
}

void medDataIndex::setSeriesId (int id)
{
    d->seriesId = id;
}

void medDataIndex::setImageId (int id)
{
    d->imageId = id;
}

int medDataIndex::patientId(void) const
{
    return d->patientId;
}

int medDataIndex::studyId(void) const
{
    return d->studyId;
}

int medDataIndex::seriesId(void) const
{
    return d->seriesId;
}

int medDataIndex::imageId(void) const
{
    return d->imageId;
}

medDataIndex& medDataIndex::operator=(const medDataIndex& index)
{
    d->patientId = index.d->patientId;
    d->studyId = index.d->studyId;
    d->seriesId = index.d->seriesId;
    d->imageId = index.d->imageId;

	return *this;
}

QString medDataIndex::asString() const
{
    QString returnString = QString::number(d->patientId) + " | " 
        + QString::number(d->studyId) + " | " 
        + QString::number(d->seriesId) + " | " 
        + QString::number(d->imageId);
    return returnString;
}

// static
bool medDataIndex::isMatch( const medDataIndex& index1, const medDataIndex& index2)
{
    if ( index1.patientId() == -1 || index2.patientId() == -1) 
        return true;

    if ( index1.patientId() != index2.patientId() ) 
        return false;

    if ( index1.studyId() == -1 || index2.studyId() == -1) 
        return true;

    if ( index1.studyId() != index2.studyId() ) 
        return false;

    if ( index1.seriesId() == -1 || index2.seriesId() == -1) 
        return true;

    if ( index1.seriesId() != index2.seriesId() ) 
        return false;

    if ( index1.imageId() == -1 || index2.imageId() == -1) 
        return true;

    if ( index1.imageId() != index2.imageId() ) 
        return false;

    return true; // patient, study, series and image match and are not -1;
}






// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

bool operator==(const medDataIndex& index1, const medDataIndex& index2)
{
    return ((index1.patientId() == index2.patientId()) && (index1.studyId() == index2.studyId()) && (index1.seriesId() == index2.seriesId()) && (index1.imageId() == index2.imageId()));
}

bool operator!=(const medDataIndex& index1, const medDataIndex& index2)
{
    return ((index1.patientId() != index2.patientId()) || (index1.studyId() != index2.studyId()) || (index1.seriesId() != index2.seriesId()) || (index1.imageId() == index2.imageId()));
}

QDebug operator<<(QDebug debug, const medDataIndex& index)
{
    debug.nospace() << "(" << index.d->patientId << ", " << index.d->studyId << ", " << index.d->seriesId << ", " << index.d->imageId << ")";

    return debug.space();
}

QDebug operator<<(QDebug debug, medDataIndex *index)
{
    debug.nospace() << "(" << index->d->patientId << ", " << index->d->studyId << ", " << index->d->seriesId << ", " << index->d->imageId << ")";

    return debug.space();
}

bool operator<(const medDataIndex& index1, const medDataIndex& index2)
{
    if (index1.patientId() < index2.patientId()) 
        return true;
    else if (index2.patientId() < index1.patientId()) 
        return false;
    else if (index1.studyId() < index2.studyId())
        return true;
    else if (index2.studyId() < index1.studyId())
        return false;
    else if (index1.seriesId() < index2.seriesId()) 
        return true;
    else if (index2.seriesId() < index1.seriesId()) 
        return false;
    else if (index1.imageId() < index2.imageId())
        return true;
    else if (index2.imageId() < index1.imageId())
        return false;
    // Equality if we get here.
    return false;
}

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

uint qHash(const medDataIndex &key)
{
   uint hash = 0;
   hash = hash ^ qHash(key.patientId());
   hash = hash ^ qHash(key.studyId());
   hash = hash ^ qHash(key.seriesId());
   hash = hash ^ qHash(key.imageId());
   return hash;
}
