/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medSqlExport.h"

#include <medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class dtkAbstractData;

class medDatabaseNonPersistentItemPrivate;

class MEDSQL_EXPORT medDatabaseNonPersistentItem : public QObject
{
    Q_OBJECT

public:
     medDatabaseNonPersistentItem();
    ~medDatabaseNonPersistentItem();

    const QString& name() const;
    const QString& birthdate() const;
    const QString& patientId() const;
    const QString& studyName() const;
    const QString& seriesName() const;
    const QString& file() const;
    const QString& studyId() const;
    const QString& studyUid() const;
    const QString& seriesId() const;
    const QString& seriesUid() const;
    const QImage& thumb() const;

    const medDataIndex& index() const;
    
    dtkAbstractData *data();

private:
    friend class medDatabaseNonPersitentController;
    friend class medDatabaseNonPersistentReader;
    friend class medDatabaseNonPersistentImporter;

private:
    medDatabaseNonPersistentItemPrivate *d;
};


