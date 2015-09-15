/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCore/dtkPluginManager.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include <dtkCore/dtkSmartPointer.h>

#include <medMetaDataKeys.h>

#include <QtCore>
#include <QDir>
#include <QString>

#include <iostream>

int itkDCMTKDataImageReaderTest (int argc, char* argv[])
{
    if (argc<3) {
        std::cerr << "Not enough arguments" << std::endl;
	return EXIT_FAILURE;
    }

    QApplication application(argc, argv);

    qDebug() << "Path: " << argv[1];
    
    dtkPluginManager::instance()->setPath (argv[1]);
    dtkPluginManager::instance()->initialize();

    // the test file
    QDir dir (argv[2]);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    
    QDir::setCurrent( argv[2] );


    QMap<QString, QStringList> seriesFilenames;
    

    foreach (QString file, dir.entryList()) {
      dtkSmartPointer<dtkAbstractData> data;
      data = dtkAbstractDataFactory::instance()->createSmartPointer ("itkDataImageDouble3");
      if (!data) {
        qDebug() << "Cannot create data object from plugin";
	return EXIT_FAILURE;
      }
      data->enableReader ("itkDCMTKDataImageReader");
      dtkSmartPointer<dtkAbstractDataReader> reader;
      reader = data->reader("itkDCMTKDataImageReader");

      QFileInfo fileInfo( file );
      
      if (reader->canRead( fileInfo.filePath() )) {
	
	  reader->readInformation( fileInfo.filePath() );

      
      if(!data->hasMetaData(medMetaDataKeys::PatientName.key()))
	  data->addMetaData(medMetaDataKeys::PatientName.key(), QStringList() << "John Doe");

      if(!data->hasMetaData(medMetaDataKeys::StudyDescription.key()))
	  data->addMetaData(medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy");

      if(!data->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
	  data->addMetaData(medMetaDataKeys::SeriesDescription.key(), QStringList() << fileInfo.baseName());

      if(!data->hasMetaData(medMetaDataKeys::StudyID.key()))
	  data->addMetaData(medMetaDataKeys::StudyID.key(), QStringList() << "");
	
      if(!data->hasMetaData(medMetaDataKeys::SeriesID.key()))
	  data->addMetaData(medMetaDataKeys::SeriesID.key(), QStringList() << "");

      if(!data->hasMetaData(medMetaDataKeys::Orientation.key()))
	  data->addMetaData(medMetaDataKeys::Orientation.key(), QStringList() << "");

      if(!data->hasMetaData(medMetaDataKeys::SeriesNumber.key()))
	  data->addMetaData(medMetaDataKeys::SeriesNumber.key(), QStringList() << "");

      if(!data->hasMetaData(medMetaDataKeys::SequenceName.key()))
	  data->addMetaData(medMetaDataKeys::SequenceName.key(), QStringList() << "");

      if(!data->hasMetaData(medMetaDataKeys::SliceThickness.key()))
	  data->addMetaData(medMetaDataKeys::SliceThickness.key(), QStringList() << "");

      if(!data->hasMetaData(medMetaDataKeys::Rows.key()))
	  data->addMetaData(medMetaDataKeys::Rows.key(), QStringList() << "");
	
      if(!data->hasMetaData(medMetaDataKeys::Columns.key()))
	  data->addMetaData(medMetaDataKeys::Columns.key(), QStringList() << "");


      QString patientName = data->metaDataValues( medMetaDataKeys::PatientName.key() )[0];
      QString studyId = data->metaDataValues(medMetaDataKeys::StudyID.key())[0];
      QString seriesId = data->metaDataValues(medMetaDataKeys::SeriesID.key())[0];
      QString orientation = data->metaDataValues(medMetaDataKeys::Orientation.key())[0];
      QString seriesNumber = data->metaDataValues(medMetaDataKeys::SeriesNumber.key())[0];
      QString sequenceName = data->metaDataValues(medMetaDataKeys::SequenceName.key())[0];
      QString sliceThickness = data->metaDataValues(medMetaDataKeys::SliceThickness.key())[0];
      QString rows = data->metaDataValues(medMetaDataKeys::Rows.key())[0];
      QString columns = data->metaDataValues(medMetaDataKeys::Columns.key())[0];
      

      QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;
      
      seriesFilenames[key] << fileInfo.filePath();
      }
      
    }
    
    

    QMap<QString, QStringList>::const_iterator it = seriesFilenames.begin();

    for(; it!=seriesFilenames.end(); it++) {

        dtkSmartPointer<dtkAbstractData> data;
        data =  dtkAbstractDataFactory::instance()->createSmartPointer ("itkDataImageShort4") ; // to be compatible with all dicoms
	
	if (!data) {
	    std::cerr << "Cannot create data object from plugin" << std::endl;
	    return EXIT_FAILURE;
	}
	
	data->enableReader ("itkDCMTKDataImageReader");
	
	if (!data->read( it.value() )) {
	    qDebug() << "Cannot read DICOM dir: " << it.value();
	    return EXIT_FAILURE;
	}

    }
    
    return EXIT_SUCCESS;
}
