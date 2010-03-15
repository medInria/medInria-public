/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWLandmarkManagerWidget.cxx 1302 2009-10-27 21:57:16Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2009-10-27 22:57:16 +0100 (Tue, 27 Oct 2009) $
Version:   $Revision: 1302 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWLandmarkManagerWidget.h>
#include "vtkObjectFactory.h"

#include <vtkKWHSVColorSelector.h>

#include <vtkPointSet.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>

#include <vtkKWPushButton.h>
#include <vtkKWComboBox.h>
#include <vtkKWLoadSaveDialog.h>
#include <vtkStringArray.h>
#include <vtkKWIcon.h>

#include <vtkLandmarkManager.h>
#include <vtkKWMessageDialog.h>
#include <vtkDataArrayCollection.h>

#include <vtksys/SystemTools.hxx>
#include <vtkErrorCode.h>

#include <vtkViewImage3D.h>
#include <vtkViewImage2D.h>
#include <vtkKWPageView.h>

#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkDataManager.h>
#include <vtkUnstructuredGrid.h>
#include <vtkKWMainWindowInteractor.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <sstream>

#include <vtkKWLoadSaveDialog.h>

#include <vtkScalarBarActor.h>
#include <vtkKWMessageDialog.h>
#include <pixmap/KWAddOnResources.h>

#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>

//#include <vtkKWColorPresetSelector.h>
#include <vtkKWMenuButton.h>
#include <vtkKWApplication.h>

#include <vtkKWComboBoxSet.h>
#include <vtkKWScaleSet.h>
#include <vtkKWComboBox.h>
#include <vtkKWScale.h>
#include <vtkKWFrameWithLabel.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkLandmarkManagerCallback.h>

#include <vtkMath.h>
#include <vtkKWSurfaceMaterialPropertyWidget.h>
#include <vtkKWPopupButtonWithLabel.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPointPicker.h>
#include <vtkRenderer.h>
#include <vtkFollower.h>
#include <vtkRenderer.h>
#include <vtkTextSource.h>
#include <vtkVectorText.h>
#include <vtkPolyDataMapper.h>
#include <vtkCaptionActor2D.h>
#include <vtkCaptionWidget.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWCheckButton.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWLandmarkManagerWidget );
vtkCxxRevisionMacro( vtkKWLandmarkManagerWidget, "$Revision: 1302 $");




void vtkKWLandmarkManagerWidgetCallback::Execute ( vtkObject *caller, unsigned long event, void* )
{

  if (!this->LandmarkWidget)
    return;
  
  if (event == vtkLandmarkManagerCallback::LandmarkAddedEvent)
  {
    if (!this->LandmarkWidget )
    {
      return;
    }
    this->LandmarkWidget->AddLandmarkToList ( this->LandmarkWidget->GetLandmarkManager()->GetLastLandmark() );
  }
  else if (event == vtkLandmarkManagerCallback::LandmarkRemovingEvent)
  {
    this->LandmarkWidget->RemoveLandmarkFromList ( this->LandmarkWidget->GetLandmarkManager()->GetLandmarkBeingRemoved() );
  }
  else if (event == vtkLandmarkManagerCallback::LandmarkPickedEvent)
  {
    this->LandmarkWidget->HighlightLandmark ( this->LandmarkWidget->GetLandmarkManager()->GetLastPickedLandmark() );
  }
}




