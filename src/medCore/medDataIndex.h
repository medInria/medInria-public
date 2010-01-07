/* medDataIndex.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 09:29:10 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 21 09:29:14 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
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

class MEDCORE_EXPORT medDataIndex
{
public:
     medDataIndex(int patientId = -1, int studyId = -1, int seriesId = -1, int imageId = -1);
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

    friend bool operator==(const medDataIndex& index1, const medDataIndex& index2);
    friend bool operator!=(const medDataIndex& index1, const medDataIndex& index2);

private:
    medDataIndexPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// Convenience operators
// /////////////////////////////////////////////////////////////////

bool operator==(const medDataIndex& index1, const medDataIndex& index2);
bool operator!=(const medDataIndex& index1, const medDataIndex& index2);

// /////////////////////////////////////////////////////////////////
// Hash functions
// /////////////////////////////////////////////////////////////////

uint qHash(const medDataIndex &key);

#endif // MEDDATAINDEX_H
