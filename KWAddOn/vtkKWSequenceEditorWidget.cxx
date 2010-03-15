/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSequenceEditorWidget.cxx 1 2007-11-21 09:03:23Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-21 10:03:23 +0100 (Wed, 21 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWSequenceEditorWidget.h>
#include "vtkObjectFactory.h"

#include <vtkKWPushButton.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWEntry.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWFrame.h>
#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWScrollbar.h>
#include <vtkKWIcon.h>

#include <vtkMetaDataSetSequence.h>

#include <pixmap/KWAddOnResources.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWSequenceEditorWidget );
vtkCxxRevisionMacro( vtkKWSequenceEditorWidget, "$Revision: 1 $");

//----------------------------------------------------------------------------
vtkKWSequenceEditorWidget::vtkKWSequenceEditorWidget()
{
  this->MultiColumnList = NULL;

  this->SameGeometryButton = NULL;
  this->DurationEntry = NULL;
  this->TimeStampEntry = NULL;
  
  this->Sequence = NULL;
  this->AllowReorderingOn();
}

//----------------------------------------------------------------------------
vtkKWSequenceEditorWidget::~vtkKWSequenceEditorWidget()
{
  
}


//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  
  this->SetBorderWidth(2);
  this->SetReliefToFlat();

  vtkKWFrame* fr = vtkKWFrame::New();
  fr->SetParent (this);
  fr->Create();
  this->Script("pack %s -fill both -anchor nw -side top -expand t", 
	       fr->GetWidgetName());
  
  vtkKWMultiColumnListWithScrollbars* listframe = vtkKWMultiColumnListWithScrollbars::New();
  listframe->SetParent (fr);
  listframe->Create();
  listframe->VerticalScrollbarVisibilityOn();
  listframe->HorizontalScrollbarVisibilityOff();
  listframe->GetVerticalScrollbar()->SetWidth(13);
  
  this->MultiColumnList = listframe->GetWidget();
  this->MultiColumnList->MovableColumnsOff();
  this->MultiColumnList->SetWidth(40);

  this->MultiColumnList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->MultiColumnList->SetSelectionForegroundColor (0,0,0);

  int col_index;
  
  // Add the columns (make some of them editable)

  col_index = this->MultiColumnList->AddColumn("N#");
  this->MultiColumnList->SetColumnEditable(col_index, 0);
  this->MultiColumnList->SetColumnSortMode (col_index, vtkKWMultiColumnList::ColumnEditWindowUnknown);
  
  col_index = this->MultiColumnList->AddColumn("Name");
  this->MultiColumnList->SetColumnEditable(col_index, 0);
  this->MultiColumnList->SetColumnWidth(col_index, 28);

  col_index = this->MultiColumnList->AddColumn("Time");
  this->MultiColumnList->SetColumnEditable(col_index, 1);
  this->MultiColumnList->SetColumnEditWindowToEntry (col_index);
  this->MultiColumnList->SetColumnWidth(col_index, 7);

  this->MultiColumnList->SetSelectionChangedCommand (this, "SelectionChangedCallback");
  this->MultiColumnList->SetCellUpdatedCommand (this, "CellUpdateCallback");
  this->MultiColumnList->SetUneditableCellDoubleClickCommand(this, "DoubleClickCallback");
#ifndef WIN32
	this->MultiColumnList->SetKeyPressDeleteCommand (this, "KeyPressDeleteCallback");
#endif
  this->MultiColumnList->SetSelectionModeToExtended ();
  this->MultiColumnList->MovableRowsOn();
  this->MultiColumnList->SetWidth (43);
  
  this->Script("pack %s -fill both -anchor nw -side left -expand t -padx 5 -pady 5", 
	       listframe->GetWidgetName());
  
  listframe->Delete();


  vtkKWFrame* buttonframe = vtkKWFrame::New();
  buttonframe->SetParent (fr);
  buttonframe->Create();

  vtkKWEntryWithLabel* durationent = vtkKWEntryWithLabel::New();
  durationent->SetParent (buttonframe);
  durationent->Create();
  durationent->SetLabelText("duration (s.) : ");

  this->DurationEntry = durationent->GetWidget();
  this->DurationEntry->SetWidth(10);
  this->DurationEntry->SetBackgroundColor(1,1,1);
  this->DurationEntry->SetRestrictValueToDouble ();
  this->DurationEntry->SetCommand(this, "DurationCallback");
  
  this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		durationent->GetWidgetName());

  durationent->Delete();

  vtkKWEntryWithLabel* timestampent = vtkKWEntryWithLabel::New();
  timestampent->SetParent (buttonframe);
  timestampent->Create();
  timestampent->SetLabelText("start time : ");

  this->TimeStampEntry = timestampent->GetWidget();
  this->TimeStampEntry->SetWidth(10);
  this->TimeStampEntry->SetBackgroundColor(1,1,1);
  this->TimeStampEntry->SetRestrictValueToDouble ();
  this->TimeStampEntry->SetCommand(this, "TimeStampCallback");
  
  this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		timestampent->GetWidgetName());

  timestampent->Delete();
  
  vtkKWCheckButtonWithLabel* samegeometrycb = vtkKWCheckButtonWithLabel::New();
  samegeometrycb->SetParent (buttonframe);
  samegeometrycb->Create();
  samegeometrycb->SetLabelText ("Same Topology");
  samegeometrycb->SetLabelPositionToRight();
  
  this->SameGeometryButton = samegeometrycb->GetWidget();
  this->SameGeometryButton->Deselect();
  this->SameGeometryButton->SetCommand(this, "SameGeometryCallback");  

  this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		samegeometrycb->GetWidgetName());

  samegeometrycb->Delete();
  
  vtkKWPushButton* resetbutton = vtkKWPushButton::New();
  resetbutton->SetParent (buttonframe);
  resetbutton->Create();
  resetbutton->SetText ("Reset");
  
  resetbutton->SetCommand(this, "Initialize");
  
  this->Script( "pack %s -side bottom -expand false -anchor se -fill none -padx 5 -pady 5", 
		resetbutton->GetWidgetName());

  resetbutton->Delete();

  this->Script( "pack %s -fill none -anchor nw -side left -expand false", 
		buttonframe->GetWidgetName());

  buttonframe->Delete();
  fr->Delete();
}