//----------------------------------------------------------------------------
vtkKWLandmarkManagerWidget::vtkKWLandmarkManagerWidget()
{
  this->ButtonAdd        = vtkKWPushButton::New();
  this->ButtonDelete     = vtkKWPushButton::New();
  this->ButtonDeleteAll  = vtkKWPushButton::New();
  this->ButtonLoad     = vtkKWPushButton::New();
  this->ButtonSave     = vtkKWPushButton::New();
  this->LandmarkListBox  = vtkKWComboBox::New();
  this->LandmarkListBox->SetBackgroundColor (1,1,1);
  
  this->HSVColorSelector = vtkKWHSVColorSelector::New();
  this->ScalingScale     = vtkKWScale::New();
  this->GlobalHSVColorSelector = vtkKWHSVColorSelector::New();
  this->GlobalScalingScale     = vtkKWScale::New();

  this->ButtonFrame = vtkKWFrame::New();
  this->PropertyFrame = vtkKWFrameWithLabel::New();
  this->PropertyFrame->SetLabelText ("Selection properties");

  this->GlobalPropertyFrame = vtkKWFrameWithLabel::New();
  this->GlobalPropertyFrame->SetLabelText ("Global properties");
  
  this->LandmarkManager = NULL;

  this->ColorCodeTypeListBox  = vtkKWComboBox::New();
  
  
  this->LinkerVisibilityCheckBox = vtkKWCheckButtonWithLabel::New();
  this->LinkerVisibilityCheckBox->SetLabelText ("Linker Visibility");
  this->LinkerVisibilityCheckBox->GetWidget()->SetCommand (this, "GlobalLinkerVisibilityChanged");
  this->LinkerVisibilityCheckBox->GetWidget()->SetSelectedState (0);
  
  
  this->Callback = vtkKWLandmarkManagerWidgetCallback::New();
  this->Callback->SetLandmarkManagerWidget (this);

}

//----------------------------------------------------------------------------
vtkKWLandmarkManagerWidget::~vtkKWLandmarkManagerWidget()
{
  std::cout<<"deleting landmark manager widget"<<std::endl;
  
  this->ButtonAdd->Delete();
  this->ButtonDelete->Delete();
  this->ButtonDeleteAll->Delete();
  this->ButtonLoad->Delete();
  this->ButtonSave->Delete();
  this->LandmarkListBox->Delete();
  this->HSVColorSelector->Delete();
  this->ScalingScale->Delete();
  this->GlobalHSVColorSelector->Delete();
  this->GlobalScalingScale->Delete();

  this->ButtonFrame->Delete();
  this->PropertyFrame->Delete();
  this->GlobalPropertyFrame->Delete();

  this->ColorCodeTypeListBox->Delete();
  this->LinkerVisibilityCheckBox->Delete();
  
  this->SetLandmarkManager (NULL);

  this->Callback->Delete();
  
}

