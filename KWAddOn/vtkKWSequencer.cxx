/*=========================================================================

  Program:   vtkINRIA3D
  Module:    $Id: vtkKWSequencer.cxx 966 2008-09-29 16:40:25Z ntoussaint $
  Language:  C++
  Author:    $Author: ntoussaint $
  Date:      $Date: 2008-09-29 17:40:25 +0100 (Mon, 29 Sep 2008) $
  Version:   $Revision: 966 $

  Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
  See Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

  =========================================================================*/
#include <vtkKWSequencer.h>
#include "vtkObjectFactory.h"

#include <vtkTimerLog.h>
#include <vtkKWScale.h>
#include <vtkKWScaleSet.h>
#include <vtkKWPushButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkDataManager.h>
#include <vtkMetaDataSetSequence.h>
#include "vtkKWTimeAnimationWidget.h"
#include <vtkKWMainWindowInteractor.h>
#include <vtkKWPageView.h>
#include <vtkKWApplication.h>
#include <vtkKWFrameWithLabel.h>
#include <pixmap/KWAddOnResources.h>
#include <vtkKWIcon.h>
#include <vtkKWSeparator.h>
#include <vtkKWSpinBoxWithLabel.h>
#include <vtkKWSpinBox.h>
#include <vtkKWLabel.h>
#include <vtkKWComboBox.h>
#include <vtkKWSequenceEditorWidget.h>
#include <vtkKWMessageDialog.h>
#include <vtkKWScaleWithLabel.h>
#include <vtkKWScale.h>
#include <vtkDoubleArray.h>
#include "vtkKWHistogram.h"
#include "vtkKWLabel.h"
#include "vtkKWMessage.h"
#include <vtksys/SystemTools.hxx>
#include "vtkKWPiecewiseFunctionEditor.h"
#include "vtkKWPiecewiseFunctionEditorSet.h"
#include <vtkKWIcon.h>
#include <vtkKWTkUtilities.h>
#include <vtkLandmarkManager.h>
#include <vtkKWLandmarkManagerWidget.h>
#include <vtkKWTimeTableWidget.h>
#include <vtkKWPreviewPage.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkDataArrayCollection.h>
#include <vtkDoubleArray.h>
#include <vtkKWSnapshotHandler.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWSequencer );
vtkCxxRevisionMacro( vtkKWSequencer, "$Revision: 966 $");

//----------------------------------------------------------------------------
vtkKWSequencer::vtkKWSequencer()
{
  
  this->ToolType = TOOL_HORIZONTAL;
  this->Icon->SetImage(image_datasequencer, image_datasequencer_width, image_datasequencer_height, image_datasequencer_pixel_size, image_datasequencer_length);
  this->SetName("Data sequencer");

  this->TimerLog = vtkTimerLog::New();
  this->TimeScaleSet     = vtkKWScaleSet::New();
  this->PlayButton       = vtkKWPushButton::New();
  this->NextButton       = vtkKWPushButton::New();
  this->PrevButton       = vtkKWPushButton::New();
  this->RealTimeCheckbox = vtkKWCheckButtonWithLabel::New();
  this->AnimationWidget  = vtkKWTimeAnimationWidget::New();
  this->AnimationFrame  = vtkKWFrameWithLabel::New();
  this->TimeScale     = vtkKWScaleWithLabel::New();
  this->SpeedSpinBox = vtkKWSpinBoxWithLabel::New();
  this->ManageSequenceButton = vtkKWPushButton::New();
  this->TimeTableButton = vtkKWPushButton::New();
  this->LandmarkTimeTableButton = vtkKWPushButton::New();
  this->CycleModeButton = vtkKWPushButton::New();

  this->ButtonFrame1 = vtkKWFrame::New();
  this->ButtonFrame11 = vtkKWFrame::New();
  this->ButtonFrame12 = vtkKWFrame::New();
  this->ButtonFrame2 = vtkKWFrame::New();
  this->ButtonFrame3 = vtkKWFrame::New();
  
  this->SequencerStatus  = vtkKWSequencer::STATUS_STOP;
  this->LinkScales       = false;
  this->RealTimeState = true;
  this->SpeedRatio = 1.0;
  this->FinestResolution = 0;
  this->CycleState = 1;
  this->CurrentSign = 1;
  
  this->TimeTableWidget = 0;
  this->TimeTableDialog = vtkKWTopLevel::New();
  this->LandmarkManager = vtkLandmarkManager::New();
  this->LandmarkDialog = vtkKWTopLevel::New();
}

