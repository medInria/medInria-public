/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkViewImage3D.h>
#include <vtkRenderingAddOn/vtkViewImage2D.h>

#include <vtkDataManager.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaDataSetSequence.h>
#include <vtksys/SystemTools.hxx>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>

#include <vtkTimerLog.h>
#include <vtkDirectory.h>
#include <vtkProperty.h>

#include <vtkActor.h>
#include <vtkLookupTableManager.h>

#include <algorithm>
#include <vtkErrorCode.h>

#ifdef vtkINRIA3D_USE_ITK
#include <itkPhilipsRECImageIOFactory.h>
#endif
int main (int argc, char*argv[])
{

  
#ifdef vtkINRIA3D_USE_ITK
  itk::PhilipsRECImageIOFactory::RegisterOneFactory();
#endif
  

  if( argc<3 )
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " [-dir <Data Directory1 Directory2 ...>] or [-seq <Sequence-Directory1 Sequence-Directory2 ...>]" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " -seq  [vtkINRIA3D_DATA_DIR]/Sequences/HeartImages/ [vtkINRIA3D_DATA_DIR]/Sequences/HeartMeshes/" << std::endl;
    exit (-1);
  }

  std::vector<std::string> directorylist;
  std::vector<std::string> sequencedirectorylist;

  if ( strcmp(argv[1], "-dir")==0 )
  {
    for( int i=2; i<argc; i++)
    {
      directorylist.push_back (argv[i]);
    }
  }
  else if (strcmp(argv[1], "-seq")==0)
  {
    for( int i=2; i<argc; i++)
    {
      sequencedirectorylist.push_back (argv[i]);
    }
  }
  else
  {
    std::cout << "Error: Argument list is not recognized." << std::endl;
    exit (-1);
  }


  /**
     In this example, we illustrate the use of the vtkDataSetManager.
     We first set up a vtkImageView3D to display an image(s)/mesh(es) of the manager.
  */

  vtkViewImage3D* view            = vtkViewImage3D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin           = vtkRenderWindow::New();
  vtkRenderer* renderer           = vtkRenderer::New();

  iren->SetRenderWindow (rwin);
  rwin->AddRenderer (renderer);
  view->SetRenderWindow ( rwin );
  view->SetRenderer ( renderer );

  rwin->SetSize (250,250);
  rwin->SetPosition (0,0);

  double color[3] = {0.0,0.0,0.0};
  view->SetTextColor (color);
  view->SetRenderingModeToPlanar();
  view->SetCubeVisibility(1);
  view->SetAboutData ("Powered by vtkINRIA3D");

  vtkViewImage2D* view2            = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin2           = vtkRenderWindow::New();
  vtkRenderer* renderer2           = vtkRenderer::New();

  vtkViewImage2D* view3            = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin3           = vtkRenderWindow::New();
  vtkRenderer* renderer3           = vtkRenderer::New();

  vtkViewImage2D* view4            = vtkViewImage2D::New();
  vtkRenderWindowInteractor* iren4 = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin4           = vtkRenderWindow::New();
  vtkRenderer* renderer4           = vtkRenderer::New();

  iren2->SetRenderWindow (rwin2);
  rwin2->AddRenderer (renderer2);
  view2->SetRenderWindow ( rwin2 );
  view2->SetRenderer ( renderer2 );
  view2->SetAboutData ("Powered by vtkINRIA3D");
  view2->SetBackgroundColor (0.0,0.0,0.0);
  view2->SetOrientation (vtkViewImage2D::AXIAL_ID);

  rwin2->SetSize (250,250);
  rwin2->SetPosition (250,0);


  iren3->SetRenderWindow (rwin3);
  rwin3->AddRenderer (renderer3);
  view3->SetRenderWindow ( rwin3 );
  view3->SetRenderer ( renderer3 );
  view3->SetAboutData ("Powered by vtkINRIA3D");
  view3->SetBackgroundColor (0.0,0.0,0.0);
  view3->SetOrientation (vtkViewImage2D::SAGITTAL_ID);

  rwin3->SetSize (250,250);
  rwin3->SetPosition (0,250);


  iren4->SetRenderWindow (rwin4);
  rwin4->AddRenderer (renderer4);
  view4->SetRenderWindow ( rwin4 );
  view4->SetRenderer ( renderer4 );
  view4->SetAboutData ("Powered by vtkINRIA3D");
  view4->SetBackgroundColor (0.0,0.0,0.0);
  view4->SetOrientation (vtkViewImage2D::CORONAL_ID);

  rwin4->SetSize (250,250);
  rwin4->SetPosition (250,250);


  view->AddChild (view2);
  view2->AddChild (view3);
  view3->AddChild (view4);
  view4->AddChild (view);

  // zooming is not linked by default
  //view2->SetLinkZoom (1);
  //view3->SetLinkZoom (1);
  //view4->SetLinkZoom (1);



  vtkDataManager* DataManager = vtkDataManager::New();

  if (directorylist.size())
  {
    for( unsigned int i=0; i<directorylist.size(); i++)
    {
      try
      {
        DataManager->ScanDirectory( directorylist[i].c_str() );
      }
      catch (vtkErrorCode::ErrorIds)
      {
        std::cout<<"error caught when scanning"<<std::endl;

      }
    }
  }

  bool do_sequences = false;
  if ( sequencedirectorylist.size() )
  {
    do_sequences = true;
    for( unsigned int i=0; i<sequencedirectorylist.size(); i++)
    {
      try
      {
        DataManager->ScanDirectoryForSequence( sequencedirectorylist[i].c_str(), 2.5);
      }
      catch (vtkErrorCode::ErrorIds)
      {
        std::cout<<"error caught when scanning"<<std::endl;



	DataManager->Delete();

	view->Detach();
	view2->Detach();
	view3->Detach();
	view4->Detach();

	view->Delete();
	iren->Delete();
	rwin->Delete();
	renderer->Delete();

	view2->Delete();
	iren2->Delete();
	rwin2->Delete();
	renderer2->Delete();

	view3->Delete();
	iren3->Delete();
	rwin3->Delete();
	renderer3->Delete();

	view4->Delete();
	iren4->Delete();
	rwin4->Delete();
	renderer4->Delete();

	return -1;


      }
    }
  }


  /** Create an empty image of the same extent as the datasets. */
  vtkDataSet* dataset = DataManager->GetMetaDataSet( (unsigned int)(0))->GetDataSet();
  if( dataset )
  {

    double* bb = dataset->GetBounds();

    int dims[3];
    dims[0] = (int)(bb[1]-bb[0]+20);
    dims[1] = (int)(bb[3]-bb[2]+20);
    dims[2] = (int)(bb[5]-bb[4]+20);

    int extent[6]={0, dims[0]-1, 0, dims[1]-1, 0, dims[2]-1};

    vtkImageData* image = vtkImageData::New();
    image->Initialize();
    image->SetOrigin (bb[0]-10,bb[2]-10,bb[4]-10);
    image->SetSpacing (1.0, 1.0, 1.0);
    image->SetDimensions ( dims );
    image->SetWholeExtent (extent);
    image->SetScalarTypeToFloat();
    image->AllocateScalars();


    view->SetImage (image);
    view2->SetImage (image);
    view3->SetImage (image);
    view4->SetImage (image);

    image->Delete();

  }



  for (unsigned int i=0; i<DataManager->GetNumberOfMetaDataSet(); i++)
  {
    vtkMetaDataSet* metadataset = DataManager->GetMetaDataSet (i);

    vtkProperty* prop = vtkProperty::SafeDownCast( metadataset->GetProperty() );

    vtkActor* act1 = view->AddDataSet ( metadataset->GetDataSet(), prop );
    metadataset->AddActor ( act1 );

    vtkProperty* prop2 = vtkProperty::New();
    prop2->DeepCopy (prop);
    prop2->SetAmbient (1.0);
    prop2->SetDiffuse (0.0);
    prop2->SetSpecular (0.0);


    vtkActor* act2 = view2->AddDataSet (metadataset->GetDataSet(), prop2);
    metadataset->AddActor ( act2 );
    prop2->Delete();


    vtkActor* act3 = view3->AddDataSet (metadataset->GetDataSet(), prop2);
    metadataset->AddActor ( act3 );


    vtkActor* act4 = view4->AddDataSet (metadataset->GetDataSet(), prop2);
    metadataset->AddActor ( act4 );


    if (vtkMetaDataSetSequence::SafeDownCast(metadataset))
    {
      vtkDataArray* array = metadataset->GetArray("Sequence_Scalars");
      if (array)
      {
	metadataset->ColorByArray(array);
      }
    }
  }

  view->SyncResetWindowLevel();
  view->SyncResetCurrentPoint();


  rwin->Render();
  rwin2->Render();
  rwin3->Render();
  rwin4->Render();


  if (do_sequences)
  {

    vtkTimerLog* TimerLog = vtkTimerLog::New();

    double mintime = DataManager->GetSequencesRangeMin();
    double maxtime = DataManager->GetSequencesRangeMax();
    //int max_number = DataManager->GetSequencesMaxNumber();
    double time = mintime;
    double absolutetime = 0.0;

    TimerLog->StartTimer();

    while( absolutetime < 10 )
    {


      DataManager->UpdateSequencesToTime(time);
      view->Render();
      view2->Render();
      view3->Render();
      view4->Render();


      if (time > maxtime)
      {
        time = mintime;
      }


      TimerLog->StopTimer();
      double delay = TimerLog->GetElapsedTime();
      TimerLog->StartTimer();

      time         += delay;
      absolutetime += delay;

    }

    TimerLog->StopTimer();


    TimerLog->Delete();
  }


  iren2->Initialize();
  iren3->Initialize();
  iren4->Initialize();
  iren->Initialize();

  iren2->Start();
  iren3->Start();
  iren4->Start();
  iren->Start();

  DataManager->Delete();

  view->Detach();
  view2->Detach();
  view3->Detach();
  view4->Detach();

  view->Delete();
  iren->Delete();
  rwin->Delete();
  renderer->Delete();

  view2->Delete();
  iren2->Delete();
  rwin2->Delete();
  renderer2->Delete();

  view3->Delete();
  iren3->Delete();
  rwin3->Delete();
  renderer3->Delete();

  view4->Delete();
  iren4->Delete();
  rwin4->Delete();
  renderer4->Delete();


  return 0;
}
