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

    bool isValid          (void);
    bool isValidForPatient(void);
    bool isValidForStudy  (void);
    bool isValidForSeries (void);
    bool isValidForImage  (void);

    int patientId(void) const;
    int   studyId(void) const;
    int  seriesId(void) const;
    int   imageId(void) const;

    medDataIndex& operator=(const medDataIndex& index);

    friend bool operator==(const medDataIndex& index1, const medDataIndex& index2);
    friend bool operator!=(const medDataIndex& index1, const medDataIndex& index2);

    friend QDebug operator<<(QDebug debug, const medDataIndex& index);
    friend QDebug operator<<(QDebug debug,       medDataIndex *index);

private:
    medDataIndexPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

bool operator==(const medDataIndex& index1, const medDataIndex& index2);
bool operator!=(const medDataIndex& index1, const medDataIndex& index2);

QDebug operator<<(QDebug debug, const medDataIndex& index);
QDebug operator<<(QDebug debug,       medDataIndex *index);

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

uint qHash(const medDataIndex &key);

// /////////////////////////////////////////////////////////////////
// Meta type registration
// /////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(medDataIndex)

#endif // MEDDATAINDEX_H
