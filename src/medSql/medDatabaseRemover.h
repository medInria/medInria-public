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

#include <medCore/medDataIndex.h>
#include <medCore/medJobItem.h>

#include "medSqlExport.h"

class medDatabaseRemoverPrivate;

/**
 * @class medDatabaseRemover
 * @brief Removes given data from the database.
 */
class MEDSQL_EXPORT medDatabaseRemover : public medJobItem
{
    Q_OBJECT;

public:
     medDatabaseRemover(const medDataIndex &index);
    ~medDatabaseRemover();

    void run();

signals:

    /**
    * Signal emitted when the removing process finishes.
    * @param index – the @medDataIndex of the removed item
    **/
    void removed(const medDataIndex &index);

public slots:
    void onCancel(QObject*);

protected:
    void removeImage( int patientId, int studyId, int seriesId, int imageId);


    bool isSeriesEmpty( int seriesId );
    void removeSeries( int patientId, int studyId, int seriesId );
    bool isStudyEmpty( int studyId );
    void removeStudy( int patientId, int studyId );
    bool isPatientEmpty( int patientId );
    void removePatient( int patientId );

    //! Remove a single file
    void removeFile( const QString & filename );
    //! Remove a data image file. Includes special cases for some file types.
    void removeDataFile( const medDataIndex &index, const QString & filename );
    void removeTableRow( const QString &table, int id );

private:
    medDatabaseRemoverPrivate *d;
};


