/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "dtkCore/dtkPluginManager.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkSmartPointer.h"

int itkMetaDataImageReaderTest (int argc, char* argv[])
{
  if (argc<3)
    return EXIT_FAILURE;

  // the test file
  QString filepath = argv[2];
  filepath += "/BrainProtonDensitySliceBorder20.mhd";
    
  dtkPluginManager::instance()->setPath (argv[1]);
  dtkPluginManager::instance()->initialize();

  // first method to read data
  dtkSmartPointer<dtkAbstractData> data1 = dtkAbstractDataFactory::instance()->createSmartPointer("itkDataImageDouble3");

  if (!data1)
      return EXIT_FAILURE;

  data1->enableReader ("itkMetaDataImageReader");
  
  if (!data1->read(filepath))
      return EXIT_FAILURE;

  
  // second method
  dtkAbstractData* data2 = 0;

  QList<QString> readers = dtkAbstractDataFactory::instance()->readers();
  for (int i=0; i<readers.size(); i++) {            
      dtkSmartPointer<dtkAbstractDataReader> dataReader( dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]) );
      if (dataReader->canRead( filepath )) {
          dataReader->read( filepath );
          data2 = dataReader->data();
          break;
      }
  }

  if (!data2)
      return EXIT_FAILURE;
  

  return EXIT_SUCCESS;
}
