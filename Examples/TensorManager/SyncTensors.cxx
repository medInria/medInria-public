/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: SynchronizedViews.cxx 917 2008-08-27 10:37:34Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2008-08-27 12:37:34 +0200 (Wed, 27 Aug 2008) $
Version:   $Revision: 917 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageActor.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkViewImage3D.h>
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
#include "vtkUnstructuredGridReader.h"
#include "vtkTensorVisuManager.h"

#include <vtkImageData.h>
#include <vtkCommand.h>
#include <vtkPropPicker.h>
#include <vtkCellPicker.h>
#include <vtkPicker.h>
#include <vtkDataManager.h>
#include <vtkTimerLog.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkMultiThreader.h>


#include <vtkWindowToImageFilter.h>

#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>

#include "vtkCamera.h"


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
	if (newscale < 0.1)
	  newscale = 0;
	TensorGlyph->SetGlyphScale (newscale);
	View->Render();
      }

      if (rwi->GetKeyCode() == '+')
      {
	double newscale = TensorGlyph->GetGlyphScale() + 0.5;
	TensorGlyph->SetGlyphScale (newscale);
	View->Render();
      }
      
	
    }
    
  }

  vtkViewImage* View;
  vtkTensorVisuManager* TensorGlyph;
  vtkMetaDataSetSequence* Sequence;
  vtkDataManager* Manager;
  
 protected:
  vtkMyCommand()
  {
  };
  ~vtkMyCommand(){};

 private:
  
  
};


// -----------------------------------------------------------------------------
int main (int argc, char* argv[])
{

  int do_tensor_sequence = 1;
  
  if (argc < 2)
  {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t" << argv[0] << " <file1 file2 my-tensors.vtk ...>" << std::endl;
    std::cout << "Example: " << std::endl;
    std::cout << "\t" << argv[0] << " [vtkINRIA3D_DATA_DIR]/MRI.vtk" << std::endl;
    exit (-1);
  }

  vtkDataManager* manager = vtkDataManager::New();

  vtkViewImage3D* view3d = vtkViewImage3D::New();
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin = vtkRenderWindow::New();
  vtkRenderer* renderer = vtkRenderer::New();
  iren->SetRenderWindow (rwin);
  rwin->AddRenderer (renderer);
  view3d->SetRenderWindow ( rwin );
  view3d->SetRenderer ( renderer );
  
  for (int i=0; i<argc - 2; i++)
  {
    std::string filename = argv[i+1];

    vtkMetaDataSet* metadataset = manager->ReadFile(filename.c_str());

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

      view3d->SetImage (image);
    }
    else
    {
      vtkProperty* prop = vtkProperty::SafeDownCast( metadataset->GetProperty() );
      prop->SetColor (0.3,0.3,0.3);
      prop->SetOpacity (0.15);
      view3d->AddDataSet( metadataset->GetDataSet(), prop );
    }

  }

  std::string filename = argv[argc - 1];

  vtkUnstructuredGrid* mytensors = 0;
  vtkTensorVisuManager* tensormanager = vtkTensorVisuManager::New();
  vtkMyCommand* command = vtkMyCommand::New();
  command->View = view3d;
  command->TensorGlyph = tensormanager;
  
  if (do_tensor_sequence)
  {
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::New();
    sequence->Read (filename.c_str());
    mytensors = vtkUnstructuredGrid::SafeDownCast (sequence->GetDataSet());
    tensormanager->SetGlyphScale (20.0);
    sequence->SetSequenceDuration (1.0);
    //std::cout<<"sequence : "<<(*sequence)<<std::endl;
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
    tensormanager->SetGlyphScale (3.5);
    tensormanager->SetGlyphShapeToArrow();
  }

  
  
  tensormanager->SetInput (mytensors);
  
  view3d->GetRenderer()->AddViewProp (tensormanager->GetActor());
  view3d->GetRenderer()->Modified();
  // int size[2] = {600, 600};
  // view3d->SetSize (size);
  // int vposition[2] = {400, 400 };
  // view3d->SetPosition (vposition);  
  view3d->GetRenderWindowInteractor()->AddObserver(vtkCommand::CharEvent, command);

  view3d->GetRenderWindowInteractor()->Start();
  
  view3d->Delete();
  manager->Delete();
  command->Delete();
  
  return 0;
}