//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::SetLandmarkManager (vtkLandmarkManager* manager)
{

  if (this->LandmarkManager)
  {
    this->LandmarkManager->RemoveObserver (this->Callback);
  }

  
  this->LandmarkManager = manager;

  if (this->LandmarkManager)
  {
    
    this->LandmarkManager->AddObserver (vtkLandmarkManagerCallback::LandmarkAddedEvent, this->Callback);
    this->LandmarkManager->AddObserver (vtkLandmarkManagerCallback::LandmarkRemovingEvent, this->Callback);
    this->LandmarkManager->AddObserver (vtkLandmarkManagerCallback::LandmarkPickedEvent, this->Callback);    
  }
  
}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  this->ButtonFrame->SetParent (this);
  this->ButtonFrame->Create();

  if (this->ButtonFrame->IsCreated())
  {
    this->Script( "pack %s -side top -expand t -anchor nw -padx 0 -pady 0",
		  this->ButtonFrame->GetWidgetName());
  }

  this->LandmarkListBox->SetParent (this);
  this->LandmarkListBox->Create();
  this->LandmarkListBox->SetWidth (20);
  this->LandmarkListBox->ReadOnlyOff();
  this->LandmarkListBox->SetValue("No Landmark");
  this->LandmarkListBox->SetCommand(this, "SelectionChangedCallback");
  
  this->PropertyFrame->SetParent (this);
  this->PropertyFrame->Create();
  this->PropertyFrame->CollapseFrame();
  this->GlobalPropertyFrame->SetParent (this);
  this->GlobalPropertyFrame->Create();
  this->GlobalPropertyFrame->CollapseFrame();

  if (this->LandmarkListBox->IsCreated())
  {
    this->Script( "pack %s -side top -expand f -anchor nw -padx 2 -pady 2", 
		  this->LandmarkListBox->GetWidgetName());
  }
  
  if (this->PropertyFrame->IsCreated())
  {
    this->Script( "pack %s -side top -expand t -anchor nw -padx 2 -pady 2", 
		  this->PropertyFrame->GetWidgetName());
  }
  if (this->GlobalPropertyFrame->IsCreated())
  {
    this->Script( "pack %s -side top -expand t -anchor nw -padx 2 -pady 2", 
		  this->GlobalPropertyFrame->GetWidgetName());
  }
  
  this->PropertyFrame->ExpandFrame();
  
  this->CreateButtons();
  
  // Update according to the current metadataset
  this->Update();

  // Pack
  this->Pack();


}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::CreateButtons()
{
  this->ButtonAdd->SetParent(this->ButtonFrame);
  this->ButtonAdd->Create();
  this->ButtonAdd->SetImageToPixels(image_add, image_add_width, image_add_height, image_add_pixel_size, image_add_length); 
  this->ButtonAdd->SetCommand(this, "ButtonAddCallback");
  this->ButtonAdd->SetBalloonHelpString("Add a landmark\nCreates a landmark in the visible views.");

  this->ButtonDelete->SetParent(this->ButtonFrame);
  this->ButtonDelete->Create();
  this->ButtonDelete->SetImageToPixels(image_delete, image_delete_width, image_delete_height, image_delete_pixel_size, image_delete_length); 
  this->ButtonDelete->SetCommand(this, "ButtonDeleteCallback");
  this->ButtonDelete->SetBalloonHelpString("Delete the selected landmark");

  this->ButtonDeleteAll->SetParent(this->ButtonFrame);
  this->ButtonDeleteAll->Create();
  this->ButtonDeleteAll->SetImageToPixels(image_deleteall, image_deleteall_width, image_deleteall_height, image_deleteall_pixel_size, image_deleteall_length); 
  this->ButtonDeleteAll->SetCommand(this, "ButtonDeleteAllCallback");
  this->ButtonDeleteAll->SetBalloonHelpString("Delete all landmarks");
 
  this->ButtonLoad->SetParent(this->ButtonFrame);
  this->ButtonLoad->Create();
  this->ButtonLoad->SetImageToPixels(image_fileopen, image_fileopen_width, image_fileopen_height, image_fileopen_pixel_size, image_fileopen_length); 
  this->ButtonLoad->SetCommand(this, "ButtonLoadCallback");
  this->ButtonLoad->SetBalloonHelpString("Load a landmark set...");

  this->ButtonSave->SetParent(this->ButtonFrame);
  this->ButtonSave->Create();
  this->ButtonSave->SetImageToPixels(image_filesave, image_filesave_width, image_filesave_height, image_filesave_pixel_size, image_filesave_length); 
  this->ButtonSave->SetCommand(this, "ButtonSaveCallback");
  this->ButtonSave->SetBalloonHelpString("Save the landmark set...");  
  
  this->HSVColorSelector->SetParent(this->PropertyFrame->GetFrame());
  this->HSVColorSelector->Create();
  this->HSVColorSelector->SetHueSatWheelRadius (20);
  this->HSVColorSelector->SetBalloonHelpString("Color");
  this->HSVColorSelector->SetSelectionChangingCommand (this, "ColorChangingCallback");
  this->HSVColorSelector->SetSelectionChangedCommand (this, "ColorChangingCallback");
  this->HSVColorSelector->InvokeCommandsWithRGBOn();

  this->ScalingScale->SetParent (this->PropertyFrame->GetFrame());
  this->ScalingScale->Create();
  this->ScalingScale->SetOrientationToHorizontal();
  this->ScalingScale->SetLabelText("Scaling:");
  this->ScalingScale->SetRange (0,20);
  this->ScalingScale->SetResolution (1);
  this->ScalingScale->SetLength (95);
  this->ScalingScale->SetSliderLength (10);
  this->ScalingScale->SetActiveBackgroundColor (0.9,0.2,0.4);
  this->ScalingScale->SetBalloonHelpString("Scaling");
  this->ScalingScale->SetValue (5);
  this->ScalingScale->SetEndCommand (this, "ScalingChangingCallback");

  this->GlobalHSVColorSelector->SetParent(this->GlobalPropertyFrame->GetFrame());
  this->GlobalHSVColorSelector->Create();
  this->GlobalHSVColorSelector->SetHueSatWheelRadius (20);
  this->GlobalHSVColorSelector->SetBalloonHelpString("Selected Color");
  this->GlobalHSVColorSelector->SetSelectionChangingCommand (this, "GlobalColorChangedCallback");
  this->GlobalHSVColorSelector->SetSelectionChangedCommand (this, "GlobalColorChangedCallback");
  this->GlobalHSVColorSelector->InvokeCommandsWithRGBOn();

  this->GlobalScalingScale->SetParent (this->GlobalPropertyFrame->GetFrame());
  this->GlobalScalingScale->Create();
  this->GlobalScalingScale->SetOrientationToHorizontal();
  this->GlobalScalingScale->SetLabelText("Scaling:");
  this->GlobalScalingScale->SetRange (0,20);
  this->GlobalScalingScale->SetResolution (1);
  this->GlobalScalingScale->SetLength (95);
  this->GlobalScalingScale->SetSliderLength (10);
  this->GlobalScalingScale->SetActiveBackgroundColor (0.9,0.2,0.4);
  this->GlobalScalingScale->SetBalloonHelpString("Scaling");
  this->GlobalScalingScale->SetValue (5);
  this->GlobalScalingScale->SetEndCommand (this, "GlobalScalingChangingCallback");

  this->LinkerVisibilityCheckBox->SetParent (this->GlobalPropertyFrame->GetFrame());
  this->LinkerVisibilityCheckBox->Create();
  this->LinkerVisibilityCheckBox->SetLabelText ("Linker visibility ");
  this->LinkerVisibilityCheckBox->SetBalloonHelpString ("link all landmarks with a virtual line (On/Off)");
  this->LinkerVisibilityCheckBox->GetWidget()->SetCommand (this, "GlobalLinkerVisibilityChanged");
  this->LinkerVisibilityCheckBox->GetWidget()->SetSelectedState (0);

  this->ColorCodeTypeListBox->SetParent (this->GlobalPropertyFrame->GetFrame());
  this->ColorCodeTypeListBox->Create();
  this->ColorCodeTypeListBox->SetBackgroundColor (1,1,1);

  this->ColorCodeTypeListBox->SetWidth (20);
  this->ColorCodeTypeListBox->ReadOnlyOn();
  this->ColorCodeTypeListBox->AddValue ("None");
  this->ColorCodeTypeListBox->AddValue ("LUT");
  this->ColorCodeTypeListBox->AddValue ("Random");
  
  this->ColorCodeTypeListBox->SetValue("None");
  this->ColorCodeTypeListBox->SetCommand(this, "GlobalColorCodeChangedCallback");
  
  
}

