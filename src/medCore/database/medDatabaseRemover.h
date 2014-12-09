/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medAbstractJob.h>

#include <medCoreExport.h>

class medDatabaseRemoverPrivate;

/**
 * @class medDatabaseRemover
 * @brief Removes given data from the database.
 */
class MEDCORE_EXPORT medDatabaseRemover : public medAbstractJob
{
    Q_OBJECT;

public:
     medDatabaseRemover(const medDataIndex &index);
    ~medDatabaseRemover();


signals:

    /**
    * Signal emitted when the removing process finishes.
    * @param index – the @medDataIndex of the removed item
    **/
    void removed(const medDataIndex &index);

public slots:
    void onCancel(QObject*);

protected:
    virtual void internalRun();

    void removeImage( int patientId, int studyId, int seriesId, int imageId);


    bool isSeriesEmpty( int seriesId );
    void removeSeries( int patientId, int studyId, int seriesId );
    bool isStudyEmpty( int studyId );
    void removeStudy( int patientId, int studyId );
    bool isPatientEmpty( int patientId );
    void removePatient( int patientId );

    void removeFile( const QString & filename );

    void removeDataFile( const medDataIndex &index, const QString & filename );
    bool removeTableRow( const QString &table, int id );

private:
    medDatabaseRemoverPrivate *d;
};


