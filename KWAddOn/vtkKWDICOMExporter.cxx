/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDICOMExporter.cxx 1 2007-11-21 09:03:23Z ntoussaint $
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
#include <vtkKWDICOMExporter.h>
#include "vtkObjectFactory.h"

#include <vtkKWDataSelectorWidget.h>
#include <vtkKWPushButton.h>
#include <vtkKWLabel.h>
#include <vtkKWLoadSaveDialog.h>
#include "vtkKWSeparator.h"
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>

#include <vtkKWDicomInfoWidget.h>
#include <pixmap/KWAddOnResources.h>
#include <vtkKWIcon.h>
#include <kwcommon.h>
#include <vtkMetaImageData.h>
#include <vtkMetaDataSetSequence.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWDICOMExporter );
vtkCxxRevisionMacro( vtkKWDICOMExporter, "$Revision: 1 $");

//----------------------------------------------------------------------------
vtkKWDICOMExporter::vtkKWDICOMExporter()
{
  this->GDCMExporter = itk::GDCMExporter::New();
  this->SelectionWidget = vtkKWDataSelectorWidget::New();
  this->ButtonEditDICOMInfo = vtkKWPushButton::New();
  this->ButtonPropagateDICOMInfo = vtkKWPushButton::New();
  this->ButtonSetOutputDirectory = vtkKWPushButton::New();
  this->OutputDirectoryLabel = vtkKWLabel::New();
  this->OutputPrefixEntry = 0;

  this->SelectionWidget->SetAllowDataType (vtkMetaDataSet::VTK_META_IMAGE_DATA, true);
  this->SelectionWidget->SetAllowDataType (vtkMetaDataSet::VTK_META_SURFACE_MESH, false);
  this->SelectionWidget->SetAllowDataType (vtkMetaDataSet::VTK_META_VOLUME_MESH, false);
  
}

//----------------------------------------------------------------------------
vtkKWDICOMExporter::~vtkKWDICOMExporter()
{
  this->SelectionWidget->Delete();
  this->ButtonEditDICOMInfo->Delete();
  this->ButtonPropagateDICOMInfo->Delete();
  this->ButtonSetOutputDirectory->Delete();
  this->OutputDirectoryLabel->Delete();
  
}




//----------------------------------------------------------------------------
void vtkKWDICOMExporter::SetDataManager (vtkDataManager* manager)
{
  this->SelectionWidget->SetDataManager(manager);
  
}

//----------------------------------------------------------------------------
vtkDataManager* vtkKWDICOMExporter::GetDataManager (void)
{
  return this->SelectionWidget->GetDataManager();
}



//----------------------------------------------------------------------------
void vtkKWDICOMExporter::CreateWidget()
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


  this->SetTitle ("DICOM exporter");
  this->SetStyleToOkCancel();
  this->SetOKButtonText("Write DICOM exam");
  
  
  this->SelectionWidget->SetParent (this->GetTopFrame());
  this->SelectionWidget->Create();

  this->SelectionWidget->SetWidth (60);
  this->SelectionWidget->SetHeight (40);

  this->Script("pack %s -side top -anchor nw -expand true -fill both", 
	       this->SelectionWidget->GetWidgetName());

  this->ButtonEditDICOMInfo->SetParent (this->GetTopFrame());
  this->ButtonEditDICOMInfo->Create();
  this->ButtonEditDICOMInfo->SetImageToPixels(image_cd, image_cd_width, image_cd_height, image_cd_pixel_size, image_cd_length); 
  this->ButtonEditDICOMInfo->SetCommand(this, "EditDICOMInfoCallback");
  this->ButtonEditDICOMInfo->SetBalloonHelpString("edit highlighted image DICOM information");

  this->Script("pack %s -side right -anchor ne -expand no -padx 5 -pady 5", 
	       this->ButtonEditDICOMInfo->GetWidgetName());

//   this->ButtonPropagateDICOMInfo->SetParent (this->GetTopFrame());
//   this->ButtonPropagateDICOMInfo->Create();
//   this->ButtonPropagateDICOMInfo->SetImageToPixels(image_cd, image_cd_width, image_cd_height, image_cd_pixel_size, image_cd_length); 
//   this->ButtonPropagateDICOMInfo->SetCommand(this, "PropagateDICOMInfoCallback");
//   this->ButtonPropagateDICOMInfo->SetBalloonHelpString("propagate highlighted image DICOM information to all other images of the exam");

//   this->OutputDirectoryLabel->SetParent (this->GetTopFrame());
//   this->OutputDirectoryLabel->Create();

//   this->ButtonSetOutputDirectory->SetParent (this->GetTopFrame());
//   this->ButtonSetOutputDirectory->Create();
//   this->ButtonSetOutputDirectory->SetImageToPixels(image_saveas, image_saveas_width, image_saveas_height, image_saveas_pixel_size, image_saveas_length); 
//   this->ButtonSetOutputDirectory->SetCommand(this, "SetOutputDirectoryCallback");
//   this->ButtonSetOutputDirectory->SetBalloonHelpString("Set the output directory for the DICOM exam");

  vtkKWEntryWithLabel* ewl = vtkKWEntryWithLabel::New();
  ewl->SetParent (this->GetTopFrame());
  ewl->Create();
  ewl->SetLabelText ("File prefix :");
  
  this->OutputPrefixEntry = ewl->GetWidget();
  this->OutputPrefixEntry->SetWidth (20);
  this->OutputPrefixEntry->SetValue ("IMG_");
  this->OutputPrefixEntry->SetCommandTriggerToReturnKeyAndFocusOut ();
  this->OutputPrefixEntry->SetBackgroundColor(1,1,1);
  this->OutputPrefixEntry->ReadOnlyOff();

  this->Script("pack %s -side right -anchor ne -expand no -padx 5 -pady 5", 
	       ewl->GetWidgetName());

  ewl->Delete();
  
  this->Pack();
  
  
}