//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::AddLandmarkToList(vtkLandmark* landmark)
{
  
  std::ostringstream os;
  os<<landmark->GetName();
  
  this->LandmarkListBox->AddValue (os.str().c_str());
}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::RemoveLandmarkFromList(vtkLandmark* landmark)
{

  int index = this->LandmarkListBox->GetValueIndex (landmark->GetName());
  if (index >= 0)
    this->LandmarkListBox->DeleteValue (index);
}

//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::HighlightLandmark(vtkLandmark* landmark)
{
  this->PropertyFrame->ExpandFrame();

  vtkProperty* prop = landmark->GetProperty();
  this->HSVColorSelector->SetSelectedColor (vtkMath::RGBToHSV (prop->GetColor()));
  this->ScalingScale->SetValue (landmark->GetScale());
  this->LandmarkListBox->SetValue (landmark->GetName());
}



//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::PropertyChangeOff()
{
  this->PropertyFrame->CollapseFrame();
  this->GlobalPropertyFrame->CollapseFrame();
}



//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::Pack()
{
  
  
  if (this->ButtonLoad->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->ButtonLoad->GetWidgetName());
  }

  if (this->ButtonSave->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->ButtonSave->GetWidgetName());
  }  

  if (this->ButtonAdd->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->ButtonAdd->GetWidgetName());
  }

  if (this->ButtonDelete->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->ButtonDelete->GetWidgetName());
  }

  if (this->ButtonDeleteAll->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->ButtonDeleteAll->GetWidgetName());
  }


  
  if (this->HSVColorSelector->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->HSVColorSelector->GetWidgetName());
  }

  if (this->ScalingScale->IsCreated())
  {
    this->Script( "pack %s -side left -expand t -anchor nw -padx 2 -pady 2", 
		  this->ScalingScale->GetWidgetName());
  }

  if (this->ColorCodeTypeListBox->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->ColorCodeTypeListBox->GetWidgetName());
  }
 
  if (this->LinkerVisibilityCheckBox->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->LinkerVisibilityCheckBox->GetWidgetName());
  }
 
  vtkKWPushButton* commentbutton = vtkKWPushButton::New();
  commentbutton->SetParent(this->PropertyFrame->GetFrame());
  commentbutton->Create();
  commentbutton->SetImageToPixels(image_pencil, image_pencil_width, image_pencil_height, image_pencil_pixel_size, image_pencil_length); 
  commentbutton->SetCommand(this, "ButtonCommentCallback");
  commentbutton->SetBalloonHelpString("post a caption text to the landmark...");
  if (commentbutton->IsCreated())
  {
    this->Script( "pack %s -side left -expand t -anchor nw -padx 3 -pady 3", 
		  commentbutton->GetWidgetName());
  }

  commentbutton->Delete();
  

  
  if (this->GlobalHSVColorSelector->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -padx 2 -pady 2", 
		  this->GlobalHSVColorSelector->GetWidgetName());
  }

  if (this->GlobalScalingScale->IsCreated())
  {
    this->Script( "pack %s -side left -expand t -anchor nw -padx 2 -pady 2", 
		  this->GlobalScalingScale->GetWidgetName());
  }

}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::Update()
{

  if (!this->LandmarkManager)
    return;
  
  this->LandmarkListBox->DeleteAllValues ();

  for (unsigned int i=0; i<this->LandmarkManager->GetNumberOfLandmarks(); i++)
  {
    this->AddLandmarkToList (this->LandmarkManager->GetLandmark (i));
  }
  
  if (this->LandmarkManager->GetFirstSelectedLandmark())
  {
    this->HighlightLandmark (this->LandmarkManager->GetFirstSelectedLandmark());
  } else if (this->LandmarkManager->GetLastLandmark())
  {
    this->HighlightLandmark (this->LandmarkManager->GetLastLandmark());
  }

  this->GlobalHSVColorSelector->SetSelectedColor(vtkMath::RGBToHSV (this->LandmarkManager->GetSelectedColor()));
  this->GlobalScalingScale->SetValue (this->LandmarkManager->GetScale());  
}


