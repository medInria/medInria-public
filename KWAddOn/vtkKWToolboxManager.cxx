/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWToolboxManager.cxx 885 2008-07-01 09:29:28Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-07-01 11:29:28 +0200 (Tue, 01 Jul 2008) $
Version:   $Revision: 885 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWToolboxManager.h>
#include <vtkKWToolBox.h>
#include "vtkObjectFactory.h"

#include <vtkKWLabel.h>
#include <vtkKWPushButton.h>
#include <vtkKWPushButtonSet.h>

#include <vtkKWIcon.h>
#include <vtkKWToolbar.h>
#include <vtkKWToolbarSet.h>
#include <vtkKWMainWindowInteractor.h>
#include "vtkKWSeparator.h"
#include <vtksys/stl/string>
#include "vtkKWTkUtilities.h"
#include <pixmap/KWAddOnResources.h>

#include <vtkKWBalloonHelpManager.h>
#include <sstream>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWToolboxManager );
vtkCxxRevisionMacro( vtkKWToolboxManager, "$Revision: 885 $");

//----------------------------------------------------------------------------
vtkKWToolboxManager::vtkKWToolboxManager()
{
  this->ParentObject = NULL;
  this->ButtonSet = vtkKWPushButtonSet::New();
  this->ButtonsFrame = vtkKWFrame::New();
  this->NameFrame = vtkKWFrame::New();
  this->ToolboxFrame = vtkKWFrame::New();
}

//----------------------------------------------------------------------------
vtkKWToolboxManager::~vtkKWToolboxManager()
{
  std::cout<<"deleting vtkKWToolboxManager "<<std::endl;
  this->ButtonSet->Delete();
  this->ButtonsFrame->Delete();
  this->NameFrame->Delete();
  this->ToolboxFrame->Delete();

  for (unsigned int i=0; i<this->ToolboxTopLevels.size(); i++)
    this->ToolboxTopLevels[i]->Delete();
  
}

//----------------------------------------------------------------------------
void vtkKWToolboxManager::CreateWidget()
{
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  this->Superclass::CreateWidget();

  this->ButtonsFrame->SetParent(this);
  this->ButtonsFrame->Create();
  
  this->ToolboxFrame->SetParent(this);
  this->ToolboxFrame->Create();

  this->NameFrame->SetParent(this->ToolboxFrame);
  this->NameFrame->Create();
  //this->NameFrame->SetBackgroundColor (1.0,1.0,1.0);
//   this->NameFrame->SetHeight (10);
  this->NameFrame->SetPadY(2);
  
  this->ButtonSet->SetParent (this->ButtonsFrame);
  this->ButtonSet->Create();
  this->ButtonSet->PackHorizontallyOn();
  this->ButtonSet->SetMaximumNumberOfWidgetsInPackingDirection (5);
  this->ButtonSet->SetBorderWidth(0);
  this->ButtonSet->SetReliefToFlat();
  this->ButtonSet->SetWidgetsPadX(3);
  this->ButtonSet->SetWidgetsPadY(3);
  this->ButtonSet->SetPadX(1);
  this->ButtonSet->SetPadY(1);

  this->Update();
  
  this->Pack();

  this->GetBalloonHelpManager()->SetDelay(100);
}


//----------------------------------------------------------------------------
void vtkKWToolboxManager::Pack()
{
  if (this->ButtonsFrame->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -pady 0", 
		 this->ButtonsFrame->GetWidgetName());
  }
  if (this->ButtonSet->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -padx 2 -pady 1", 
		 this->ButtonSet->GetWidgetName());
  }
  
  if (this->ToolboxFrame->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -pady 0 -fill x", 
		 this->ToolboxFrame->GetWidgetName());
  }
  
}

//----------------------------------------------------------------------------
void vtkKWToolboxManager::Update()
{


}

//----------------------------------------------------------------------------
vtkKWToolBox* vtkKWToolboxManager::GetToolbox(unsigned int id)
{
  vtkKWToolBox* box = this->ToolboxList[id];
  if (!box)
    return NULL;
  return box;
}


//----------------------------------------------------------------------------
void vtkKWToolboxManager::InsertToolbox(vtkKWToolBox* box)
{

  if (!this->IsCreated())
  {
    vtkErrorMacro("cannot insert toolbox : widget not created !");
    return;
  }
  
  unsigned int id = this->ButtonSet->GetNumberOfWidgets();
  
  vtkKWPushButton* button = this->ButtonSet->AddWidget (id);
  button->SetImageToIcon(box->GetIcon());
  std::ostringstream os;
  os << "SelectToolboxCallback " << id;
  
  button->SetCommand(this, os.str().c_str());
  button->SetBalloonHelpString(box->GetName());
  
  vtkKWTopLevel* toplevel = vtkKWTopLevel::New();
  toplevel->SetMasterWindow (this->ToolboxFrame);
  //toplevel->SetParent (this->ToolboxFrame);
  toplevel->SetApplication (this->GetApplication());
  
  box->SetParent (this->ToolboxFrame);
  box->SetParentObject(this->GetParentObject());
  box->Create();

  this->ToolboxTopLevels.push_back (toplevel);
  this->ToolboxList.push_back(box);

  this->Update();  

}




//----------------------------------------------------------------------------
void vtkKWToolboxManager::SelectToolbox (vtkKWToolBox* box)
{
 
}

