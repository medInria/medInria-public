#include "medItk/medItk.h"

#include "dtkCore/dtkPluginManager.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractDataImage.h"

int itkDataImageDouble3Test (int argc, char* argv[])
{
  if (argc<3) {
      std::cerr << "Not enough arguments" << std::endl;
      return EXIT_FAILURE;
  }
  
  // the test file
  QString filepath = argv[2];
  filepath += "/BrainProtonDensitySliceBorder20.mhd";
  
  dtkPluginManager::instance()->setPath (argv[1]);
  dtkPluginManager::instance()->initialize();
  
  dtkAbstractDataImage *data = dynamic_cast<dtkAbstractDataImage*>( dtkAbstractDataFactory::instance()->create ("itkDataImageDouble3") );
  
  if (!data) {
      std::cerr << "Cannot create data object from plugin" << std::endl;
      return EXIT_FAILURE;
  }
  
  data->enableReader ("itkMetaDataImageReader");
  
  if (!data->read(filepath)) {
      std::cerr << "Cannot read image file" << std::endl;
      return EXIT_FAILURE;
  }

  typedef itk::Image<double, 3> ImageType;

  ImageType::Pointer image = dynamic_cast<ImageType*>( (itk::Object*)(data->data()) );
  if (image.IsNull()) {
     std::cerr << "Cannot cast data() to ITK image" << std::endl;
     return EXIT_FAILURE;
  }
  
  image = dynamic_cast<ImageType*>( (itk::Object*)( data->output() ) );
  
  if (image.IsNull()) {
      std::cerr << "Cannot cast output() to ITK image" << std::endl; 
      return EXIT_FAILURE;
  }

  ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
  if ((int)size[0]!=data->xDimension()) {
      std::cerr << "Bad X dimension" << std::endl;
      return EXIT_FAILURE;
  }
  if ((int)size[1]!=data->yDimension()) {
      std::cerr << "Bad Y dimension" << std::endl;
      return EXIT_FAILURE;
  }
  if ((int)size[2]!=data->zDimension()) {
      std::cerr << "Bad Z dimension" << std::endl;
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

  dtkAbstractDataImage *data2 = dynamic_cast<dtkAbstractDataImage*>( dtkAbstractDataFactory::instance()->create ("itkDataImageDouble3") );
  if (!data2) {
      std::cerr << "Cannot create data object from plugin" << std::endl;
      return EXIT_FAILURE;
  }
  
  data2->setData ( image.GetPointer() );

  ImageType::Pointer image2 = dynamic_cast<ImageType*>( (itk::Object*)(data2->data()) );
  if (image2.IsNull()) {
     std::cerr << "Cannot cast data() to ITK image" << std::endl;
     return EXIT_FAILURE;
  }

  image2 = dynamic_cast<ImageType*>( (itk::Object*)(data2->output()) );
  if (image2.IsNull()) {
     std::cerr << "Cannot cast output() to ITK image" << std::endl;
     return EXIT_FAILURE;
  }

  if ((int)size2[0]!=data2->xDimension()) {
      std::cerr << "Bad X dimension" << std::endl;
      return EXIT_FAILURE;
  }
  if ((int)size2[1]!=data2->yDimension()) {
      std::cerr << "Bad Y dimension" << std::endl;
      return EXIT_FAILURE;
  }
  if ((int)size2[2]!=data2->zDimension()) {
      std::cerr << "Bad Z dimension" << std::endl;
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
