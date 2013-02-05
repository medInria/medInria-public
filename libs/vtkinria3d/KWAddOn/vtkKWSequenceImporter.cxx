/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWSequenceImporter.cxx 1006 2008-11-13 15:58:19Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-11-13 15:58:19 +0000 (Thu, 13 Nov 2008) $
Version:   $Revision: 1006 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkKWSequenceImporter.h>
#include "vtkObjectFactory.h"

#include <vtkKWSequenceEditorWidget.h>
#include <vtkKWPushButton.h>
#include <vtkMetaDataSetSequence.h>
#include <vtkKWLoadSaveDialog.h>
#include "vtkKWSeparator.h"
#include <vtkStringArray.h>
#include <pixmap/KWAddOnResources.h>
#include <vtkKWIcon.h>
#include <kwcommon.h>
#include <vtkMetaImageData.h>
#include <vtkMetaSurfaceMesh.h>
#include <vtkMetaVolumeMesh.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWWindowBase.h>
#include <vtkKWProgressGauge.h>
#include "vtkKWInternationalization.h"
#include <vtkKWApplication.h>
//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWSequenceImporter );
vtkCxxRevisionMacro( vtkKWSequenceImporter, "$Revision: 1006 $");

//----------------------------------------------------------------------------
vtkKWSequenceImporter::vtkKWSequenceImporter()
{
  this->SequenceEditorWidget = vtkKWSequenceEditorWidget::New();
  this->ButtonAddFromFile = vtkKWPushButton::New();
  this->ButtonReadAttributes = vtkKWPushButton::New();
  this->ButtonAddFromManager = vtkKWPushButton::New();
  
}

//----------------------------------------------------------------------------
vtkKWSequenceImporter::~vtkKWSequenceImporter()
{
  this->SequenceEditorWidget->Delete();
  this->ButtonAddFromFile->Delete();
  this->ButtonReadAttributes->Delete();
  this->ButtonAddFromManager->Delete();
  
}

//----------------------------------------------------------------------------
void vtkKWSequenceImporter::SetSequence (vtkMetaDataSetSequence* sequence)
{
  this->SequenceEditorWidget->SetSequence(sequence);
}

//----------------------------------------------------------------------------
vtkMetaDataSetSequence* vtkKWSequenceImporter::GetSequence (void)
{
  return this->SequenceEditorWidget->GetSequence();
}



//----------------------------------------------------------------------------
void vtkKWSequenceImporter::CreateWidget()
{
  // Check if already created
  if (this->IsCreated())
  {
    vtkErrorMacro("class already created");
    return;
  }
  
  // Call the superclass to create the whole widget
  this->Superclass::CreateWidget();
  
  this->SetTitle ("Sequence Importer");
  this->SetStyleToOkCancel();
  
  this->SetBorderWidth(2);
  this->SetReliefToFlat();

  vtkKWFrame* tmp1 = vtkKWFrame::New();
  vtkKWFrame* tmp2 = vtkKWFrame::New();
  tmp1->SetParent(this->GetTopFrame());
  tmp2->SetParent(this->GetTopFrame());
  tmp1->Create();
  tmp2->Create();
  
  
  this->SequenceEditorWidget->SetParent (tmp2);
  this->SequenceEditorWidget->Create();

  this->ButtonAddFromFile->SetParent (tmp1);
  this->ButtonAddFromFile->Create();
  this->ButtonAddFromFile->SetImageToPixels(image_open, image_open_width, image_open_height, image_open_pixel_size, image_open_length); 
  this->ButtonAddFromFile->SetCommand(this, "AddFromFileCallback");
  this->ButtonAddFromFile->SetBalloonHelpString("Add file(s) -frames- to sequence ");

  this->ButtonReadAttributes->SetParent (tmp1);
  this->ButtonReadAttributes->Create();
  this->ButtonReadAttributes->SetImageToPixels(image_attributesopen, image_attributesopen_width, image_attributesopen_height, image_attributesopen_pixel_size, image_attributesopen_length); 
  this->ButtonReadAttributes->SetCommand(this, "ReadAttributesCallback");
  this->ButtonReadAttributes->SetBalloonHelpString("load some attrbutes files (global or frame attributes) to the sequence ");

  this->Script("pack %s -side top -anchor nw -expand no -padx 0 -pady 0", 
	       tmp1->GetWidgetName());
  this->Script("pack %s -side top -anchor nw -expand t -fill both -padx 0 -pady 0", 
	       tmp2->GetWidgetName());
  
  this->Script("pack %s -side left -anchor nw -expand no -padx 4 -pady 4", 
	       this->ButtonAddFromFile->GetWidgetName());
  this->Script("pack %s -side left -anchor nw -expand no -padx 4 -pady 4", 
	       this->ButtonReadAttributes->GetWidgetName());


    
  this->Script("pack %s -side top -anchor nw -expand t -fill both -padx 4 -pady 4", 
	       this->SequenceEditorWidget->GetWidgetName());


  tmp1->Delete();
  tmp2->Delete();
  
  this->PackSelf();
  
  
}


