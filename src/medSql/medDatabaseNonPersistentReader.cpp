/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNonPersistentController.h"
#include "medDatabaseController.h"
#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"
#include "medDatabaseNonPersistentReader.h"

#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>

class medDatabaseNonPersistentReaderPrivate
{
public:
    medDatabaseNonPersistentReaderPrivate ( const QString& uuid ) :callerUuid ( uuid ) {}
    QString file;
    bool isCancelled;
    const QString callerUuid;
    medDataIndex index;
};



medDatabaseNonPersistentReader::medDatabaseNonPersistentReader ( const QString& file
        ,const QString& callerUuid ) : medJobItem(), d ( new medDatabaseNonPersistentReaderPrivate ( callerUuid ) )
{
    d->file = file;
    d->isCancelled = false;
    qDebug() << "npr created with uuid:"<< d->callerUuid;
}

medDatabaseNonPersistentReader::~medDatabaseNonPersistentReader()
{
    delete d;

    d = NULL;
}

void medDatabaseNonPersistentReader::run()
{
    qDebug() << DTK_PRETTY_FUNCTION;

    if ( d->file.isEmpty() )
        return;

    QDir dir ( d->file );
    dir.setFilter ( QDir::Files );

    QStringList fileList;
    if ( dir.exists() )
    {
        QDirIterator directory_walker ( d->file, QDir::Files, QDirIterator::Subdirectories );
        while ( directory_walker.hasNext() )
        {
            fileList << directory_walker.next();
        }
    }
    else
        fileList << d->file;

    fileList.sort();

    QMap<QString, QStringList> imagesToWriteMap;
    QMap<QString, QString> patientsToWriteMap;
    QMap<QString, QString> seriesToWriteMap;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    int fileCount = fileList.count();
    int fileIndex = 0;

    QMap<QString, int> keyToInt;
    int currentIndex = 1;

    foreach ( QString file, fileList )
    {

        emit progress ( this, ( int ) ( ( ( qreal ) fileIndex/ ( qreal ) fileCount ) *50.0 ) );

        fileIndex++;

        QFileInfo fileInfo ( file );

        dtkSmartPointer<dtkAbstractData> dtkdata;

        for ( int i=0; i<readers.size(); i++ )
        {
            dtkSmartPointer<dtkAbstractDataReader> dataReader;
            dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer ( readers[i] );

            if ( dataReader->canRead ( fileInfo.filePath() ) )
            {
                dataReader->readInformation ( fileInfo.filePath() );
                dtkdata = dataReader->data();
                break;
            }
        }

        if ( !dtkdata )
            continue;

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::PatientName.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << fileInfo.baseName() );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::BirthDate.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::BirthDate.key(), "" );

        QString generatedPatientID = QUuid::createUuid().toString().replace("{","").replace("}","");

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::PatientID.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << generatedPatientID );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::SeriesDescription.key(), QStringList() << fileInfo.baseName() );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::StudyID.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::StudyDicomID.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "0" );

        QString generatedSeriesID = QUuid::createUuid().toString().replace("{","").replace("}","");

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::SeriesID.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << generatedSeriesID );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::SeriesDicomID.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::SeriesDicomID.key(), QStringList() << "0" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::Orientation.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::Orientation.key(), QStringList() << "" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::SeriesNumber.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::SeriesNumber.key(), QStringList() << "" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::SequenceName.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::SequenceName.key(), QStringList() << "" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::SliceThickness.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::SliceThickness.key(), QStringList() << "" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::Rows.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::Rows.key(), QStringList() << "" );

        if ( !dtkdata->hasMetaData ( medMetaDataKeys::Columns.key() ) )
            dtkdata->addMetaData ( medMetaDataKeys::Columns.key(), QStringList() << "" );


        QString patientName = medMetaDataKeys::PatientName.getFirstValue(dtkdata);
        QString birthdate   = medMetaDataKeys::BirthDate.getFirstValue(dtkdata);
        QString patientID   = medMetaDataKeys::PatientID.getFirstValue(dtkdata);
        QString studyName   = medMetaDataKeys::StudyDescription.getFirstValue(dtkdata);
        QString seriesName  = medMetaDataKeys::SeriesDescription.getFirstValue(dtkdata);

        QString studyId = medMetaDataKeys::StudyID.getFirstValue(dtkdata);
        QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(dtkdata);
        QString studyUid = medMetaDataKeys::StudyDicomID.getFirstValue(dtkdata);
        QString seriesUid = medMetaDataKeys::SeriesDicomID.getFirstValue(dtkdata);
        QString orientation = medMetaDataKeys::Orientation.getFirstValue(dtkdata);
        QString seriesNumber = medMetaDataKeys::SeriesNumber.getFirstValue(dtkdata);
        QString sequenceName = medMetaDataKeys::SequenceName.getFirstValue(dtkdata);
        QString sliceThickness = medMetaDataKeys::SliceThickness.getFirstValue(dtkdata);
        QString rows = medMetaDataKeys::Rows.getFirstValue(dtkdata);
        QString columns = medMetaDataKeys::Columns.getFirstValue(dtkdata);

        // define a unique key string to identify which volume an image belongs to.
        // we use: patientName, studyID, seriesID, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns. All images of the same volume should share similar values of these parameters
        QString key = patientName+studyUid+seriesUid+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;
        if ( !keyToInt.contains ( key ) )
        {
            keyToInt[key] = currentIndex;
            currentIndex++;
        }

        imagesToWriteMap[ key ] << fileInfo.filePath();
        patientsToWriteMap[ key ] = patientID;
        seriesToWriteMap[ key ] = seriesId;
    }

    // read and write images in mhd format

    QList< dtkSmartPointer<dtkAbstractData> > dtkDataList;

    QMap<QString, QStringList>::const_iterator it;
    QMap<QString, QString>::const_iterator itPat = patientsToWriteMap.begin();
    QMap<QString, QString>::const_iterator itSer = seriesToWriteMap.begin();

    int imagesCount = imagesToWriteMap.count();
    int imageIndex = 0;

    for ( it = imagesToWriteMap.begin(); it != imagesToWriteMap.end(); it++)
    {
        emit progress ( this, ( int ) ( ( ( qreal ) imageIndex/ ( qreal ) imagesCount ) *50.0 + 50.0 ) );

        imageIndex++;

        dtkSmartPointer<dtkAbstractData> imData;

        for ( int i=0; i<readers.size(); i++ )
        {
            dtkSmartPointer<dtkAbstractDataReader> dataReader;
            dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer ( readers[i] );

            if ( dataReader->canRead ( it.value() ) )
            {
                //connect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));
                if ( dataReader->read ( it.value() ) )
                {
                    imData = dataReader->data();

                    if ( imData )
                    {
                        if ( !imData->hasMetaData ( medMetaDataKeys::FilePaths.key() ) )
                            imData->addMetaData ( medMetaDataKeys::FilePaths.key(), it.value() );

                        if ( !imData->hasMetaData ( medMetaDataKeys::PatientName.key() ) )
                            imData->addMetaData ( medMetaDataKeys::PatientName.key(), QStringList() << QFileInfo ( it.value() [0] ).baseName() );

                        if ( !imData->hasMetaData ( medMetaDataKeys::BirthDate.key() ) )
                            imData->addMetaData ( medMetaDataKeys::BirthDate.key(), "" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::PatientID.key() ) )
                            imData->addMetaData ( medMetaDataKeys::PatientID.key(), QStringList() << itPat.value() );

                        if ( !imData->hasMetaData ( medMetaDataKeys::StudyDescription.key() ) )
                            imData->addMetaData ( medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::SeriesDescription.key() ) )
                            imData->addMetaData ( medMetaDataKeys::SeriesDescription.key(), QStringList() << QFileInfo ( it.value() [0] ).baseName() );

                        if ( !imData->hasMetaData ( medMetaDataKeys::StudyID.key() ) )
                            imData->addMetaData ( medMetaDataKeys::StudyID.key(), QStringList() << "0" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::StudyDicomID.key() ) )
                            imData->addMetaData ( medMetaDataKeys::StudyDicomID.key(), QStringList() << "0" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::SeriesID.key() ) )
                            imData->addMetaData ( medMetaDataKeys::SeriesID.key(), QStringList() << itSer.value() );

                        if ( !imData->hasMetaData ( medMetaDataKeys::SeriesDicomID.key() ) )
                            imData->addMetaData ( medMetaDataKeys::SeriesDicomID.key(), QStringList() << "0" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::Orientation.key() ) )
                            imData->addMetaData ( medMetaDataKeys::Orientation.key(), QStringList() << "" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::SeriesNumber.key() ) )
                            imData->addMetaData ( medMetaDataKeys::SeriesNumber.key(), QStringList() << "" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::SequenceName.key() ) )
                            imData->addMetaData ( medMetaDataKeys::SequenceName.key(), QStringList() << "" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::SliceThickness.key() ) )
                            imData->addMetaData ( medMetaDataKeys::SliceThickness.key(), QStringList() << "" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::Rows.key() ) )
                            imData->addMetaData ( medMetaDataKeys::Rows.key(), QStringList() << "" );

                        if ( !imData->hasMetaData ( medMetaDataKeys::Columns.key() ) )
                            imData->addMetaData ( medMetaDataKeys::Columns.key(), QStringList() << "" );

