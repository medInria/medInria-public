/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>

#include <medJobItem.h>

#include <medDataIndex.h>

class dtkAbstractData;
class medDatabaseWriterPrivate;

/**
 * @class medDatabaseWriter
 * @author Pierre Fillard
 * @brief Stores an instance of a dtkAbstractData to the SQL database.
 * This class stores an instance of a dtkAbstractData to the SQL database.
 * The metadata PatientName / StudyDescription are used to retrieve the
 * patient Id in the database. If none is given, John Doe / EmptyStudy will
 * be used. The SeriesDescription is mandatory and import will fail it not
 * found.
 */
class medDatabaseWriter : public medJobItem
{
    Q_OBJECT

public:
     medDatabaseWriter(dtkAbstractData *data, const QString& callerUuid = QString());
    ~medDatabaseWriter();

    void run();

public slots:
    void onCancel ( QObject* );

signals:
    void addedIndex(const medDataIndex &, const QString &);
    void addedIndex(const medDataIndex &);

private:
    medDatabaseWriterPrivate *d;
};


