/* medDatabaseNonPersitentItem_p.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 18:51:32 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:54:12 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <medDataIndex.h>

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include <QtCore/QString>
#include <QtGui/QImage>

class dtkAbstractData;

class medDatabaseNonPersistentItemPrivate
{
public:
    QString name;
    QString birthdate;
    QString patientId;
    QString studyName;
    QString studyId;
    QString studyUid;
    QString seriesName;
    QString seriesId;
    QString seriesUid;
    QString file;
    QImage thumb;

    medDataIndex index;

    dtkSmartPointer<dtkAbstractData> data;
};


