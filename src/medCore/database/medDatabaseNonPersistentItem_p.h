/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medDataIndex.h>

#include <medAbstractData.h>
#include <dtkCoreSupport/dtkSmartPointer.h>

#include <QtCore/QString>
#include <QtGui/QImage>

class medAbstractData;

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
    QString orientation;
    QString seriesNumber;
    QString sequenceName;
    QString sliceThickness;
    QString rows;
    QString columns;

    medDataIndex index;

    dtkSmartPointer<medAbstractData> data;
};