//----------------------------------------------------------------------------
vtkKWSequencer::~vtkKWSequencer()
{  
  this->SequencerStatus  = vtkKWSequencer::STATUS_STOP;
  
  this->TimerLog->Delete();
  this->PlayButton->Delete();
  this->NextButton->Delete();
  this->PrevButton->Delete();
  this->RealTimeCheckbox->Delete();
  this->AnimationWidget->Delete();
  this->AnimationFrame->Delete();
  this->TimeScale->Delete();
  this->TimeScaleSet->Delete();
  this->SpeedSpinBox->Delete(); 
  this->ManageSequenceButton->Delete();
  this->TimeTableButton->Delete();
  this->LandmarkTimeTableButton->Delete();
  if (this->TimeTableWidget)
    this->TimeTableWidget->Delete();
  this->TimeTableDialog->Delete();
  this->LandmarkManager->Delete();
  this->LandmarkDialog->Delete();
  this->CycleModeButton->Delete();
  
  this->ButtonFrame1->Delete();
  this->ButtonFrame11->Delete();
  this->ButtonFrame12->Delete();
  this->ButtonFrame2->Delete();
  this->ButtonFrame3->Delete();
}


//----------------------------------------------------------------------------
void vtkKWSequencer::Update()
{

  this->Superclass::Update();
  
  
  if (!this->ParentObject ||
      !this->ParentObject->GetDataManager()
      )
    return;
  
  double timemin = this->ParentObject->GetDataManager()->GetSequencesRangeMin();
  double timemax = this->ParentObject->GetDataManager()->GetSequencesRangeMax();  

  if (timemin >= timemax)
    return;
  
  double finestresolution = (timemax - timemin) / (double)this->ParentObject->GetDataManager()->GetSequencesMaxNumber();

  this->FinestResolution = finestresolution;  
  this->AnimationWidget->SetSequenceRange(timemin, timemax, this->ParentObject->GetDataManager()->GetSequencesMaxNumber());
  if (this->TimeScale->IsCreated())
  {
    vtkKWScale* scale = this->TimeScale->GetWidget();
    scale->SetRange(timemin, timemax);
    scale->SetResolution(finestresolution);
  }

}


//----------------------------------------------------------------------------
void vtkKWSequencer::RemoveAllSequences()
{
  this->TimeScaleSet->DeleteAllWidgets();
}