//----------------------------------------------------------------------------
void vtkKWDICOMExporter::PackSelf()
{


//   this->Script("pack %s -side left -anchor nw -expand no", 
// 	       this->ButtonPropagateDICOMInfo->GetWidgetName());


//   vtkKWSeparator* separator1 = vtkKWSeparator::New();
//   separator1->SetParent(this->GetTopFrame());
//   separator1->Create();
//   this->Script("pack %s -side top -fill x ", separator1->GetWidgetName());
//   separator1->Delete();

  
//   this->Script("pack %s -side top -anchor se -expand no", 
// 	       this->ButtonSetOutputDirectory->GetWidgetName());

  
}

//----------------------------------------------------------------------------
void vtkKWDICOMExporter::EditDICOMInfoCallback()
{

  
  vtkMetaImageData* imagedata = vtkMetaImageData::SafeDownCast(this->SelectionWidget->GetHighlightedDataSet());
  vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast( this->SelectionWidget->GetHighlightedDataSet() );
    
  if (sequence)
  {
    imagedata = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet (0));
  }

  if (!imagedata)  
    return;
  
  
  vtkKWMessageDialog* toplevel = vtkKWMessageDialog::New();
  
  toplevel->SetMasterWindow(this);
  toplevel->SetApplication(this->GetApplication());
  toplevel->Create();
  toplevel->SetTitle(imagedata->GetName());
  toplevel->SetStyleToOkCancel();
  toplevel->SetResizable(1,1);
  
  vtkKWDicomInfoWidget* widget = vtkKWDicomInfoWidget::New();
  widget->SetParent(toplevel->GetTopFrame());
  widget->Create();
  this->Script("pack %s -side top -expand no", 
	       widget->GetWidgetName());
  

  widget->SetDicomTagListAsDictionary(imagedata->GetDicomDictionary());

  widget->Delete();
  
  toplevel->SetPosition (100,100);
  toplevel->Invoke();


  if (toplevel->GetStatus() != vtkKWDialog::StatusOK)
  {
    toplevel->Delete();
    return;
  }


  if (sequence)
  {
    vtkKWPopupWarningMessage (this, "Unfortunatelly there is no possibility \nto set DICOM flags to all frames of a sequence.");
    
//     for (unsigned int i=0; i<sequence->GetNumberOfMetaDataSets(); i++)
//     {
//       vtkMetaImageData* frame = vtkMetaImageData::SafeDownCast(sequence->GetMetaDataSet (i));
//       if (!frame)
// 	continue;
      
//       frame->SetDicomDictionary( widget->GetDicomTagListAsDictionary() );
//     }
  }

  imagedata->SetDicomDictionary( widget->GetDicomTagListAsDictionary() );
  toplevel->Delete();

}

//----------------------------------------------------------------------------
void vtkKWDICOMExporter::PropagateDICOMInfoCallback()
{}

//----------------------------------------------------------------------------
void vtkKWDICOMExporter::SetOutputDirectoryCallback()
{

}



  
//----------------------------------------------------------------------------
int vtkKWDICOMExporter::PreInvoke()
{
  int ret = this->Superclass::PreInvoke();
  if(!ret)
    return 0;

  this->SetResizable (1,1);

  this->SelectionWidget->Initialize();
  return 1;
}

//----------------------------------------------------------------------------
void vtkKWDICOMExporter::PostInvoke()
{


  this->Superclass::PostInvoke();

  std::vector<vtkMetaDataSet*> selection = this->SelectionWidget->GetSelection();
  
  if ((this->Done == vtkKWDialog::StatusCanceled) || !selection.size())
    return;

  
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->ChooseDirectoryOn();
  
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Choose an output directory...");

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }
  
  const char* directory = dialog->GetFileName();

  this->GDCMExporter->SetOutputDirectory(directory);

  std::vector<itk::GDCMExporter::ImageType::Pointer> imagelist;
    
  for (unsigned int index=0; index < selection.size(); index++)
  {
    
    vtkMetaImageData* imagedata = vtkMetaImageData::SafeDownCast( selection[index] );
    vtkMetaDataSetSequence* sequence = vtkMetaDataSetSequence::SafeDownCast( selection[index] );
    
    if (sequence)
    {
      for (int j=0; j<sequence->GetNumberOfMetaDataSets(); j++)
      {
	vtkMetaImageData* frame = vtkMetaImageData::SafeDownCast( sequence->GetMetaDataSet (j) );
	if (!frame)
	  continue;
	
	itk::GDCMExporter::ImageType::Pointer image;
	
	bool isvalid = frame->GetItkImage<itk::GDCMExporter::ImageComponentType>(image);    
	
	if (!isvalid || image.IsNull())
	  continue;
	
	image->SetMetaDataDictionary(frame->GetDicomDictionary());
	imagelist.push_back (image);
      }
    }
    else if (imagedata)
    {
      itk::GDCMExporter::ImageType::Pointer image;
      
      bool isvalid = imagedata->GetItkImage<itk::GDCMExporter::ImageComponentType>(image);    
      
      if (!isvalid || image.IsNull())
	break;
      
      image->SetMetaDataDictionary(imagedata->GetDicomDictionary());
      imagelist.push_back (image);
    }
  }

  for (unsigned int i=0; i<imagelist.size(); i++)
  {
    this->GDCMExporter->AddVolume (imagelist[i]); 
  }

  this->GDCMExporter->SetFilePrefix(this->OutputPrefixEntry->GetValue());
  this->GDCMExporter->WriteAll();
  
  dialog->Delete();
}