//----------------------------------------------------------------------------
void vtkKWSequenceImporter::PackSelf()
{

}

//----------------------------------------------------------------------------
void vtkKWSequenceImporter::AddFromFileCallback()
{
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOn();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Open a File");
  dialog->SetFileTypes (kwsupportedformats.c_str());

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }

  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (!win)
    return;
  
  win->SetStatusText(ks_("Progress|Opening files..."));
  win->GetProgressGauge()->SetValue(0);

  double time = this->GetSequence()->GetMaxTime() + this->GetSequence()->GetTimeResolution();
  
  double step = 0.1;
  if (this->GetSequence()->GetTimeResolution() > 0)
    step = this->GetSequence()->GetTimeResolution();
      
  bool all_ok = true;
  
  for (int i=0; i<dialog->GetNumberOfFileNames(); i++)
  {
    win->GetProgressGauge()->SetValue((int)(100.0 * i / dialog->GetNumberOfFileNames()));
     
    const char* filename = dialog->GetFileNames()->GetValue (i);
//     const char* path = dialog->GetLastPath();
    
    try
    {
      this->GetSequence()->ReadAndAddFile(filename, time + (double)(i)*step);
    }
    catch (vtkErrorCode::ErrorIds)
    {
      // we don't throw exception as we want to continue scanning the directory
      all_ok = false;
      continue;
    }    
  }

  vtksys_stl::string end_msg(win->GetStatusText());
  end_msg += " -- ";
  end_msg += ks_("Progress|Done");  
  win->SetStatusText(end_msg.c_str());
  win->GetProgressGauge()->SetValue(0);

  if (!all_ok)
  {
    vtkKWMessageDialog::PopupMessage(this->GetApplication(),
				     this->GetParentTopLevel(),
				     "Error",
				     "One or more file(s) could not be opened !\nPlease check the log window for details.",
				     vtkKWMessageDialog::ErrorIcon);
  }
  
  this->SequenceEditorWidget->Initialize();
  

  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
}




//----------------------------------------------------------------------------
void vtkKWSequenceImporter::ReadAttributesCallback()
{
  if (!this->GetSequence())
    return;
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOn();
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Apply list of attributes");
  dialog->SetFileTypes (kwsupportedattributesformats.c_str());

  if ( dialog->Invoke () == 0  || (!dialog->GetNumberOfFileNames()) )
  {
    dialog->Delete();
    return ;
  }


  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (!win)
    return;
  
  win->SetStatusText(ks_("Progress|Opening files..."));
  win->GetProgressGauge()->SetValue(0);
  

  const char* firstfile = dialog->GetFileNames ()->GetValue (0);
  std::string ext = vtksys::SystemTools::GetFilenameLastExtension (firstfile);
  
  bool all_ok = true;
  
  for (int i=0; i<dialog->GetNumberOfFileNames(); i++)
  {
    win->GetProgressGauge()->SetValue((int)(100.0 * i / dialog->GetNumberOfFileNames()));
    
    const char* filename = dialog->GetFileNames ()->GetValue (i);
    
    try
    {
      if (strcmp (ext.c_str(), ".scl") == 0 )
      {
	if (i == 0)
	  this->GetSequence()->ReadData (filename);
	this->GetSequence()->ReadFrameData (filename, i);
      }
      else
      {
	this->GetSequence()->ReadData (filename);
      }
      
    }
    catch (vtkErrorCode::ErrorIds)
    {
      // we don't throw exception as we want to continue scanning the directory
      all_ok = false;      
      continue;  
    }
  }

  
  vtksys_stl::string end_msg(win->GetStatusText());
  end_msg += " -- ";
  end_msg += ks_("Progress|Done");  
  win->SetStatusText(end_msg.c_str());
  win->GetProgressGauge()->SetValue(0);

  if (!all_ok)
  {
    vtkKWMessageDialog::PopupMessage(this->GetApplication(),
				     this->GetParentTopLevel(),
				     "Error",
				     "One or more file(s) could not be opened !\nPlease check the log window for details.",
				     vtkKWMessageDialog::ErrorIcon);
  }
  

  this->SequenceEditorWidget->Initialize();
  
  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
}

  
//----------------------------------------------------------------------------
int vtkKWSequenceImporter::PreInvoke()
{
  int ret = this->Superclass::PreInvoke();
  if(!ret)
    return 0;

  this->SetResizable (1,1);
  this->SetOKButtonText("Import");


  if (!this->GetSequence())
  {
    vtkErrorMacro(<<"cannot invoke the sequence importer whithout feeding it with a sequence !"<<endl);
    return 0;
  }
  
  this->SequenceEditorWidget->Initialize();
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWSequenceImporter::PostInvoke()
{
  this->Superclass::PostInvoke();

  
  if (this->Done == vtkKWDialog::StatusCanceled)
    return;

  if (this->GetSequence()->GetNumberOfMetaDataSets() < 1)
  {
    this->Done = vtkKWDialog::StatusCanceled;
    return;
  }
  
  

  this->SequenceEditorWidget->TransposeDataToSequence();
  

}