//----------------------------------------------------------------------------
void vtkKWSequencer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkKWSequencer::Start(void)
{
  if (!this->ParentObject ||
      !this->ParentObject->GetDataManager()
      )
    return;
  
  // Sequence is playing: we pause it
  if ( this->SequencerStatus == vtkKWSequencer::STATUS_PLAY )
  {
    this->SequencerStatus = vtkKWSequencer::STATUS_STOP;
    this->PlayButton->SetImageToPixels(image_play, image_play_width, image_play_height, image_play_pixel_size, image_play_length);
    return;
  }
  
  // Sequence is pause: we play it
//   unsigned int Nmax = 0;
  double timemin = this->ParentObject->GetDataManager()->GetSequencesRangeMin();
  double timemax = this->ParentObject->GetDataManager()->GetSequencesRangeMax();  
  if (timemin >= timemax)
    return;
  double finestresolution = (timemax - timemin) / (double)this->ParentObject->GetDataManager()->GetSequencesMaxNumber();
  this->FinestResolution = finestresolution;
  
  double time = timemin;
  std::vector<vtkMetaDataSet*> metadatasetlist = this->ParentObject->GetDataManager()->GetMetaDataSetList();
  std::vector<vtkMetaDataSetSequence*> sequencelist;
  
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    vtkMetaDataSetSequence* sequence = NULL;
    sequence = vtkMetaDataSetSequence::SafeDownCast(metadatasetlist[i]);
    if (!sequence)
      continue;
    sequencelist.push_back(sequence);
    time = sequence->GetTime();
  }
  
  if ( !sequencelist.size() )
    return;
  
  this->SequencerStatus = vtkKWSequencer::STATUS_PLAY;
  this->PlayButton->SetImageToPixels(image_pause, image_pause_width, image_pause_height, image_pause_pixel_size, image_pause_length);
  
  double globaltime = 0.0;
  
  this->GetApplication()->ProcessPendingEvents();
  
  TimerLog->StartTimer();

  double delay = 0;
  
  while(globaltime < 120.0 && this->GetParentObject()->IsMapped() && this->SequencerStatus == vtkKWSequencer::STATUS_PLAY)
  {
    this->GetApplication()->ProcessPendingEvents();

    if (!this->RealTimeState)
    {
      time += (double)(this->CurrentSign)*finestresolution;
    }
    if (time > timemax)
    {
      if (this->CycleState)
	time = timemin;
      else
      {
	this->CurrentSign = -this->CurrentSign;
	time = timemax;
      }
    }
    if (time < timemin)
    {      
      if (this->CycleState)
	time = timemax;
      else
      {
	this->CurrentSign = -this->CurrentSign;
	time = timemin;
      }
    }
    
    this->TimeScale->GetWidget()->SetValue (time);
    
    TimerLog->StopTimer();

    delay = TimerLog->GetElapsedTime();
    globaltime+=delay;
    TimerLog->StartTimer();

    delay*=this->SpeedRatio;

    if (this->RealTimeState)
    {
      time+=(double)(this->CurrentSign)*delay;
    }
  }
  
  TimerLog->StopTimer();

  this->SequencerStatus = vtkKWSequencer::STATUS_STOP;
  this->PlayButton->SetImageToPixels(image_play, image_play_width, image_play_height, image_play_pixel_size, image_play_length);
  
}

//----------------------------------------------------------------------------
void vtkKWSequencer::Stop(void)
{
  this->SequencerStatus = vtkKWSequencer::STATUS_STOP;
}

//----------------------------------------------------------------------------
void vtkKWSequencer::Next(void)
{
  // Next/Prev frames work only if the player is stopped
  if (this->SequencerStatus == vtkKWSequencer::STATUS_PLAY)
    return;

  if ( !this->ParentObject || !this->ParentObject->GetDataManager() )
    return;

  double timemin = this->ParentObject->GetDataManager()->GetSequencesRangeMin();
  double timemax = this->ParentObject->GetDataManager()->GetSequencesRangeMax();  
  if (timemin >= timemax)
    return;
  double finestresolution = (timemax - timemin) / (double) this->ParentObject->GetDataManager()->GetSequencesMaxNumber();
  this->FinestResolution = finestresolution;
  
  double time = timemin;
  std::vector<vtkMetaDataSet*> metadatasetlist = this->ParentObject->GetDataManager()->GetMetaDataSetList();
  std::vector<vtkMetaDataSetSequence*> sequencelist;
  
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    vtkMetaDataSetSequence* sequence = NULL;
    sequence = vtkMetaDataSetSequence::SafeDownCast(metadatasetlist[i]);
    if (!sequence)
      continue;
    sequencelist.push_back(sequence);
    time = sequence->GetTime();
  }

  if ( !sequencelist.size() )
    return;

  this->GetApplication()->ProcessPendingEvents();
  time += finestresolution;
  if (time > timemax)
    time = timemax;
  if (time < timemin)
    time = timemin;

  this->TimeScale->GetWidget()->SetValue (time);
  this->GetApplication()->ProcessPendingEvents();
}

