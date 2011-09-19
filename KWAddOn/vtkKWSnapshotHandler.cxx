/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSnapshotHandler.cxx 572 2007-12-21 15:52:58Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-21 16:52:58 +0100 (Fri, 21 Dec 2007) $
Version:   $Revision: 572 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWSnapshotHandler.h"
#include "vtkObjectFactory.h"
#include "vtkKWRenderWidget.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkKWRadioButtonSet.h>
#include <vtkKWRadioButton.h>

#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkKWCameraAnimationWidget.h>
#include <vtkKWTimeAnimationWidget.h>

#include <vtkWindowToImageFilter.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkTIFFWriter.h>
#include <vtkPNGWriter.h>
#include <sstream>

#include <vtksys/SystemTools.hxx>
#include <vtksys/stl/string>

#include <vtkKWLoadSaveDialog.h>
#include <kwcommon.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWSnapshotHandler );
vtkCxxRevisionMacro( vtkKWSnapshotHandler, "$Revision: 1 $");


//----------------------------------------------------------------------------
vtkKWSnapshotHandler::vtkKWSnapshotHandler()
{
  this->CameraAnimationWidget = vtkKWCameraAnimationWidget::New();
  this->TimeAnimationWidget = vtkKWTimeAnimationWidget::New();
  this->ButtonFrame = vtkKWFrame::New();
  this->AnimationFrame = vtkKWFrame::New();
  this->SetSnapshotType (vtkKWSnapshotHandler::SNAPSHOTTYPE_TIME);
  this->RadioButtonSlice = NULL;
  this->RadioButtonCamera = NULL;
  this->RadioButtonTime = NULL;
  this->RenderWidget = NULL;
  this->View = NULL;

  this->AllowSliceType = 1;
  this->AllowCameraType = 1;
  this->AllowTimeType = 1;
  
}

//----------------------------------------------------------------------------
vtkKWSnapshotHandler::~vtkKWSnapshotHandler()
{
  std::cout<<"deleting vtkKWSnapshotHandler "<<std::endl;

  this->ButtonFrame->Delete();
  this->AnimationFrame->Delete();

  this->CameraAnimationWidget->SetRenderWidget (NULL);
  this->TimeAnimationWidget->SetRenderWidget (NULL);
  
  this->CameraAnimationWidget->Delete();
  this->TimeAnimationWidget->Delete();
}


//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::Update (void)
{

  if (!this->IsCreated())
  {
    return;
  }
  
  if (this->AllowSliceType)
    this->RadioButtonSlice->EnabledOn();
  else
    this->RadioButtonSlice->EnabledOff();

  if (this->AllowCameraType)
    this->RadioButtonCamera->EnabledOn();
  else
    this->RadioButtonCamera->EnabledOff();

  if (this->AllowTimeType)
    this->RadioButtonTime->EnabledOn();
  else
    this->RadioButtonTime->EnabledOff();

  this->UnpackChildren();
  
  this->Script ("pack %s -side top -anchor nw -fill x -expand 1",
		this->ButtonFrame->GetWidgetName());

  switch (this->GetSnapshotType())
  {
      case SNAPSHOTTYPE_SLICE :
	this->Script("pack %s -side top -anchor nw -fill both -expand 1",
		this->CameraAnimationWidget->GetWidgetName());
	break;
	
      case SNAPSHOTTYPE_CAMERA :
	this->Script("pack %s -side top -anchor nw -fill both -expand 1",
		this->CameraAnimationWidget->GetWidgetName());
	break;

      default :
	this->Script("pack %s -side top -anchor nw -fill both -expand 1",
		this->TimeAnimationWidget->GetWidgetName());
	break;
  }
}




//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::CreateButtons()
{

  
  this->ButtonFrame->SetParent (this);
  this->ButtonFrame->Create();
  
  vtkKWRadioButtonSet* RBSet = vtkKWRadioButtonSet::New();
  RBSet->SetParent (this->ButtonFrame);
  RBSet->Create();

  RBSet->PackHorizontallyOn();
  
  this->RadioButtonSlice = RBSet->AddWidget (0);
  this->RadioButtonSlice->SetText ("Slice");
  std::ostringstream command0;
  command0 << "SetSnapshotType "<< vtkKWSnapshotHandler::SNAPSHOTTYPE_SLICE;
  this->RadioButtonSlice->SetCommand(this, command0.str().c_str());

  this->RadioButtonCamera = RBSet->AddWidget (1);
  this->RadioButtonCamera->SetText ("Camera");
  std::ostringstream command1;
  command1 << "SetSnapshotType "<< vtkKWSnapshotHandler::SNAPSHOTTYPE_CAMERA;
  this->RadioButtonCamera->SetCommand(this, command1.str().c_str());

  this->RadioButtonTime = RBSet->AddWidget (2);
  this->RadioButtonTime->SetText ("Time");
  std::ostringstream command2;
  command2 << "SetSnapshotType "<< vtkKWSnapshotHandler::SNAPSHOTTYPE_TIME;
  this->RadioButtonTime->SetCommand(this, command2.str().c_str());

  this->Script ("pack %s -side top -anchor nw -fill x -expand 1",
		RBSet->GetWidgetName());

  RBSet->Delete();
}


