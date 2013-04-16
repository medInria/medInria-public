/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageActor.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkCamera.h>
#include <vtksys/SystemTools.hxx>
#include <vtkMatrix4x4.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkErrorCode.h>
#include <vtkImageViewCollection.h>
#include "vtkDataSetReader.h"
#include <vtkLookupTableManager.h>

#ifdef ITK_USE_REVIEW
#include <itkPhilipsRECImageIOFactory.h>
#endif

#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkPropPicker.h>
#include <vtkCellPicker.h>
#include <vtkPicker.h>
#include <vtkDataManager.h>
#include <vtkTimerLog.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMultiThreader.h>
#include <vtkImageViewCornerAnnotation.h>
#include <vtkInteractorStyleImageView2D.h>

#include <vtkImageViewCollection.h>

// -----------------------------------------------------------------------------
class vtkMyCommand : public vtkCommand
{
  
public:
  
  static vtkMyCommand *New() {return new vtkMyCommand;};

  void SetViewer (vtkImageView3D* viewer)
  { this->Viewer = viewer; };
  void SetManager (vtkDataManager* manager)
  { this->Manager = manager; };
  void SetCollection (vtkImageViewCollection* arg)
  { this->Collection = arg; };
  void AddSequence (vtkMetaDataSetSequence* sequence)
  { this->Sequences.push_back (sequence); };
  
  virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData))
  {
    
    if (event == vtkCommand::CharEvent)
    {
      vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
      if (rwi->GetKeyCode() == 'v')
      {
        this->Viewer->SetRenderingMode (!this->Viewer->GetRenderingMode());
	this->Viewer->Render();
      }

      if (rwi->GetKeyCode() == 'y')
      {
      	if (!this->Sequences.size())
      	  return;
      	
	int id = this->Sequences[0]->GetCurrentId();
	if (id >= this->Sequences[0]->GetNumberOfMetaDataSets() - 1)
	  id = 0;
	else
	  id = id+1;
	for (unsigned int i=0; i<this->Sequences.size(); i++)
	  this->Sequences[i]->UpdateToIndex (id);
	this->Collection->SyncRender();
	
      }
      if (rwi->GetKeyCode() == 'z')
      {
	this->Collection->SyncSetViewOrientation (orientation++);
	if (orientation > 2) orientation = 0;
	this->Collection->SyncRender();
      }
    }
  }
  

 protected:
  vtkMyCommand()
  {
    toggle = 1;
    orientation = 0;
    this->Viewer = NULL;
    this->Collection = NULL;
  };
  ~vtkMyCommand(){};

 private:
  vtkImageView3D* Viewer;
  vtkDataManager* Manager;
  vtkImageViewCollection* Collection;
  
  std::vector<vtkMetaDataSetSequence*> Sequences;
  bool toggle;
  unsigned int orientation;
};