void vtkKWLandmarkManagerWidget::ScalingChangingCallback(double op)
{
  if (!this->LandmarkManager)
    return;

//   this->LandmarkManager->ScaleAll (op);
  std::string name = this->LandmarkListBox->GetValue();
  vtkLandmark* landmark = this->LandmarkManager->FindLandmark (name.c_str());
  if (landmark)
    landmark->SetScale (op);
  
  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
}




void vtkKWLandmarkManagerWidget::GlobalScalingChangingCallback(double op)
{
  if (!this->LandmarkManager)
    return;

  this->LandmarkManager->ScaleAll (op);
  
  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
}

void vtkKWLandmarkManagerWidget::ColorChangingCallback(double r, double g, double b)
{
  if (!this->LandmarkManager)
    return;

//   this->LandmarkManager->SetSelectedColor (r,g,b);
  std::string name = this->LandmarkListBox->GetValue();
  vtkLandmark* landmark = this->LandmarkManager->FindLandmark (name.c_str());
  if (landmark)
  {
    if (landmark->IsSelected())
      landmark->SetSelectedColor (r,g,b);
    landmark->SetColor (r,g,b);
  }
  
  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
  
}

void vtkKWLandmarkManagerWidget::GlobalColorChangedCallback(double r, double g, double b)
{
  if (!this->LandmarkManager)
    return;

  this->LandmarkManager->SetSelectedColor (r,g,b);
  
  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
  
}

  
void vtkKWLandmarkManagerWidget::SelectionChangedCallback (const char* value)
{

  int index = this->LandmarkListBox->GetValueIndex (value);
  
  std::string name = this->LandmarkListBox->GetValue();
  
  vtkLandmark* landmark = this->LandmarkManager->FindLandmark (name.c_str());
  if (landmark)
    this->LandmarkManager->SelectLandmark(landmark);
  else if ( index >= 0)
  {
    this->LandmarkManager->GetLandmark (index)->SetName (value);
  }
  
  this->Update();

  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
}