//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::Initialize()
{

  if (!this->GetSequence() || !this->MultiColumnList)
    return;

  if (this->GetSequence()->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
    this->SameGeometryButton->SetStateToDisabled();
  else
    this->SameGeometryButton->SetStateToNormal();
  
    
  this->MultiColumnList->DeleteAllRows();
  
  std::vector<vtkMetaDataSet*> metadatasetlist = this->GetSequence()->GetMetaDataSetList();
  
  for (unsigned int i=0; i<metadatasetlist.size(); i++)
  {
    vtkMetaDataSet* metadataset = metadatasetlist[i];

    int rowid = this->MultiColumnList->GetNumberOfRows();
    
    this->MultiColumnList->InsertCellTextAsInt(rowid, 0, rowid);
    
    this->MultiColumnList->InsertCellText(rowid, 1, metadataset->GetName());
    this->MultiColumnList->InsertCellTextAsDouble(rowid, 2, metadataset->GetTime());
    
    if (rowid == 0)
      this->TimeStampEntry->SetValueAsDouble (metadataset->GetTime());

  }

  bool samegeometry = this->GetSequence()->GetSameGeometryFlag();
  this->SameGeometryButton->SetSelectedState (samegeometry);

  this->DurationEntry->SetValueAsDouble (this->GetSequence()->GetSequenceDuration());
  
  
  
}
  


//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::SelectionChangedCallback()
{
  // we don't care
}

//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::CellUpdateCallback(int row, int col, double state)
{
  if (!this->IsMapped())
    return;
  
  // we don't care
  
}

//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::DoubleClickCallback()
{
  if (!this->IsMapped())
    return;
  
  // we don't care
}

//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::ResetCallback ()
{
  if (!this->IsMapped())
    return;
  
}


//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::DurationCallback (const char* val)
{
  if (!this->IsMapped())
    return;
  

  double duration = this->DurationEntry->GetValueAsDouble();
  if (duration <= 0.0)
  {
    duration = 1.0;
    vtkWarningMacro(<<"cannot have negative duration"<<endl);
    if (this->GetSequence())
      duration = this->GetSequence()->GetSequenceDuration();
    this->DurationEntry->SetValueAsDouble(duration);
    return;
  }
  this->UpdateTimes();
}

//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::TimeStampCallback (const char* val)
{
  if (!this->IsMapped())
    return;
  
  this->UpdateTimes();
}


//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::SameGeometryCallback (int val)
{
  if (!this->IsMapped())
    return;

  
  
  // we don't care
}

//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::KeyPressDeleteCallback (void)
{
  if (!this->GetSequence() || !this->MultiColumnList)
    return;
  
  const unsigned int Number_Of_Selected = this->MultiColumnList->GetNumberOfSelectedRows();
  int* indices = new int[Number_Of_Selected];
  this->MultiColumnList->GetSelectedRows(indices);

  for (unsigned int i=0; i<Number_Of_Selected; i++)
  {
    int index = indices[i];
    
    index-=i;
    
    this->MultiColumnList->DeleteRow (index);
    this->GetSequence()->RemoveMetaDataSet(index);
  }

  delete [] indices;

  this->Initialize();  
}



//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::UpdateTimes (void)
{
  unsigned int N = this->MultiColumnList->GetNumberOfRows();
  double duration = this->DurationEntry->GetValueAsDouble();
  double timestamp = this->TimeStampEntry->GetValueAsDouble();
  for (unsigned int i=0; i<N; i++)
  {
    double time = timestamp + ((double)i/(double)N)*duration;
    
    this->MultiColumnList->SetCellTextAsDouble(i, 2, time);
  }
  
}



//----------------------------------------------------------------------------
void vtkKWSequenceEditorWidget::TransposeDataToSequence (void)
{

  if (!this->GetSequence() || !this->MultiColumnList)
    return;

  //this->UpdateTimes();
  
  std::vector<vtkMetaDataSet*> list = this->GetSequence()->GetMetaDataSetList();

  unsigned int N = this->MultiColumnList->GetNumberOfRows();
  double duration = this->DurationEntry->GetValueAsDouble();
  std::vector<unsigned int> idtokeep;
  std::vector<unsigned int> idtoremove;
  
  for (unsigned int i=0; i<N; i++)
  {
    std::string name = this->MultiColumnList->GetCellText(i, 1);
    double time = this->MultiColumnList->GetCellTextAsDouble(i, 2);
    
    unsigned int id = this->MultiColumnList->GetCellTextAsInt (i, 0);
    
//     vtkMetaDataSet* metadataset = this->GetSequence()->FindMetaDataSet (name.c_str());
    vtkMetaDataSet* metadataset = this->GetSequence()->GetMetaDataSet (id);
    if (!metadataset)
      continue;
    
    metadataset->SetTime (time);
    idtokeep.push_back (id);
  }

  for (int i=0; i<this->GetSequence()->GetNumberOfMetaDataSets(); i++)
  {

    bool keep = false;
    for (unsigned int j=0; j<idtokeep.size(); j++)
    {
      if ((int)(idtokeep[j]) == (int)i)
      {
	keep = true;
	break;
      }
    }

    if (!keep)
      idtoremove.push_back (i);
  }


  for (unsigned int i=0; i<idtoremove.size(); i++)
  {
    // tricky stuff to remove the right id 
    int index = idtoremove[i];
    index-=i;
    this->GetSequence()->RemoveMetaDataSet (index);
  }

    
  this->GetSequence()->SetSameGeometryFlag (this->SameGeometryButton->GetSelectedState());
  this->GetSequence()->SetSequenceDuration (duration);
}

  

