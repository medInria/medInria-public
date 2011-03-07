
#include "dtkCore/dtkPluginManager.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"

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
      dtkAbstractData *data = dtkAbstractDataFactory::instance()->create ("itkDataImageDouble3");
      if (!data) {
        qDebug() << "Cannot create data object from plugin";
	return EXIT_FAILURE;
      }
      data->enableReader ("itkDCMTKDataImageReader");
      dtkAbstractDataReader *reader = data->reader("itkDCMTKDataImageReader");

      QFileInfo fileInfo( file );
      
      if (reader->canRead( fileInfo.filePath() )) {
	
	  reader->readInformation( fileInfo.filePath() );

      
      if(!data->hasMetaData("PatientName"))
	  data->addMetaData("PatientName", QStringList() << "John Doe");

      if(!data->hasMetaData("StudyDescription"))
	  data->addMetaData("StudyDescription", QStringList() << "EmptyStudy");

      if(!data->hasMetaData("SeriesDescription"))
	  data->addMetaData("SeriesDescription", QStringList() << fileInfo.baseName());

      if(!data->hasMetaData("StudyID"))
	  data->addMetaData("StudyID", QStringList() << "");
	
      if(!data->hasMetaData("SeriesID"))
	  data->addMetaData("SeriesID", QStringList() << "");

      if(!data->hasMetaData("Orientation"))
	  data->addMetaData("Orientation", QStringList() << "");

      if(!data->hasMetaData("SeriesNumber"))
	  data->addMetaData("SeriesNumber", QStringList() << "");

      if(!data->hasMetaData("SequenceName"))
	  data->addMetaData("SequenceName", QStringList() << "");

      if(!data->hasMetaData("SliceThickness"))
	  data->addMetaData("SliceThickness", QStringList() << "");

      if(!data->hasMetaData("Rows"))
	  data->addMetaData("Rows", QStringList() << "");
	
      if(!data->hasMetaData("Columns"))
	  data->addMetaData("Columns", QStringList() << "");


      QString patientName = data->metaDataValues( "PatientName" )[0];
      QString studyId = data->metaDataValues("StudyID")[0];
      QString seriesId = data->metaDataValues("SeriesID")[0];
      QString orientation = data->metaDataValues("Orientation")[0];
      QString seriesNumber = data->metaDataValues("SeriesNumber")[0];
      QString sequenceName = data->metaDataValues("SequenceName")[0];
      QString sliceThickness = data->metaDataValues("SliceThickness")[0];
      QString rows = data->metaDataValues("Rows")[0];
      QString columns = data->metaDataValues("Columns")[0];
      

      QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;
      
      seriesFilenames[key] << fileInfo.filePath();
      }
      
      delete data;
    }
    
    

    QMap<QString, QStringList>::const_iterator it = seriesFilenames.begin();

    for(; it!=seriesFilenames.end(); it++) {

        dtkAbstractData *data = dtkAbstractDataFactory::instance()->create ("itkDataImageShort4"); // to be compatible with all dicoms
	
	if (!data) {
	    std::cerr << "Cannot create data object from plugin" << std::endl;
	    return EXIT_FAILURE;
	}
	
	data->enableReader ("itkDCMTKDataImageReader");
	
	if (!data->read( it.value() )) {
	    qDebug() << "Cannot read DICOM dir: " << it.value();
	    return EXIT_FAILURE;
	}

	delete data;
    }
    
    return EXIT_SUCCESS;
}
