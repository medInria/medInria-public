/* medDataIndex.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 09:29:18 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:06:14 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 15
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

bool medDataIndex::isValid(void)
{
    return (d->patientId >= 0);
}

bool medDataIndex::isValidForPatient(void)
{
    return (d->patientId >= 0);
}

bool medDataIndex::isValidForStudy(void)
{
    return ((d->patientId >= 0) && (d->studyId >= 0));
}

bool medDataIndex::isValidForSeries(void)
{
    return ((d->patientId >= 0) && (d->studyId >= 0) && (d->seriesId >= 0));
}

bool medDataIndex::isValidForImage(void)
{
    return ((d->patientId >= 0) && (d->studyId >= 0) && (d->seriesId >= 0) && (d->imageId >= 0));
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