//----------------------------------------------------------------------------
void vtkKWSequencer::Prev(void)
{
  // Next/Prev frames work only if the player is stopped
  if (this->SequencerStatus == vtkKWSequencer::STATUS_PLAY)
    return;

  if ( !this->ParentObject || !this->ParentObject->GetDataManager() )
    return;

  double timemin = this->ParentObject->GetDataManager()->GetSequencesRangeMin();
  double timemax = this->ParentObject->GetDataManager()->GetSequencesRangeMax();  
  if (timemin >= timemax)
    return;
  
  double finestresolution = (timemax - timemin) / (double) this->ParentObject->GetDataManager()->GetSequencesMaxNumber();
  this->FinestResolution = finestresolution;
  
  double time = timemin;
  std::vector<vtkMetaDataSet*> metadatasetlist = this->ParentObject->GetDataManager()->GetMetaDataSetList();
  std::vector<vtkMetaDataSetSequence*> sequencelist;
  
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    vtkMetaDataSetSequence* sequence = NULL;
    sequence = vtkMetaDataSetSequence::SafeDownCast(metadatasetlist[i]);
    if (!sequence)
      continue;
    sequencelist.push_back(sequence);
    time = sequence->GetTime();
  }

  if ( !sequencelist.size() )
    return;

  this->GetApplication()->ProcessPendingEvents();
  time -= finestresolution;
  if (time > timemax)
    time = timemax;
  if (time < timemin)
    time = timemin;

  this->TimeScale->GetWidget()->SetValue (time);
  this->GetApplication()->ProcessPendingEvents();
}

//----------------------------------------------------------------------------
void vtkKWSequencer::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();

  //this->SetLabelText ("Sequencer");
  
  this->TimeScaleSet->SetParent (this);
  this->TimeScaleSet->Create();
  this->TimeScaleSet->SetBorderWidth(2);
  this->TimeScaleSet->SetReliefToGroove();
  this->TimeScaleSet->PackHorizontallyOn();
  this->TimeScaleSet->SetMaximumNumberOfWidgetsInPackingDirection(3);

  this->ButtonFrame1->SetParent (this);
  this->ButtonFrame1->Create();
  this->ButtonFrame11->SetParent (this->ButtonFrame1);
  this->ButtonFrame11->Create();
  this->ButtonFrame12->SetParent (this->ButtonFrame1);
  this->ButtonFrame12->Create();
  this->ButtonFrame2->SetParent (this);
  this->ButtonFrame2->Create();
  this->ButtonFrame3->SetParent (this);
  this->ButtonFrame3->Create();

  
  this->CycleModeButton->SetParent(this->ButtonFrame11);
  this->CycleModeButton->Create();
  this->CycleModeButton->SetImageToPixels (image_cyclic, image_cyclic_width, image_cyclic_height, image_cyclic_pixel_size, image_cyclic_length);
  this->CycleModeButton->SetCommand(this, "CycleModeButtonCallback");
  this->CycleModeButton->SetBalloonHelpString("Toggle the play mode \n(cyclic / non-cyclic)");

  this->RealTimeCheckbox->SetParent(this->ButtonFrame11);
  this->RealTimeCheckbox->Create();
  this->RealTimeCheckbox->SetLabelText("Real-Time");
  this->RealTimeCheckbox->GetWidget()->Select();
  this->RealTimeCheckbox->GetWidget()->SetCommand(this, "RealTimeCheckboxCallback");
  this->RealTimeCheckbox->SetBalloonHelpString("Toggle the real time mode");
  
  this->SpeedSpinBox->SetParent(this->ButtonFrame12);
  this->SpeedSpinBox->Create();
  this->SpeedSpinBox->GetWidget()->SetRange (-400.0, 400.0);
  this->SpeedSpinBox->GetWidget()->SetIncrement(5.0);
  this->SpeedSpinBox->GetWidget()->SetValue(100.0);
  this->SpeedSpinBox->GetWidget()->SetRestrictValueToDouble ();
  this->SpeedSpinBox->SetLabelText("Speed (%) : ");
  this->SpeedSpinBox->GetWidget()->SetCommand(this, "SpeedSpinBoxCallback");
  this->SpeedSpinBox->GetWidget()->SetBalloonHelpString("Set the display speed (percent of real time speed)");


  this->PrevButton->SetParent(this->ButtonFrame2);
  this->PrevButton->Create();
  this->PrevButton->SetImageToPixels(image_backward, image_backward_width, image_backward_height, image_backward_pixel_size, image_backward_length);
  this->PrevButton->SetCommand(this, "Prev");
  this->PrevButton->SetBalloonHelpString("Previous frame");

  this->PlayButton->SetParent(this->ButtonFrame2);
  this->PlayButton->Create();
  this->PlayButton->SetImageToPixels(image_play, image_play_width, image_play_height, image_play_pixel_size, image_play_length);
  this->PlayButton->SetCommand(this, "Start");
  this->PlayButton->SetBalloonHelpString("Play sequence");

  this->NextButton->SetParent(this->ButtonFrame2);
  this->NextButton->Create();
  this->NextButton->SetImageToPixels(image_forward, image_forward_width, image_forward_height, image_forward_pixel_size, image_forward_length);
  this->NextButton->SetCommand(this, "Next");
  this->NextButton->SetBalloonHelpString("Next frame");
  

  this->TimeScale->SetParent(this);
  this->TimeScale->Create();
  vtkKWScale* scale = this->TimeScale->GetWidget();
  
  scale->SetRange (0.0, 1.0);
  scale->SetLength (300);
  scale->SetCommand (this, "TimeScaleCallback");
  scale->SetResolution(0.05);
  scale->SetValue (0.0);
  scale->SetBalloonHelpString("Follow the time line here");
  
  this->TimeScale->SetLabelText("Time cursor (in seconds)");
  this->TimeScale->SetLabelPositionToTop();

  
  
  this->ManageSequenceButton->SetParent(this->ButtonFrame3);
  this->ManageSequenceButton->Create();
  this->ManageSequenceButton->SetImageToPixels(image_control, image_control_width, image_control_height, image_control_pixel_size, image_control_length);
  this->ManageSequenceButton->SetCommand(this, "ManageSequenceButtonCallback");
  this->ManageSequenceButton->SetBalloonHelpString("Manage the selected sequence...");
  
  this->TimeTableButton->SetParent(this->ButtonFrame3);
  this->TimeTableButton->Create();
  this->TimeTableButton->SetImageToPixels(image_info, image_info_width, image_info_height, image_info_pixel_size, image_info_length);
  this->TimeTableButton->SetCommand(this, "TimeTableButtonCallback");
  this->TimeTableButton->SetBalloonHelpString("Global parameter follow-up...");
