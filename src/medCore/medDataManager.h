/* medDataManager.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 08:34:45 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 21 08:34:49 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATAMANAGER_H
#define MEDDATAMANAGER_H

#include <QtCore/QObject>

#include "medCoreExport.h"

#include "medDataIndex.h"

class dtkAbstractData;
class dtkAbstractDataFactory;

class medDataManagerPrivate;

class MEDCORE_EXPORT medDataManager : public QObject
{
    Q_OBJECT

public:
    static medDataManager *instance(void);

    void insert(const medDataIndex& index, dtkAbstractData *data);
    void insert(int patientId, int studyId, int seriesId, int imageId, dtkAbstractData *data);

          dtkAbstractData *  data(const medDataIndex& index);
          dtkAbstractData *  data(int patientId = -1, int studyId = -1, int seriesId = -1, int imageId = -1);
    QList<dtkAbstractData *> dataForPatient(int patientId);
    QList<dtkAbstractData *> dataForStudy  (int   studyId);
    QList<dtkAbstractData *> dataForSeries (int  seriesId);
    QList<dtkAbstractData *> dataForImage  (int   imageId);

protected:
     medDataManager(void);
    ~medDataManager(void);

protected:
    static medDataManager *s_instance;

private:
    medDataManagerPrivate *d;
};

#endif // MEDDATAMANAGER_H
