#include <QtGui>

#include "medItk/medItk.h"

#include "dtkCore/dtkPluginManager.h"
#include "dtkCore/dtkAbstractDataFactory.h"
#include <dtkCore/dtkAbstractDataReader.h>
#include "dtkCore/dtkAbstractData.h"
#include "dtkCore/dtkAbstractViewFactory.h"
#include "dtkCore/dtkAbstractView.h"


int v3dViewTest(int argc, char *argv[])
{
  if (argc<3) {
      std::cerr << "Not enough arguments" << std::endl;
      return EXIT_FAILURE;
  }


  QApplication application(argc, argv);

    
  // the test file
  QString filepath = argv[2];
  filepath += "/BrainProtonDensitySliceBorder20.mhd";
  
  dtkPluginManager::instance()->setPath (argv[1]);
  dtkPluginManager::instance()->initialize();

  dtkAbstractData *data = dtkAbstractDataFactory::instance()->create ("itkDataImageDouble3");
  
  if (!data) {
      std::cerr << "Cannot create data object from plugin" << std::endl;
      return EXIT_FAILURE;
  }

  data->enableReader ("itkMetaDataImageReader");
  
  if (!data->read(filepath)) {
      std::cerr << "Cannot read image file" << std::endl;
      return EXIT_FAILURE;
  }

  dtkAbstractView *view = dtkAbstractViewFactory::instance()->create ("v3dView");
  if (!view) {
      std::cerr << "Cannot create view object from plugin" << std::endl;
      return EXIT_FAILURE;
  }
  

  QMainWindow window;  
  window.setCentralWidget ( view->widget() );
  window.show();
  window.resize (128,128);


  view->setData ( data );
  view->reset();
  view->update();


  view->setProperty ("Orientation", "Axial"); view->update();
  view->setProperty ("Orientation", "Sagittal"); view->update();
  view->setProperty ("Orientation", "Coronal"); view->update();
  view->setProperty ("Orientation", "3D"); view->update();
  view->setProperty ("Mode", "VR"); view->update();
  view->setProperty ("Cropping", "true"); view->update();
  view->setProperty ("Cropping", "false"); view->update();
  view->setProperty ("Mode", "MPR"); view->update();
  view->setProperty ("Mode", "MIP - Maximum"); view->update();
  view->setProperty ("Mode", "MIP - Minimum"); view->update();
  view->setProperty ("Orientation", "Axial"); view->update();
  view->setProperty ("ScalarBarVisibility", "true"); view->update();
  view->setProperty ("ScalarBarVisibility", "false"); view->update();
  view->setProperty ("LookupTable", "Black&White"); view->update();
  view->setProperty ("LookupTable", "Black&WhiteInversed"); view->update();
  view->setProperty ("LookupTable", "Spectrum"); view->update();
  view->setProperty ("LookupTable", "HotMetal"); view->update();
  view->setProperty ("LookupTable", "GE"); view->update();
  view->setProperty ("LookupTable", "Flow"); view->update();
  view->setProperty ("LookupTable", "Loni"); view->update();
  view->setProperty ("LookupTable", "Loni2"); view->update();
  view->setProperty ("LookupTable", "Asymmetry"); view->update();
  view->setProperty ("LookupTable", "PValue"); view->update();
  view->setProperty ("LookupTable", "blueBlackAlpha"); view->update();
  view->setProperty ("LookupTable", "greenBlackAlpha"); view->update();
  view->setProperty ("LookupTable", "redBlackAlpha"); view->update();
  view->setProperty ("LookupTable", "Muscles&Bones"); view->update();
  view->setProperty ("LookupTable", "Red Vessels"); view->update();
  view->setProperty ("LookupTable", "Bones"); view->update();
  view->setProperty ("LookupTable", "Stern"); view->update();
  view->setProperty ("Preset", "VR Muscles&Bones"); view->update();
  view->setProperty ("Preset", "Vascular I"); view->update();
  view->setProperty ("Preset", "Vascular II"); view->update();
  view->setProperty ("Preset", "Vascular III"); view->update();
  view->setProperty ("Preset", "Vascular IV"); view->update();
  view->setProperty ("Preset", "Standard"); view->update();
  view->setProperty ("Preset", "Soft"); view->update();
  view->setProperty ("Preset", "Soft on White"); view->update();
  view->setProperty ("Preset", "Soft on Blue"); view->update();
  view->setProperty ("Preset", "Red on White"); view->update();
  view->setProperty ("Preset", "Glossy"); view->update();
  view->setProperty ("ShowAxis" ,"true"); view->update();
  view->setProperty ("ShowAxis" ,"false"); view->update();

  return EXIT_SUCCESS; //application.exec();
  
}