//   this->TimeTableButton->SetStateToDisabled();
  
  this->LandmarkTimeTableButton->SetParent(this->ButtonFrame3);
  this->LandmarkTimeTableButton->Create();
  this->LandmarkTimeTableButton->SetImageToPixels(image_find, image_find_width, image_find_height, image_find_pixel_size, image_find_length);
  this->LandmarkTimeTableButton->SetCommand(this, "LandmarkTimeTableButtonCallback");
  this->LandmarkTimeTableButton->SetBalloonHelpString("Landmark follow-up...");

  this->Pack();
  
}

    
//----------------------------------------------------------------------------
void vtkKWSequencer::TimeScaleCallback (double time)
{

  this->ParentObject->UpdateToTime(time);  
  
  if (this->TimeTableWidget && this->TimeTableWidget->IsMapped())
    this->TimeTableWidget->UpdateTime (time);

  this->ParentObject->Render();
}

    
  
//----------------------------------------------------------------------------
void vtkKWSequencer::Pack (void)
{

  if (this->ButtonFrame1->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -fill none -expand n -padx 2 -pady 2", 
		  this->ButtonFrame1->GetWidgetName());
  }
  if (this->ButtonFrame11->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -fill none -expand n -padx 2 -pady 2", 
		  this->ButtonFrame11->GetWidgetName());
  }
  if (this->ButtonFrame12->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -fill none -expand n -padx 2 -pady 2", 
		  this->ButtonFrame12->GetWidgetName());
  }
  if (this->CycleModeButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -fill none -expand n -padx 5 -pady 2", 
		  this->CycleModeButton->GetWidgetName());
  }
  if (this->RealTimeCheckbox->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -fill none -expand n -padx 5 -pady 2", 
		  this->RealTimeCheckbox->GetWidgetName());
  }
  if (this->SpeedSpinBox->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -fill none -expand n -padx 5 -pady 2", 
		  this->SpeedSpinBox->GetWidgetName());
  }

  if (this->ButtonFrame2->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -fill none -expand n -padx 2 -pady 2", 
		  this->ButtonFrame2->GetWidgetName());
  }
  if (this->PrevButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -fill none -expand n -padx 7 -pady 2", 
		  this->PrevButton->GetWidgetName());
  }
  if (this->PlayButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -fill none -expand n -padx 7 -pady 2", 
		  this->PlayButton->GetWidgetName());
  }

  if (this->NextButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor nw -fill none -expand n -padx 7 -pady 2", 
		  this->NextButton->GetWidgetName());
  }

  
  if (this->TimeScale->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -fill none -expand n -padx 2 -pady 2", 
		  this->TimeScale->GetWidgetName());
  }  


  vtkKWSeparator* separator1 = vtkKWSeparator::New();
  separator1->SetParent(this);
  separator1->Create();
  this->Script("pack %s -side top -fill x -pady 15", separator1->GetWidgetName());
  separator1->Delete();

  
  if (this->ButtonFrame3->IsCreated())
  {
    this->Script( "pack %s -side top -anchor nw -fill none -expand n -padx 2 -pady 2", 
		  this->ButtonFrame3->GetWidgetName());
  }

  if (this->ManageSequenceButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor ne -fill none -expand n -padx 2 -pady 2", 
		  this->ManageSequenceButton->GetWidgetName());
  }
  if (this->TimeTableButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor ne -fill none -expand n -padx 2 -pady 2", 
		  this->TimeTableButton->GetWidgetName());
  }
  if (this->LandmarkTimeTableButton->IsCreated())
  {
    this->Script( "pack %s -side left -anchor ne -fill none -expand n -padx 2 -pady 2", 
		  this->LandmarkTimeTableButton->GetWidgetName());
  }


  
  
}

