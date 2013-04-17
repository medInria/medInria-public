/*=========================================================================

 medInria

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
#include <vtkImageViewCornerAnnotation.h>
#include <vtkImageViewCollection.h>
#include <vtkCamera.h>
#include <vtksys/SystemTools.hxx>
#include <vtkMatrix4x4.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkPolyData.h>
#include "vtkUnstructuredGrid.h"
#include <vtkProperty.h>
#include <vtkErrorCode.h>
#include "vtkDataSetReader.h"
#include "vtkPointSet.h"
#include "vtkUnstructuredGridReader.h"
#include "vtkTensorVisuManager.h"
#include "vtkDataSetCollection.h"

#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkPropPicker.h>
#include <vtkCellPicker.h>
#include <vtkPicker.h>
#include <vtkDataManager.h>
#include <vtkTimerLog.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMultiThreader.h>
#include <vtkProp3DCollection.h>
#include <vtkLookupTableManager.h>

#include <vtkWindowToImageFilter.h>

#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>

#include "vtkCamera.h"

#ifdef vtkINRIA3D_USE_ITK
#include "itkTransformFileReader.h"
#include "itkTransformFileWriter.h"
#include "itkTranslationTransform.h"
#include <itkMatrixOffsetTransformBase.h>
#include <itkRigid3DTransform.h>
#include "itkTransformFactory.h"
#endif


// -----------------------------------------------------------------------------
class vtkMyCommand : public vtkCommand
{
  
public:
  
  static vtkMyCommand *New() {return new vtkMyCommand;};

  virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData))
  {
    
    if (event == vtkCommand::CharEvent)
    {
      vtkRenderWindowInteractor* rwi = vtkRenderWindowInteractor::SafeDownCast(caller);
      if (rwi->GetKeyCode() == 'y')
      {
	bool write_movie = 0;
	
	vtkTimerLog* TimerLog = vtkTimerLog::New();
	double absolutetime = 0.0;
	
	TimerLog->StartTimer();

	while( absolutetime < 30 )
	{
	  int id = Sequence->GetCurrentId();
	  if (id >= Sequence->GetNumberOfMetaDataSets()-1)
	    id = 0;
	  else
	    id = id+1;
	  Manager->UpdateSequencesToIndex (id);
	  TensorGlyph->UpdateLUT();
	  View->Render();

	  if (write_movie)
	  {
	    
	    std::ostringstream os;
	    os << Sequence->GetName() << "-" << 0;
	    unsigned int N = Sequence->GetNumberOfMetaDataSets();
	    if (N > 99)
	    {
	      if (id < 10) os << "0";
	      if (id <100) os << "0";
	    }
	    else if (N > 9)
	      if (id < 10) os << "0";
	    os << id << ".png";
	    
	    vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
	    filter->SetInput (View->GetRenderWindow());
	    filter->Update();
	    vtkPNGWriter* writer = vtkPNGWriter::New();
	    writer->SetInputConnection (filter->GetOutputPort());	  
	    writer->SetFileName (os.str().c_str());
	    writer->Write();
	    filter->Delete();
	    writer->Delete();
	  }
	  
	  TimerLog->StopTimer();
	  double delay = TimerLog->GetElapsedTime();
	  TimerLog->StartTimer();
	  
	  absolutetime += delay;
	  
	  
	}
	TimerLog->StopTimer();
	TimerLog->Delete();
      }

      if (rwi->GetKeyCode() == 'm')
      {
	int id = Sequence->GetCurrentId();
	if (id >= Sequence->GetNumberOfMetaDataSets()-1)
	  id = 0;
	else
	  id = id+1;
	Manager->UpdateSequencesToIndex (id);
	TensorGlyph->UpdateLUT();
	//TensorGlyph->GetActor()->Modified();
	//TensorGlyph->GetMapper()->Update();
	View->Render();
	
      }

      if (rwi->GetKeyCode() == 'g')
      {
	unsigned int newshape = TensorGlyph->GetShapeMode()+1;
	if (newshape > vtkTensorVisuManager::GLYPH_SUPERQUADRIC)
	  newshape = vtkTensorVisuManager::GLYPH_LINE;	
	TensorGlyph->SetGlyphShape (newshape);
	View->Render();
      }

      if (rwi->GetKeyCode() == '-')
      {
	double newscale = TensorGlyph->GetGlyphScale() - 0.5;
	// if (newscale < 0.1)
	//   newscale = 0;
	TensorGlyph->SetGlyphScale (newscale);
	View->Render();
      }

      if (rwi->GetKeyCode() == '+')
      {
	double newscale = TensorGlyph->GetGlyphScale() + 0.5;
	TensorGlyph->SetGlyphScale (newscale);
	View->Render();
      }
 
      if (rwi->GetKeyCode() == '/')
      {
	int newresolution = TensorGlyph->GetGlyphResolution() + 1;
	TensorGlyph->SetGlyphResolution (newresolution);
	View->Render();
      }
      if (rwi->GetKeyCode() == '*')
      {
	int newresolution = TensorGlyph->GetGlyphResolution() - 1;
	TensorGlyph->SetGlyphResolution (newresolution);
	View->Render();
      }


      
#ifdef vtkINRIA3D_USE_ITK
      if (rwi->GetKeyCode() == 's')
      {
	typedef itk::TransformFileWriter TransformWriterType;
	typedef itk::MatrixOffsetTransformBase<double, 3, 3> TransformType;
	
	typedef TransformType::ParametersType ParametersType;
	
	TransformType::Pointer transform = TransformType::New();
	ParametersType params;
	params.SetSize (12);
	TransformWriterType::Pointer writer = TransformWriterType::New();
	writer->SetInput (transform);
	
	std::cout<<"gathering orientation... "<<std::endl;
	vtkMatrix4x4* inv_orientation = vtkMatrix4x4::New();
	inv_orientation->Identity();
	
	// vtkMetaImageData* metaimage = this->MetaImage;
	vtkMetaImageData* metaimage = NULL;
	
	if (metaimage)
	{
	  vtkMatrix4x4* orientation = metaimage->GetOrientationMatrix();
	  orientation->Invert (orientation, inv_orientation);
	}
	
	vtkMatrix4x4* M = vtkMatrix4x4::New();
	if (metaimage)
	{
	  vtkProp3D* prop = NULL;
	  prop = vtkProp3D::SafeDownCast (this->View->GetExtraPlaneCollection()->GetItemAsObject (0));
	  M->DeepCopy (prop->GetUserMatrix());
	}
	else
	{ 
	  vtkProp3D* prop = NULL;
	  // prop = this->View->FindDataSetActor (this->View->GetDataSetCollection()->GetDataSet ((int)(1)));
	  prop = TensorGlyph->GetActor();
	  M->DeepCopy(prop->GetMatrix());
	}
	
	std::cout<<"estimating modification... "<<std::endl;
	
	vtkMatrix4x4* A = vtkMatrix4x4::New();
	vtkMatrix4x4::Multiply4x4 (M,inv_orientation, A);
	A->Print (std::cout);
	
	// saving only the translation
	unsigned int counter = 0;
	// for (unsigned int j=0; j<3; j++)
	//   params (counter++) = M->GetElement (j,3) - orientation1->GetElement (j,3);
	for (unsigned int j=0; j<3; j++)
	  for (unsigned int k=0; k<3; k++)
	    params (counter++) = A->GetElement (j,k);
	params (counter++) = A->GetElement (0, 3);
	params (counter++) = A->GetElement (1, 3);
	params (counter++) = A->GetElement (2, 3);
	
	std::cout<<"writing matrix... "<<std::endl;
	transform->SetParameters (params);
	std::ostringstream ostr;
	if (metaimage)
	  ostr << metaimage->GetName()<<"-transform.mat";
	else
	  ostr << "transform.mat";
	  
	std::cout<<"writing translation to : "<<ostr.str().c_str()<<std::endl;
	
	writer->SetFileName (ostr.str().c_str());
	writer->Update ();
	
	A->Delete();
	M->Delete();	
      }
#endif
           
	
    }
    
  }

  vtkImageView3D* View;
  vtkTensorVisuManager* TensorGlyph;
  vtkMetaDataSetSequence* Sequence;
  vtkDataManager* Manager;
  vtkMetaImageData* MetaImage;
  
 protected:
  vtkMyCommand()
  {
    this->View = 0;
    this->TensorGlyph = 0;
    this->Sequence = 0;
    this->Manager = 0;
    this->MetaImage = 0;
  };
  ~vtkMyCommand(){};

 private:
  
  
};


// -----------------------------------------------------------------------------
int main (int argc, char* argv[])
{

  int do_tensor_sequence = 0;
  
  if (argc < 2)
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <file1 file2 my-tensors.vtk ...>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk" << std::endl;
    exit (-1);
  }

  int position[2] = {0, 0};  
  vtkImageViewCollection* pool = vtkImageViewCollection::New();
  vtkDataManager* manager = vtkDataManager::New();

  vtkImageView3D* view3d = vtkImageView3D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin = vtkRenderWindow::New();
  vtkRenderer* renderer = vtkRenderer::New();
  iren->SetRenderWindow (rwin);
  rwin->AddRenderer (renderer);
  view3d->SetRenderWindow ( rwin );
  view3d->SetRenderer ( renderer );
  double color[3]={0.9,0.9,0.9};
  view3d->SetBackground (color);
  position[0] = 400; position[1] = 420;
  view3d->SetPosition (position);
  view3d->SetShowActorX (0);
  view3d->SetShowActorY (0);
  view3d->SetShowActorZ (0);

  
  pool->AddItem (view3d);

  vtkTensorVisuManager* tensormanager = vtkTensorVisuManager::New();
  vtkMyCommand* command = vtkMyCommand::New();
  command->View = view3d;
  command->TensorGlyph = tensormanager;
  view3d->GetInteractor()->AddObserver(vtkCommand::CharEvent, command);
  
  for (int i=0; i<argc - 2; i++)
  {
    std::string filename = argv[i+1];

    vtkMetaDataSet* metadataset = manager->ReadFile(filename.c_str());
    // manager->AddMetaDataSet (metadataset);

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

      command->MetaImage = metaimage;
      
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
      view->GetCornerAnnotation()->SetText (0, filename.c_str());

      view3d->AddExtraPlane (view->GetImageActor());

      pool->AddItem (view);
    }
    else
    {
      vtkProperty* prop = vtkProperty::SafeDownCast( metadataset->GetProperty() );
      prop->SetColor (0.5,0.5,0.5);
      if (vtkPointSet::SafeDownCast (metadataset->GetDataSet())->GetNumberOfPoints() > 10000)
	prop->SetOpacity (0.25);
      else
      {
	prop->SetOpacity (0.60);
	prop->SetLineWidth (1);
      }
	
      pool->SyncAddDataSet( vtkPointSet::SafeDownCast (metadataset->GetDataSet()), prop );
      metadataset->SetScalarVisibility(1);
    }

  }

  
  vtkImageView* firstview = pool->GetItem (0);

  if (firstview)
  {
    view3d->SetInput (firstview->GetInput());
    view3d->SetOrientationMatrix(firstview->GetOrientationMatrix());
  }

  std::string filename = argv[argc - 1];

  vtkUnstructuredGrid* mytensors = 0;
  
  if (do_tensor_sequence)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::New();
    sequence->Read (filename.c_str());
    mytensors = vtkUnstructuredGrid::SafeDownCast (sequence->GetDataSet());
    tensormanager->SetGlyphScale (5.0);
    sequence->SetSequenceDuration (1.0);
    command->Sequence = sequence;
    command->Manager = manager;
    manager->AddMetaDataSet (sequence);
  }
  else
  {
    vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New();
    reader->SetFileName (filename.c_str());
    reader->Update();
    mytensors = reader->GetOutput();
    tensormanager->SetGlyphScale (1);
    tensormanager->SetGlyphResolution (14);
    tensormanager->SetGlyphShapeToArrow();
  }
  
  tensormanager->SetInput (mytensors);
  // tensormanager->SetLookupTable (vtkLookupTableManager::GetJetLookupTable());
  
  // tensormanager->SetColorModeToVolume();
  
  view3d->GetRenderer()->AddViewProp (tensormanager->GetActor());
  view3d->GetRenderer()->Modified();

  pool->SyncReset();
  pool->SyncSetShowAnnotations (1);
  pool->SyncSetShowScalarBar (0);
  pool->SyncSetShowRulerWidget (0);
  pool->SyncSetShowImageAxis (1);
  pool->SetLinkColorWindowLevel (0);
  
  pool->SyncSetAnnotationStyle (vtkImageView2D::AnnotationStyle2);
  pool->SyncSetWheelInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeSlice);
  //pool->SyncSetLeftButtonInteractionStyle(vtkInteractorStyleImageView2D::InteractionTypeTime);
  int size[2]={370, 370};
  pool->SyncSetSize (size);
  pool->SyncRender();
  
  // pool->SyncSetUseLookupTable (1);
  
  pool->SyncStart();
  
  view3d->Delete();
  manager->Delete();
  command->Delete();
  
  return 0;
}
