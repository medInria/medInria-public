/* medDataIndex.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 09:29:10 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu Mar  4 10:37:12 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 23
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATAINDEX_H
#define MEDDATAINDEX_H

#include <QtCore>

#include "medCoreExport.h"

class medDataIndexPrivate;

class MEDCORE_EXPORT medDataIndex : public QObject
{
    Q_OBJECT

public:
     medDataIndex(int patientId = -1, int studyId = -1, int seriesId = -1, int imageId = -1);
     medDataIndex(const medDataIndex& index);
    ~medDataIndex(void);

    bool isValid          (void) const;
    bool isValidForPatient(void) const;
    bool isValidForStudy  (void) const;
    bool isValidForSeries (void) const;
    bool isValidForImage  (void) const;

    QString asString() const;

    int patientId(void) const;
    int   studyId(void) const;
    int  seriesId(void) const;
    int   imageId(void) const;

    medDataIndex& operator=(const medDataIndex& index);

    friend MEDCORE_EXPORT bool operator==(const medDataIndex& index1, const medDataIndex& index2);
    friend MEDCORE_EXPORT bool operator!=(const medDataIndex& index1, const medDataIndex& index2);
	
	/** The less than operator can be used with STL maps.
     *  Ordering is by patientId, then studyId, then seriesId, then imageId.
     */  
    friend MEDCORE_EXPORT bool operator<(const medDataIndex& index1, const medDataIndex& index2);
    friend MEDCORE_EXPORT QDebug operator<<(QDebug debug, const medDataIndex& index);
    friend MEDCORE_EXPORT QDebug operator<<(QDebug debug,       medDataIndex *index);

private:
    medDataIndexPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

MEDCORE_EXPORT bool operator==(const medDataIndex& index1, const medDataIndex& index2);
MEDCORE_EXPORT bool operator!=(const medDataIndex& index1, const medDataIndex& index2);
MEDCORE_EXPORT bool operator<(const medDataIndex& index1, const medDataIndex& index2);

MEDCORE_EXPORT QDebug operator<<(QDebug debug, const medDataIndex& index);
MEDCORE_EXPORT QDebug operator<<(QDebug debug,       medDataIndex *index);

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

MEDCORE_EXPORT uint qHash(const medDataIndex &key);

// /////////////////////////////////////////////////////////////////
// Meta type registration
// /////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(medDataIndex)

#endif // MEDDATAINDEX_H