//----------------------------------------------------------------------------
void vtkKWSequencer::RealTimeCheckboxCallback (int val)
{
  if (val)
  {
    this->RealTimeState = true;
    this->SpeedSpinBox->GetWidget()->SetStateToNormal ();
  }
  else
  {
    this->SpeedSpinBox->GetWidget()->SetStateToDisabled ();
    this->RealTimeState = false;
  }
}

//----------------------------------------------------------------------------
void vtkKWSequencer::SpeedSpinBoxCallback (double val)
{
  
  this->SpeedRatio = val/100.0;
}


//----------------------------------------------------------------------------
void vtkKWSequencer::CycleModeButtonCallback ()
{
  this->CycleState = !this->CycleState;
  if (this->CycleState)
  {
    this->CycleModeButton->SetImageToPixels (image_cyclic, image_cyclic_width, image_cyclic_height, image_cyclic_pixel_size, image_cyclic_length);
    this->CurrentSign = 1;
  }
  else
    this->CycleModeButton->SetImageToPixels (image_noncyclic, image_noncyclic_width, image_noncyclic_height, image_noncyclic_pixel_size, image_noncyclic_length);
    
}

//----------------------------------------------------------------------------
void vtkKWSequencer::ManageSequenceButtonCallback (void)
{

  
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  if (!sequence)
    return;
  
  vtkKWMessageDialog* toplevel = vtkKWMessageDialog::New();
  
  toplevel->SetMasterWindow(this);
  toplevel->Create();
  toplevel->SetResizable (1,1);
  toplevel->SetStyleToOkCancel();
  toplevel->SetTitle(sequence->GetName());
  toplevel->SetDeleteWindowProtocolCommand(toplevel, "Cancel");

  vtkKWSequenceEditorWidget* widget = vtkKWSequenceEditorWidget::New();

  widget->SetParent(toplevel->GetTopFrame());
  widget->Create();

  widget->SetSequence (sequence);
  widget->Initialize();  
  
  this->Script("pack %s -fill both -side top -expand t", 
	       widget->GetWidgetName());


  toplevel->SetDisplayPositionToDefault();
  toplevel->SetOKButtonText("Apply");
  if ( toplevel->Invoke () == 0 )
  {
    toplevel->Delete();
    widget->Delete();
    return ;
  }
  

  widget->TransposeDataToSequence();
 
  toplevel->Delete();
  widget->Delete();

  this->Update();

  
}



