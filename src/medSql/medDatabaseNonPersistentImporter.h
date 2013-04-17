/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medJobItem.h>


class dtkAbstractData;

class medDatabaseNonPersistentImporterPrivate;

/**
 * @class medDatabaseNonPersistentImporter
 * @author Pierre Fillard
 * @brief Import an instance of a dtkAbstractData into the non-persistent database
 * This class imports an instance of a dtkAbstractData into the non-persistent
 * database. The metaData PatientName, StudyDescription and SeriesDescription
 * must be set, otherwise import will fail.
 */
class medDatabaseNonPersistentImporter : public medJobItem
{
    Q_OBJECT

public:
     medDatabaseNonPersistentImporter(dtkAbstractData *data,
                                      const QString& callerUuid);
    ~medDatabaseNonPersistentImporter();

    void run();

public slots:
    void onCancel(QObject*);

signals:
    void nonPersistentImported(const medDataIndex &, QString);


private:
    medDatabaseNonPersistentImporterPrivate *d;
};