//----------------------------------------------------------------------------
void vtkKWToolboxManager::SelectToolboxCallback (unsigned int pos)
{
  vtkKWToolBox* box = this->ToolboxList[pos];

  if (!box || (this->SelectedToolbox == box))
    return;
  
  this->SelectedToolbox = box;

  this->ToolboxFrame->UnpackChildren();

  
  
  if (this->ToolboxTopLevels[pos]->IsCreated() && this->ToolboxTopLevels[pos]->IsMapped())
  {
    this->ToolboxTopLevels[pos]->DeIconify();
    this->ToolboxTopLevels[pos]->Raise();
    this->ToolboxTopLevels[pos]->Focus();
    return;
  }  
  
  this->NameFrame->UnpackChildren();

  vtkKWSeparator* separator1 = vtkKWSeparator::New();
  separator1->SetParent(this->ToolboxFrame);
  separator1->Create();
  this->Script("pack %s -side top -fill x ", separator1->GetWidgetName());
  separator1->Delete();
  
  vtkKWLabel* label = vtkKWLabel::New();
  label->SetParent (this->NameFrame);
  label->Create();
  label->SetAnchorToNorthWest();
  vtkKWTkUtilities::ChangeFontWeightToBold(label);
  label->SetImageToPredefinedIcon(vtkKWIcon::IconInfoMini);
  label->SetText(box->GetName());

  this->Script("pack %s -side left -anchor nw -pady 1", 
		 label->GetWidgetName());
  label->Delete();

  vtkKWPushButton* button = vtkKWPushButton::New();
  button->SetParent (this->NameFrame);
  button->Create();
  button->SetImageToPixels(image_popup, image_popup_width, image_popup_height, image_popup_pixel_size, image_popup_length); 
  std::ostringstream command;
  command << "PopupToolbox " <<pos;
  button->SetCommand (this, command.str().c_str());
  button->SetBalloonHelpString("Popup toolbox...");
  
  this->Script("pack %s -side right -anchor ne -expand n", 
	       button->GetWidgetName());
  button->Delete();
  
  if (this->NameFrame->IsCreated())
  {
    this->Script("pack %s -side top -fill x -expand yes -pady 0", 
		 this->NameFrame->GetWidgetName());
  }
  
  vtkKWSeparator* separator2 = vtkKWSeparator::New();
  separator2->SetParent(this->ToolboxFrame);
  separator2->Create();
  this->Script("pack %s -side top -fill x", separator2->GetWidgetName());
  separator2->Delete();

  if (box->IsCreated())
  {
    this->Script("pack %s -side top -anchor nw -fill x", 
		 box->GetWidgetName());
  }

  box->Update();
}


//----------------------------------------------------------------------------
void vtkKWToolboxManager::ReplaceToolBox (unsigned int pos, vtkKWToolBox* box)
{
  std::vector<vtkKWToolBox*> tmp = this->ToolboxList;
  this->ToolboxList.clear();

  for (unsigned int i=0; i<tmp.size(); i++)
  {
    if (i != pos)
      this->ToolboxList.push_back (tmp[i]);
    else
      this->ToolboxList.push_back (box);
  }
  
  std::vector<vtkMetaDataSet*> list = tmp[pos]->GetMetaDataSets();
  this->ToolboxList[pos]->SetMetaDataSets (list);
}

//----------------------------------------------------------------------------
void vtkKWToolboxManager::PopupToolbox (unsigned int pos)
{
  this->ToolboxFrame->UnpackChildren();
  
  vtkKWToolBox* newbox = this->ToolboxList[pos]->NewInstance();

  this->ToolboxList[pos]->PrepareToPop();
  
  bool firsttime = false;
  
  if (!this->ToolboxTopLevels[pos]->IsCreated())
  {
    this->ToolboxTopLevels[pos]->Create();
    this->ToolboxTopLevels[pos]->SetTitle (newbox->GetName());
    std::ostringstream os2;
    os2 << "PopdownToolbox " << pos;
    this->ToolboxTopLevels[pos]->SetDeleteWindowProtocolCommand(this, os2.str().c_str());
    this->ToolboxTopLevels[pos]->SetDisplayPositionToPointer ();
    firsttime = true;
  }

  this->ToolboxTopLevels[pos]->RemoveAllChildren();
  
  newbox->SetParent (this->ToolboxTopLevels[pos]);
  newbox->SetParentObject(this->GetParentObject());
  newbox->Create();

  this->ToolboxTopLevels[pos]->UnpackChildren();
  
  this->Script("pack %s -side top -anchor nw -fill x ", 
	       newbox->GetWidgetName());

  if (firsttime)
    this->ToolboxTopLevels[pos]->Display();
  else
  {
    this->ToolboxTopLevels[pos]->DeIconify();
    this->ToolboxTopLevels[pos]->Raise();
  }
  
  this->ReplaceToolBox (pos, newbox);

  newbox->Update();

  newbox->Delete();
  
  
}



//----------------------------------------------------------------------------
void vtkKWToolboxManager::PopdownToolbox (unsigned int pos)
{
  this->ToolboxFrame->UnpackChildren();
  this->ToolboxTopLevels[pos]->Withdraw();

  this->ToolboxList[pos]->PrepareToPop();
  
  vtkKWToolBox* newbox = this->ToolboxList[pos]->NewInstance();
  
  newbox->SetParent (this->ToolboxFrame);
  newbox->SetParentObject(this->GetParentObject());
  newbox->Create();

  this->ReplaceToolBox (pos, newbox);
  this->SelectToolboxCallback (pos);

  //this->ToolboxTopLevels[pos]->RemoveAllChildren();
  
  newbox->Delete();

  
}