//----------------------------------------------------------------------------
void vtkKWSequencer::TimeTableButtonCallback (void)
{

  if (!this->ParentObject ||
      !this->ParentObject->GetDataManager()
      )
    return;

  if (!this->TimeTableDialog->IsCreated())
  {
    this->TimeTableDialog->SetMasterWindow (this);
    this->TimeTableDialog->SetApplication(this->GetApplication());
    this->TimeTableDialog->Create();
    this->TimeTableDialog->SetDeleteWindowProtocolCommand(this->TimeTableDialog, "Withdraw");
  }
  
  if (this->TimeTableDialog->IsMapped())
    return;
  
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  if (!sequence)
    return;

  this->TimeTableDialog->UnpackChildren();
  
  vtkKWTimeTableWidget* widget = vtkKWTimeTableWidget::New();
  widget->SetParent(this->TimeTableDialog);
  widget->Create();
  this->Script("pack %s -fill both -side top -expand t", 
	       widget->GetWidgetName());
  
  this->SetTimeTableWidget (widget);
  widget->Delete();

  this->TimeTableDialog->SetTitle ("Global Parameter Follow-up");
  this->TimeTableWidget->SetSequence (sequence);
  this->TimeTableWidget->Initialize();

  vtkDataArrayCollection* arraylist = sequence->GetArrayCollection();

  for (int i=0; i<arraylist->GetNumberOfItems(); i++)
  {
    vtkDoubleArray* array = vtkDoubleArray::SafeDownCast (arraylist->GetItemAsObject(i));
    if (array)
      this->TimeTableWidget->AddDataArray (array);
  }
  
  
  this->TimeTableDialog->DeIconify();
  this->TimeTableDialog->Raise();    

}


//----------------------------------------------------------------------------
void vtkKWSequencer::LandmarkDialogOked (void)
{
  this->LandmarkDialog->Withdraw();

  
  if (!this->TimeTableDialog->IsCreated())
  {
    this->TimeTableDialog->SetMasterWindow (this);
    this->TimeTableDialog->SetApplication(this->GetApplication());
    this->TimeTableDialog->Create();
    this->TimeTableDialog->SetDeleteWindowProtocolCommand(this->TimeTableDialog, "Withdraw");
  }
  
  if (this->TimeTableDialog->IsMapped())
    return;
  
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  if (!sequence || !sequence->GetDataSet())
    return;

  vtkKWPageView* page = this->ParentObject->GetPage (sequence->GetTag());
  if (!page)
    return;
  vtkLandmarkManager* landmarkmanager = page->GetLandmarkManager();
  
  std::vector<std::string> arraynames;
  vtkDataArrayCollection* collection = vtkDataArrayCollection::New();
  sequence->GetColorArrayCollection (collection);
  for (int i=0; i<collection->GetNumberOfItems(); i++)
  {
    if (collection->GetItem (i)->GetNumberOfComponents() == 1)
      arraynames.push_back (collection->GetItem (i)->GetName());
  }
  collection->Delete();
  
  this->TimeTableDialog->UnpackChildren();
  this->TimeTableDialog->SetTitle ("Landmark Follow-up");
  
  
  vtkKWTimeTableWidget* widget = vtkKWTimeTableWidget::New();
  widget->SetParent(this->TimeTableDialog);
  widget->Create();
  this->Script("pack %s -fill both -side top -expand t", 
	       widget->GetWidgetName());
  
  this->SetTimeTableWidget (widget);
  widget->Delete();
  
  this->TimeTableWidget->SetSequence (sequence);
  this->TimeTableWidget->Initialize();
  
  for (unsigned int i=0; i<landmarkmanager->GetNumberOfLandmarks(); i++)
  {
    vtkLandmark* landmark = landmarkmanager->GetLandmark(i);
    double* pos = landmark->GetPosition();
    vtkIdType id = sequence->GetDataSet()->FindPoint (pos);
    if (id < 0)
      continue;
    
    landmark->SetFlag ("PointIdToFollow", (double)id);

    for (unsigned int j=0; j<arraynames.size(); j++)
    {
      vtkDoubleArray* follower = sequence->GenerateFollowerTimeTable (arraynames[j].c_str(), id);
      if (follower)
      {
	this->TimeTableWidget->AddDataArray (follower, landmark->GetColor());
	follower->Delete();
      }
      
    }
    
  }
  
  this->TimeTableDialog->DeIconify();
  this->TimeTableDialog->Raise();
  
}



