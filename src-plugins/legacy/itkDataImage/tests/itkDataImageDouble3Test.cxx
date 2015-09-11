/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "dtkCore/dtkPluginManager.h"
#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkSmartPointer.h"

#include "medAbstractImageData.h"

#include <itkImage.h>

int itkDataImageDouble3Test (int argc, char* argv[])
{
  if (argc<3) {
      qDebug() << "Not enough arguments";
      return EXIT_FAILURE;
  }

  // the test file
  QString filepath = argv[2];
  filepath += "/BrainProtonDensitySliceBorder20.mhd";

  dtkPluginManager::instance()->setPath (argv[1]);
  dtkPluginManager::instance()->initialize();

  dtkSmartPointer<medAbstractData> medData;
  medData =  medAbstractDataFactory::instance()->createSmartPointer ("itkDataImageDouble3") ;
  medAbstractImageData *data = dynamic_cast<medAbstractImageData*>( medData.data() );

  if (!data) {
      qDebug() << "Cannot create data object from plugin";
      return EXIT_FAILURE;
  }

  data->enableReader ("itkMetaDataImageReader");

  if (!data->read(filepath)) {
      qDebug() << "Cannot read image file";
      return EXIT_FAILURE;
  }

  typedef itk::Image<double, 3> ImageType;

  ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*)(data->data()) );
  if (image.IsNull()) {
     qDebug() << "Cannot cast data() to ITK image";
     return EXIT_FAILURE;
  }

  ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
  if ((int)size[0]!=data->xDimension()) {
      qDebug() << "Bad X dimension";
      return EXIT_FAILURE;
  }
  if ((int)size[1]!=data->yDimension()) {
      qDebug() << "Bad Y dimension";
      return EXIT_FAILURE;
  }
  if ((int)size[2]!=data->zDimension()) {
      qDebug() << "Bad Z dimension";
      return EXIT_FAILURE;
  }


  // create an ITK image and feed a data plugin with it
  image = ImageType::New();
  ImageType::RegionType region;
  ImageType::SizeType   size2 = {{10,10,10}};
  ImageType::IndexType  index = {{0,0,0}};
  region.SetSize (size2);
  region.SetIndex (index);
  image->SetRegions (region);
  image->Allocate();

  dtkSmartPointer<medAbstractData> medData2;
  medData2 =  medAbstractDataFactory::instance()->createSmartPointer ("itkDataImageDouble3") ;
  medAbstractImageData *data2 = dynamic_cast<medAbstractImageData*>( medData2.data() );
  if (!data2) {
      qDebug() << "Cannot create data object from plugin";
      return EXIT_FAILURE;
  }

  data2->setData ( image.GetPointer() );

  ImageType::Pointer image2 = dynamic_cast<ImageType*>( (itk::Object*)(data2->data()) );
  if (image2.IsNull()) {
     qDebug() << "Cannot cast data() to ITK image";
     return EXIT_FAILURE;
  }

  if ((int)size2[0]!=data2->xDimension()) {
      qDebug() << "Bad X dimension";
      return EXIT_FAILURE;
  }
  if ((int)size2[1]!=data2->yDimension()) {
      qDebug() << "Bad Y dimension";
      return EXIT_FAILURE;
  }
  if ((int)size2[2]!=data2->zDimension()) {
      qDebug() << "Bad Z dimension";
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