// -----------------------------------------------------------------------------
int main (int argc, char* argv[])
{

  int do_multi_planes  = 0;
  int do_cardiac_style = 1;

#ifdef ITK_USE_REVIEW
  itk::PhilipsRECImageIOFactory::RegisterOneFactory();
#endif
  
  if (argc < 2)
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <file1 file2 file3 ...>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk" << std::endl;
    exit (-1);
  }


  int position[2] = {0, 0};  
  
  unsigned int doSequence = 0;
  
  vtkImageViewCollection*      pool = vtkImageViewCollection::New();
  vtkDataManager*           manager = vtkDataManager::New();
  vtkImageView3D*            view3d = vtkImageView3D::New();
  vtkRenderWindowInteractor* iren3d = vtkRenderWindowInteractor::New();
  vtkRenderWindow*           rwin3d = vtkRenderWindow::New();
  vtkRenderer*                ren3d = vtkRenderer::New();

  position[0] = 400; position[1] = 420;
  
  iren3d->SetRenderWindow(rwin3d);
  rwin3d->AddRenderer (ren3d);

  view3d->SetRenderWindow(rwin3d);
  view3d->SetRenderer(ren3d);
  view3d->SetPosition (position);
  double color[3]={0.9,0.9,0.9};
  view3d->SetBackground (color);
  view3d->SetVolumeMapperToGPU();
  
  if (!do_cardiac_style)
  {
    view3d->SetShowActorX(do_multi_planes);
    view3d->SetShowActorY(do_multi_planes);
    view3d->SetShowActorZ(1);
  }
  else
  {
    view3d->SetShowActorX (0);
    view3d->SetShowActorY (0);
    view3d->SetShowActorZ (0);
  }
  
  vtkMyCommand* command = vtkMyCommand::New();
  command->SetViewer (view3d);
  command->SetCollection (pool);
  
  view3d->GetInteractor()->AddObserver(vtkCommand::CharEvent, command);

  pool->AddItem (view3d);
  
  for (int i=1; i<argc; i++)
  {
    std::string filename = argv[i];

    vtkMetaDataSet* metadataset = NULL;

    try
    {
      metadataset = manager->ReadFile(filename.c_str());
    } catch (...)
    {
      // not a metadataset...
    }
    
    if ( metadataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA )
    {
      vtkMetaImageData* metaimage = vtkMetaImageData::SafeDownCast(metadataset);
      vtkImageData* image = NULL;
      vtkMatrix4x4* matrix = NULL;
      
      if (!metaimage)
      {
	metaimage = vtkMetaImageData::SafeDownCast(vtkMetaDataSetSequence::SafeDownCast(metadataset)->GetMetaDataSet(0));
	image = vtkImageData::SafeDownCast(vtkMetaDataSetSequence::SafeDownCast(metadataset)->GetDataSet());
      }
      else
	image = metaimage->GetImageData();
      
      matrix = metaimage->GetOrientationMatrix();
      
      vtkImageView2D* view = vtkImageView2D::New();
      vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
      vtkRenderWindow* rwin = vtkRenderWindow::New();
      vtkRenderer* ren = vtkRenderer::New();
      position[0] = 0; position[1] = 0;
      iren->SetRenderWindow(rwin);
      rwin->AddRenderer (ren);
      
      view->SetRenderWindow(rwin);
      view->SetRenderer(ren);
      view->SetPosition (position);
      view->SetInput (image);
      view->SetOrientationMatrix(matrix);
      view->GetCornerAnnotation()->SetText (1, filename.c_str());
      
      if (do_cardiac_style)
      {
	view3d->AddExtraPlane (view->GetImageActor());
      }
      
      pool->AddItem (view);

      view->Delete();
      iren->Delete();
      ren->Delete();
      rwin->Delete();
      
      if (do_multi_planes)
      {
	view->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_AXIAL);
	
	vtkImageView2D* view2 = vtkImageView2D::New();
	vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
	vtkRenderWindow* rwin2 = vtkRenderWindow::New();
	vtkRenderer* ren2 = vtkRenderer::New();
	position[0] = 400; position[1] = 0;
	
	iren2->SetRenderWindow(rwin2);
	rwin2->AddRenderer (ren2);
	
	view2->SetRenderWindow(rwin2);
	view2->SetRenderer(ren2);
	view2->SetPosition (position);
	view2->SetInput (image);
	view2->SetOrientationMatrix(matrix);
	view2->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_SAGITTAL);
	view2->GetCornerAnnotation()->SetText (0, filename.c_str());
	pool->AddItem (view2);
	view2->Delete();
	iren2->Delete();
	rwin2->Delete();
	ren2->Delete();
	
	
	vtkImageView2D* view3 = vtkImageView2D::New();
	vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
	vtkRenderWindow* rwin3 = vtkRenderWindow::New();
	vtkRenderer* ren3 = vtkRenderer::New();
	position[0] = 0; position[1] = 420;
	
	iren3->SetRenderWindow(rwin3);
	rwin3->AddRenderer (ren3);
	
	view3->SetRenderWindow(rwin3);
	view3->SetRenderer(ren3);
	view3->SetPosition (position);
	view3->SetInput (image);
	view3->SetOrientationMatrix(matrix);
	view3->SetViewOrientation (vtkImageView2D::VIEW_ORIENTATION_CORONAL);
	view3->GetCornerAnnotation()->SetText (0, filename.c_str());
	
	if (do_cardiac_style)
	{
	  view3d->AddExtraPlane (view2->GetImageActor());
	  view3d->AddExtraPlane (view3->GetImageActor());
	}
	
	pool->AddItem (view3);
	
	view3->Delete();
	iren3->Delete();
	rwin3->Delete();
	ren3->Delete();
	
      }
      
    }
    else
    {
      vtkProperty* prop = vtkProperty::SafeDownCast( metadataset->GetProperty() );
      prop->SetColor (0.5,0.5,0.5);
      prop->SetLineWidth (2);
      if (vtkPointSet::SafeDownCast (metadataset->GetDataSet())->GetNumberOfPoints() > 10000)
	prop->SetOpacity (0.9);
      pool->SyncAddDataSet( vtkPointSet::SafeDownCast (metadataset->GetDataSet()), prop);
      metadataset->AddActor (vtkActor::SafeDownCast (view3d->FindDataSetActor (metadataset->GetDataSet())));
      
      vtkDataArray* array = metadataset->GetArray ("Sequence_Scalars");
      if (array)
      {
	if (array->GetLookupTable())
	{
	  std::cout<<"array "<<"Sequence_Scalars"<<" have a lut :"<<(*array->GetLookupTable())<<std::endl;
	}
	else
	  array->SetLookupTable (vtkLookupTableManager::GetLONILookupTable());
	metadataset->ColorByArray (array);
      }
      else
	std::cout<<"no array called "<<"Sequence_Scalars"<<std::endl;
    }
    
    
    if (vtkMetaDataSetSequence::SafeDownCast(metadataset))
    {
      std::cout<<"setting the sequence : "<<filename.c_str()<<std::endl;
      command->AddSequence (vtkMetaDataSetSequence::SafeDownCast (metadataset));
      doSequence++;
    }
  }

  vtkImageView2D* firstview = vtkImageView2D::SafeDownCast (pool->GetItem (1));
  
  if (firstview)
  {
    view3d->SetInput (firstview->GetInput());
    view3d->SetOrientationMatrix(firstview->GetOrientationMatrix());
  }

  pool->SyncReset();
  pool->SyncSetShowAnnotations (1);
  pool->SyncSetShowRulerWidget (0);
  pool->SyncSetShowImageAxis (0);
  pool->SyncSetShowScalarBar (0);
  
  pool->SyncSetAnnotationStyle (vtkImageView2D::AnnotationStyle2);
  pool->SyncSetViewConvention (vtkImageView2D::VIEW_CONVENTION_RADIOLOGICAL);
  
  pool->LinkColorWindowLevelOn ();
  pool->LinkSliceMoveOn();
  
  
  pool->SyncSetWheelInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeSlice);
  //pool->SyncSetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeTime);
  
  int size[2]={370, 370};
  pool->SyncSetSize (size);
  pool->SyncRender();
  
  
  pool->SyncStart();
  
  view3d->Delete();
  iren3d->Delete();
  rwin3d->Delete();
  ren3d->Delete();
  
  manager->Delete();
  pool->Delete();
  command->Delete();
  
  return 0;
}
