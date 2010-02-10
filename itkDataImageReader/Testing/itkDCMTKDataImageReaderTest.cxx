
#include "dtkCore/dtkPluginManager.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"

#include <QDir>

#include <iostream>

int itkDCMTKDataImageReaderTest (int argc, char* argv[])
{
	if (argc<3) {
		std::cerr << "Not enough arguments" << std::endl;
        return EXIT_FAILURE;
	}
    
  dtkPluginManager::instance()->setPath (argv[1]);
  dtkPluginManager::instance()->initialize();

  // the test file
  QDir dir (argv[2]);
  dir.setFilter(QDir::Files | QDir::NoSymLinks);
	
	QDir::setCurrent( argv[2] );

  // first method to read data
  dtkAbstractData *data = dtkAbstractDataFactory::instance()->create ("itkDataImageDouble3");

	if (!data) {
		std::cerr << "Cannot create data object from plugin" << std::endl;
        return EXIT_FAILURE;
	}

  data->enableReader ("itkDCMTKDataImageReader");
  
	if (!data->read(dir.entryList())) {
		std::cerr << "Cannot read DICOM dir: " << argv[2] << std::endl;
        return EXIT_FAILURE;
	}

  return EXIT_SUCCESS;
}