//                         imData->addMetaData ( "FileName", it.key() );

                        break;
                    }
                }

                //disconnect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));
            }
        }

        if ( !imData )
        {
            qDebug() << "Could not read data: " << it.value();
            continue;
        }

        dtkDataList.push_back ( imData );

        itPat++;
        itSer++;

    }

    medDataIndex index;
    QPointer<medDatabaseNonPersistentControllerImpl> npdc =
            medDatabaseNonPersistentController::instance();

    for ( int i=0; i<dtkDataList.count(); i++ )
    {

        dtkAbstractData *data = dtkDataList[i];

        QList<medDatabaseNonPersistentItem*> items = npdc->items();

        int     patientDbId   = -1;
        QString patientName = medMetaDataKeys::PatientName.getFirstValue(data);
        QString patientId = medMetaDataKeys::PatientID.getFirstValue(data);

        // check if patient is already in the persistent database
        medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient ( patientName );
        if ( databaseIndex.isValid() )
        {
            qDebug() << "Patient exists in the database, I reuse her Id";
            patientDbId = databaseIndex.patientId();
        }
        else
        {
            for ( int i=0; i<items.count(); i++ )
                if ( items[i]->name() ==patientName )
                {
                    patientDbId = items[i]->index().patientId();
                    break;
                }
        }

        if ( patientDbId==-1 )
            patientDbId = npdc->patientId ( true );

        int     studyDbId   = -1;
        QString studyName = medMetaDataKeys::StudyDescription.getFirstValue(data);
        QString studyId = medMetaDataKeys::StudyID.getFirstValue(data);

        databaseIndex = medDatabaseController::instance()->indexForStudy ( patientName, studyName );
        if ( databaseIndex.isValid() )
        {
            qDebug() << "Study exists in the database, I reuse its Id";
            studyDbId = databaseIndex.studyId();
        }
        else
        {
            for ( int i=0; i<items.count(); i++ )
                if ( items[i]->name() ==patientName && items[i]->studyName() ==studyName )
                {
                    studyDbId = items[i]->index().studyId();
                    break;
                }
        }

        if ( studyDbId==-1 )
            studyDbId = npdc->studyId ( true );

        index = medDataIndex ( npdc->dataSourceId(), patientDbId, studyDbId, npdc->seriesId ( true ), -1 );

        QString seriesName = medMetaDataKeys::SeriesDescription.getFirstValue(data);
        QString seriesId = medMetaDataKeys::SeriesID.getFirstValue(data);

        QFileInfo info ( d->file );

        medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

        if ( !patientName.isEmpty() )
            item->d->name = patientName;
        else
            item->d->name = info.baseName();

        item->d->patientId = patientId;
        item->d->studyName = studyName;
        item->d->seriesName = seriesName;
        item->d->seriesId = seriesId;
        item->d->file = d->file;
        item->d->thumb = data->thumbnail();
        item->d->index = index;
        item->d->data = data;

        npdc->insert ( index, item );
    }

    d->index = index;

    emit progress ( this, 100 );
    emit success ( this );
//    qDebug() << "uuid value before signal"<< d->callerUuid;
    emit nonPersistentRead ( index,d->callerUuid );
}

medDataIndex medDatabaseNonPersistentReader::index() const
{
    return d->index;
}

void medDatabaseNonPersistentReader::onCancel ( QObject* )
{
    d->isCancelled = true;
}