//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::CreateWidget()
{
  // Check if already created

  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  
  this->SetTitle ("Snapshot Handler");
  this->SetBorderWidth(2);
  this->SetReliefToFlat();  

  this->SetSize(400,230);
  this->SetDeleteWindowProtocolCommand (this, "OK");
  this->CreateButtons();

  this->TimeAnimationWidget->SetParent(this);
  this->TimeAnimationWidget->Create();
  this->TimeAnimationWidget->SetAnimationTypeToSequence();

  this->CameraAnimationWidget->SetParent(this);
  this->CameraAnimationWidget->Create();
  this->CameraAnimationWidget->SetAnimationTypeToSlice();
  this->CameraAnimationWidget->SetMaximumNumberOfFrames(500);
  
  this->Update();

  this->RadioButtonTime->Select();
}



//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::SetSnapshotType (unsigned int id)
{
  this->SnapshotType = id;

  switch (this->GetSnapshotType())
  {
      case SNAPSHOTTYPE_SLICE :
	this->CameraAnimationWidget->SetAnimationTypeToSlice();
	break;
	
      case SNAPSHOTTYPE_CAMERA :
	this->CameraAnimationWidget->SetAnimationTypeToCamera();
	break;

      default :

	this->TimeAnimationWidget->SetAnimationTypeToSequence();
	break;
  }
  this->Update();
}




//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::SetRenderWidget (vtkKWRenderWidget* widget)
{
  this->TimeAnimationWidget->SetRenderWidget (widget);
  this->CameraAnimationWidget->SetRenderWidget (widget);
  this->RenderWidget = widget;
}


//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::SetView (vtkImageView* view)
{
  if (!this->IsCreated())
  {
    vtkErrorMacro("Don't use this method before calling Create() : no effect");
    return;
  }

  vtkImageView2D* view2D = vtkImageView2D::SafeDownCast (view);
  
  if (view2D)
  {
    this->CameraAnimationWidget->SetSliceSetCommand (view2D, "SetZSlice");
    this->CameraAnimationWidget->SetSliceGetCommand (view2D, "GetZSlice");
    
    this->CameraAnimationWidget->SetSliceRange (view2D->GetSliceMin(), view2D->GetSliceMax());
    this->CameraAnimationWidget->SetSliceStart (view2D->GetSliceMin());
    this->CameraAnimationWidget->SetSliceEnd (view2D->GetSliceMax());

    this->CameraAnimationWidget->Update(); 
  }
  this->View = view;    
    
}


//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::Snap (vtkKWRenderWidget* widget)
{

  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(widget);
  dialog->Create();
  dialog->MultipleSelectionOff();
  dialog->SaveDialogOn ();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  std::ostringstream os;
  os << "Save snapshot to file...";
  
  dialog->SetTitle (os.str().c_str());
  
  dialog->SetFileTypes ("{{Image files} {.jpeg .jpg .png .tiff .bmp}} {{jpeg} {.jpeg .jpg}} {{png} {.png}} {{tiff} {.tiff}} {{bmp} {.bmp}} {{all} {.*}}");
  
  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return;
  }
  
  const char* filename = dialog->GetFileName();

  std::string s_filename (filename);
  vtksys_stl::string ext = vtksys::SystemTools::GetFilenameLastExtension (s_filename);

  if( ext=="" )
  {
    s_filename += ".png";
    ext = vtksys::SystemTools::GetFilenameLastExtension (s_filename);
  }

  vtkImageWriter* writer = 0;

  if( ext==".jpg" || ext==".jpeg" )
  {
    writer = vtkJPEGWriter::New();
  }
  else if ( ext==".bmp" )
  {
    writer = vtkBMPWriter::New();
  }
  else if ( ext==".tiff" )
  {
    writer = vtkTIFFWriter::New();
  }
  else if ( ext==".png" )
  {
    writer = vtkPNGWriter::New();
  }
  else
  {
    vtkKWPopupWarningMessage (widget, "Error: Format is not supported.");
    dialog->Delete();
    return;
  }

  vtkWindowToImageFilter* snapper = vtkWindowToImageFilter::New();
  snapper->SetMagnification( 2 );
  snapper->SetInput( widget->GetRenderWindow() );
  
  writer->SetInput( snapper->GetOutput() );
  writer->SetFileName( s_filename.c_str() );
  writer->Write();

  snapper->Delete();
  writer->Delete();

  dialog->SaveLastPathToRegistry("DataPath");

  dialog->Delete();
}


//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::SetTimeUpdateCommand( vtkObject *object, const char *method)
{
  this->TimeAnimationWidget->SetSequenceUpdateCommand (object, method);
}


//----------------------------------------------------------------------------
void vtkKWSnapshotHandler::SetTimeSettings (double timemin, double timemax, unsigned int maxnumber)
{
  this->TimeAnimationWidget->SetSequenceRange(timemin, timemax, maxnumber);
  this->TimeAnimationWidget->Update();
  
}