void vtkKWLandmarkManagerWidget::ButtonAddCallback()
{
  if (!this->LandmarkManager)
    return;

  this->LandmarkManager->CreateAndAddLandmark();
  this->Update();

  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
  

  
}

//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::ButtonDeleteCallback()
{
  std::string name = this->LandmarkListBox->GetValue();
  vtkLandmark* landmark = this->LandmarkManager->FindLandmark (name.c_str());
  if (landmark)
    this->LandmarkManager->RemoveLandmark (landmark);
  this->Update();

  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }

}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::ButtonDeleteAllCallback()
{
  this->LandmarkManager->RemoveAllLandmarks ();
  this->Update();

  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }

}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::ButtonLoadCallback()
{

  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Open a Landmark Set");
  dialog->SetFileTypes ("{{landmark set file} {.lms}} {{all} {.*}}");

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }

  const char* filename = dialog->GetFileName();

//   this->LandmarkManager->Reset();
  this->LandmarkManager->ReadFile (filename);  
  
  this->Update();

  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
}


//----------------------------------------------------------------------------
void vtkKWLandmarkManagerWidget::ButtonSaveCallback()
{
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOff();
  dialog->SaveDialogOn ();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Save Landmark Set to File");
  dialog->SetFileTypes ("{{landmark set file} {.lms}} {{all} {.*}}");

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }

  const char* filename = dialog->GetFileName();

  std::string sfilename = filename;
  std::string::size_type pos1 = sfilename.rfind(".lms");
  std::string::size_type pos2 = sfilename.rfind(".LMS");
  if( pos1==std::string::npos && pos2==std::string::npos)
    sfilename += ".lms";
  
  this->LandmarkManager->WriteFile (sfilename.c_str());
  
  this->Update();
  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
}


void vtkKWLandmarkManagerWidget::ButtonCommentCallback()
{
  if (!this->LandmarkManager)
    return;

  std::string name = this->LandmarkListBox->GetValue();
  vtkLandmark* landmark = this->LandmarkManager->FindLandmark (name.c_str());

  vtkViewImage3D* view3D = vtkViewImage3D::SafeDownCast (this->LandmarkManager->GetViewList()->GetItemAsObject (3));
  if (!view3D)
    return;

  
  vtkKWSimpleEntryDialog* dialog = vtkKWSimpleEntryDialog::New();
  dialog->SetMasterWindow (this);
  dialog->Create();
  dialog->SetTitle ("Comment Landmark");
  
  dialog->GetEntry()->SetLabelText ("Set comment");
  dialog->GetEntry()->GetWidget()->SetBackgroundColor (1,1,1);
  dialog->GetEntry()->GetWidget()->SetValue (landmark->GetComment());
  
  if (!dialog->Invoke())
  {
    dialog->Delete();
    return;
  }
  
  
  landmark->SetComment (dialog->GetEntry()->GetWidget()->GetValue());
  landmark->UseCommentOn();

  dialog->Delete();

  vtkFollower* follower = landmark->GetTextActor();
  follower->SetCamera (view3D->GetRenderer()->GetActiveCamera());
  view3D->AddActor (follower);

  landmark->Initialize();
  view3D->Render();
  
}

void vtkKWLandmarkManagerWidget::GlobalLinkerVisibilityChanged (int val)
{
  this->LandmarkManager->SetLinkerVisibility (val);
  vtkViewImage* view = this->LandmarkManager->GetFirstView();
  if (view)
  {
    view->SyncRender();
  }
}


void vtkKWLandmarkManagerWidget::GlobalColorCodeChangedCallback (const char* val)
{
  if (! strcmp (val, "None"))
    this->LandmarkManager->SetColorMode (vtkLandmarkManager::LANDMARK_COLOR_NONE);

  else if (! strcmp (val, "LUT"))
    this->LandmarkManager->SetColorMode (vtkLandmarkManager::LANDMARK_COLOR_LUT);

  else if (! strcmp (val, "Random"))
    this->LandmarkManager->SetColorMode (vtkLandmarkManager::LANDMARK_COLOR_RANDOM);

  
}