//----------------------------------------------------------------------------
void vtkKWSequencer::LandmarkTimeTableButtonCallback (void)
{

  if (!this->ParentObject ||
      !this->ParentObject->GetDataManager()
      )
    return;

  if (!this->LandmarkDialog->IsCreated())
  {
    this->LandmarkDialog->SetMasterWindow (this);
    this->LandmarkDialog->SetApplication(this->GetApplication());
    this->LandmarkDialog->Create();
    this->LandmarkDialog->SetDeleteWindowProtocolCommand(this->LandmarkDialog, "Withdraw");
  }
  if (!this->TimeTableDialog->IsCreated())
  {
    this->TimeTableDialog->SetMasterWindow (this);
    this->TimeTableDialog->SetApplication(this->GetApplication());
    this->TimeTableDialog->Create();
    this->TimeTableDialog->SetDeleteWindowProtocolCommand(this->TimeTableDialog, "Withdraw");
  }
  
  if (this->TimeTableDialog->IsMapped())
    return;
  if (this->LandmarkDialog->IsMapped())
    return;
  
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast (this->MetaDataSet);
  if (!sequence)
    return;

  vtkKWLandmarkManagerWidget* landmarkwidget = vtkKWLandmarkManagerWidget::New();
  landmarkwidget->SetParent (this->LandmarkDialog);
  landmarkwidget->Create();

  vtkKWPageView* page = this->ParentObject->GetPage (sequence->GetTag());
  if (!page)
    return;
    
  landmarkwidget->SetLandmarkManager (page->GetLandmarkManager());
  landmarkwidget->Update();
  
  this->LandmarkDialog->UnpackChildren();
  
  
  vtkKWFrame* buttonframe = vtkKWFrame::New();
  buttonframe->SetParent (this->LandmarkDialog);
  buttonframe->Create();
  vtkKWPushButton* buttoncancel = vtkKWPushButton::New();
  buttoncancel->SetText ("Cancel");
  buttoncancel->SetParent (buttonframe);
  buttoncancel->Create();
  buttoncancel->SetCommand (this->LandmarkDialog, "Withdraw");
  vtkKWPushButton* buttonok = vtkKWPushButton::New();
  buttonok->SetText ("Ok");
  buttonok->SetParent (buttonframe);
  buttonok->Create();
  buttonok->SetCommand (this, "LandmarkDialogOked");
  
  this->Script ("pack %s -fill both -side left -expand t -padx 2 -pady 2",
		buttoncancel->GetWidgetName());
  this->Script ("pack %s -fill both -side left -expand t -padx 2 -pady 2",
		buttonok->GetWidgetName());
  
  
  this->Script ("pack %s -fill both -side top -expand t -padx 2 -pady 2",
		landmarkwidget->GetWidgetName());
  this->Script ("pack %s -fill both -side top -expand t -padx 2 -pady 2",
		buttonframe->GetWidgetName());
  

  buttoncancel->Delete();
  buttonok->Delete();
  buttonframe->Delete();
  landmarkwidget->Delete();

  this->LandmarkDialog->DeIconify();
  this->LandmarkDialog->Raise();  
}


  
//----------------------------------------------------------------------------
void vtkKWSequencer::SetTimeTableWidget (vtkKWTimeTableWidget* widget)
{
  if (this->TimeTableWidget == widget)
  {
    return;
  }
  if (this->TimeTableWidget)
  {
    this->TimeTableWidget->UnRegister(this);
  }
  this->TimeTableWidget = widget;
  
  if (this->TimeTableWidget)
  {
    this->TimeTableWidget->Register(this);
  }
  
}



//----------------------------------------------------------------------------
void vtkKWSequencer::PrepareToPop (void)
{
  this->Stop();
}
