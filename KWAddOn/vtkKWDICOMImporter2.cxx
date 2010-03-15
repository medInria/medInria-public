/*=========================================================================

  Program:   vtkINRIA3D
  Module:    $Id: vtkKWDICOMImporter2.cxx 1 2007-11-21 09:03:23Z ntoussaint $
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
#include <vtkKWDICOMImporter2.h>
#include "vtkObjectFactory.h"

#include <vtkKWPushButton.h>
#include <vtkKWLabel.h>
#include <vtkKWLoadSaveDialog.h>
#include "vtkKWSeparator.h"

#include <vtkKWDicomInfoWidget.h>
#include <pixmap/KWAddOnResources.h>
#include <vtkKWIcon.h>
#include <kwcommon.h>
#include <vtkMetaImageData.h>

#include "vtkKWIcon.h"
#include "vtkKWMessageWithLabel.h"
#include "vtkKWPushButton.h"
#include "vtkKWFrame.h"
#include <vtkKWLoadSaveDialog.h>
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWTree.h>
#include "vtkKWInternationalization.h"
#include "vtkKWRenderWidget.h"
#include <vtkKWCheckButtonWithLabel.h>
#include <vtkKWCheckButton.h>
#include <vtkKWSimpleEntryDialog.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWMenuButton.h>
#include <vtkKWMenu.h>
#include <vtkTimerLog.h>
#include <vtkStringArray.h>
#include <vtkKWDicomInfoWidget.h>
#include <vtkKWTkUtilities.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include "vtkViewImage2D.h"
#include <vtkDICOMImageReader.h>

#include <vtksys/stl/string>
#include <vtksys/stl/vector>
#include <vtksys/SystemTools.hxx>
#include <sstream>

#include <kwcommon.h>

#include <vtkKWWindowBase.h>
#include <vtkKWProgressGauge.h>
#include <vtkKWApplication.h>

#include <vtkKWMultiColumnList.h>
#include <vtkKWMultiColumnListWithScrollbars.h>
#include <vtkKWEntryWithLabel.h>
#include <vtkKWEntry.h>
#include <vtkKWFrameWithLabel.h>

#include <vtkMetaDataSetSequence.h>
#include <itkImageSeriesReshapeFilter.h>
#include <vtkKWPushButtonWithMenu.h>
#include <vtkKWMenu.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWDICOMImporter2 );
vtkCxxRevisionMacro( vtkKWDICOMImporter2, "$Revision: 1 $");

//----------------------------------------------------------------------------
vtkKWDICOMImporter2::vtkKWDICOMImporter2()
{

  this->GDCMImporter = ImporterType::New();
  this->GDCMImporter->RecursiveScanOn();

  this->Preview      = vtkViewImage2D::New();
  this->RenderWidget = vtkKWRenderWidget::New();
  
//   this->OpenDirectoryButton   = vtkKWPushButton::New();
  this->ResetButton           = vtkKWPushButton::New();
  this->RemoveVolumeButton    = vtkKWPushButton::New();
  this->AutoOrganizeButton     = vtkKWMenuButton::New();
  this->OnceVolumeButton      = vtkKWPushButton::New();

  this->RecursiveCheckButton  = vtkKWCheckButtonWithLabel::New();

  this->TopFrame    = vtkKWFrame::New();
  this->BottomFrame = vtkKWFrame::New();
  this->MLeftFrame  = vtkKWFrame::New();
  this->MRightFrame = vtkKWFrame::New();
  this->ButtonFrame = vtkKWFrame::New();

  this->InteractiveMode = 0;

  this->InteractiveModeButton = vtkKWCheckButtonWithLabel::New();
  
  this->MultiColumnList = NULL;

  this->SequenceFrame = vtkKWFrameWithLabel::New();
  this->SequenceCheckButton = vtkKWCheckButtonWithLabel::New();
  this->SequenceDurationEntry = vtkKWEntryWithLabel::New();

  this->InteractiveStatus = STATUS_DISABLED;
  this->InteractiveVolume = 0;
  
  

}

//----------------------------------------------------------------------------
vtkKWDICOMImporter2::~vtkKWDICOMImporter2()
{


  this->Preview->Delete();
  this->RenderWidget->SetParent (NULL);
  this->RenderWidget->Delete();
  
  this->ResetButton->Delete();
  this->RemoveVolumeButton->Delete();
  this->AutoOrganizeButton->Delete();
  this->OnceVolumeButton->Delete();
  this->RecursiveCheckButton->Delete();

  this->TopFrame->Delete();
  this->BottomFrame->Delete();
  this->MLeftFrame->Delete();
  this->MRightFrame->Delete();
  this->ButtonFrame->Delete();

  this->InteractiveModeButton->Delete();

  this->SequenceFrame->Delete();
  this->SequenceCheckButton->Delete();
  this->SequenceDurationEntry->Delete();
  
  this->InteractiveStatus = STATUS_DISABLED;
}




//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::CreatePreview()
{
  this->RenderWidget->SetParent(this->MRightFrame);
  this->RenderWidget->Create();
  
  this->RenderWidget->SetWidth(150);
  this->RenderWidget->SetHeight(150);

  
  
  this->Preview->SetRenderWindow(this->RenderWidget->GetRenderWindow());
  this->Preview->SetRenderer(this->RenderWidget->GetRenderer());
  this->Preview->SetRenderWindowInteractor(this->RenderWidget->GetRenderWindow()->GetInteractor());
  this->Preview->SetOrientation(vtkViewImage::AXIAL_ID);
  this->Preview->SetInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);

  this->Preview->SetAboutData ("");
  this->Preview->SetBackgroundColor (0,0,0);
  this->Preview->SetShowAnnotations (false);
  this->Preview->SetShowDirections(false);
  
  
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::CreateMultiColumnList()
{

  vtkKWMultiColumnListWithScrollbars* mcl = vtkKWMultiColumnListWithScrollbars::New();
  mcl->SetParent(this->BottomFrame);
  mcl->Create();
  mcl->HorizontalScrollbarVisibilityOff();
  this->Script("pack %s -side left -expand true -fill both -anchor nw -padx 20 -pady 5",
	       mcl->GetWidgetName());
  
  this->MultiColumnList = mcl->GetWidget();
  
  this->MultiColumnList->SetReliefToGroove ();
  this->MultiColumnList->SetBorderWidth (3);

  this->MultiColumnList->MovableColumnsOff();
  this->MultiColumnList->SetSelectionModeToExtended ();
  this->MultiColumnList->SetSelectionBackgroundColor(1,0.9647,0.6314);  
  this->MultiColumnList->SetSelectionForegroundColor (0,0,0);

  int col_index;
  
  // Add the columns (make some of them editable)
  
  col_index = this->MultiColumnList->AddColumn("Volume");
  this->MultiColumnList->SetColumnEditable(col_index, 0);  
  this->MultiColumnList->SetColumnWidth(col_index, 30);
  
  col_index = this->MultiColumnList->AddColumn("Size");
  this->MultiColumnList->SetColumnEditable(col_index, 0);

  col_index = this->MultiColumnList->AddColumn("Sequence 2D+t");
  this->MultiColumnList->SetColumnEditable(col_index, 0);
  this->MultiColumnList->SetColumnFormatCommandToEmptyOutput(col_index);
  this->MultiColumnList->SetColumnAlignmentToCenter(col_index);  
      
  
  this->MultiColumnList->SetSelectionChangedCommand (this, "SelectionChangedCallback");
  this->MultiColumnList->SetCellUpdatedCommand (this, "CellUpdateCallback");
  this->MultiColumnList->SetUneditableCellDoubleClickCommand(this, "DoubleClickCallback");
  
  this->MultiColumnList->SetWidth(53);
  this->MultiColumnList->SetHeight(15);

  mcl->Delete();
  
  
  

}



//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::CreateButtons ()
{

  this->ButtonFrame->SetParent(this->TopFrame);
  this->ButtonFrame->Create();


  int width2 = 10;

  vtkKWPushButtonWithMenu* menubutton= vtkKWPushButtonWithMenu::New();
  menubutton->SetParent(this->ButtonFrame);
  menubutton->Create();
  menubutton->GetPushButton()->SetImageToPixels(image_fileopen, image_fileopen_width, image_fileopen_height, image_fileopen_pixel_size, image_fileopen_length);
  menubutton->GetPushButton()->SetCommand(this, "OpenDirectoryCallback");
  menubutton->GetPushButton()->SetBalloonHelpString("Open the DICOM root directory");
  this->Script ("pack %s -side left -anchor nw -expand no -padx 3 -pady 3",
		menubutton->GetWidgetName());
  
  vtkKWMenu *menu = menubutton->GetMenu();
  menu->AddCommand("Open DICOM root directory...", this, "OpenDirectoryCallback");
  menu->AddCommand("Open single DICOM file...", this, "OpenFileCallback");
  menu->SetBalloonHelpString("Open the DICOM root directory");
  menubutton->Delete();

//   this->OpenDirectoryButton->SetParent(this->ButtonFrame);
//   this->OpenDirectoryButton->Create();
//   this->OpenDirectoryButton->SetText(ks_("DICOM Importer|Button|Open the DICOM root directory"));
//   this->OpenDirectoryButton->SetBalloonHelpString(this->OpenDirectoryButton->GetText());
//   this->OpenDirectoryButton->SetImageToPixels(image_cdload, image_cdload_width, image_cdload_height, image_cdload_pixel_size, image_cdload_length);
//   this->OpenDirectoryButton->SetWidth(width);
//   this->OpenDirectoryButton->SetCommand(this, "OpenDirectoryCallback");

//   vtkKWPushButton* openfilebutton = vtkKWPushButton::New();
  
//   openfilebutton->SetParent(this->ButtonFrame);
//   openfilebutton->Create();
//   openfilebutton->SetText(ks_("DICOM Importer|Button|Open a single file..."));
//   openfilebutton->SetBalloonHelpString(openfilebutton->GetText());
//   openfilebutton->SetImageToPixels(image_fileopen, image_fileopen_width, image_fileopen_height, image_fileopen_pixel_size, image_fileopen_length);
//   openfilebutton->SetWidth(width);
//   openfilebutton->SetCommand(this, "OpenFileCallback");

//   this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 5 -pady 2",
// 	       openfilebutton->GetWidgetName());
//   openfilebutton->Delete();
  
  
  this->ResetButton->SetParent(this->ButtonFrame);
  this->ResetButton->Create();
  this->ResetButton->SetText(ks_("Reset"));
  this->ResetButton->SetBalloonHelpString(ks_("Reset to the initial configuration"));
  this->ResetButton->SetWidth(width2);
  this->ResetButton->SetCommand(this, "ResetCallback");

  this->RemoveVolumeButton->SetParent(this->ButtonFrame);
  this->RemoveVolumeButton->Create();
  this->RemoveVolumeButton->SetText( ks_("Remove"));
  this->RemoveVolumeButton->SetBalloonHelpString( ks_("Remove this volume of the list"));
  this->RemoveVolumeButton->SetWidth(width2);
  this->RemoveVolumeButton->SetCommand(this, "RemoveVolumeCallback");

  this->AutoOrganizeButton->SetParent(this->ButtonFrame);
  this->AutoOrganizeButton->Create();
  this->AutoOrganizeButton->SetValue(ks_("Auto Organization"));
  this->AutoOrganizeButton->SetBalloonHelpString("Choose between options for the automatic re-organization of volumes");
  
  vtkKWMenu *menu2 = this->AutoOrganizeButton->GetMenu();
  menu2->AddCommand("Default", this, "AutoOrganizeCallback");
  menu2->AddCommand("Position Based", this, "AutoOrganizePositionBasedCallback");
  menu2->SetBalloonHelpString("Will attempt to merge significantly similar instances, \nKeeping coherence in size and orientation");

//   this->AutoOrganizeButton->SetParent(this->ButtonFrame);
//   this->AutoOrganizeButton->Create();
//   this->AutoOrganizeButton->SetText(ks_("Auto Reorganization"));
//   this->AutoOrganizeButton->SetBalloonHelpString(ks_("Split the volume by position consistency"));
//   this->AutoOrganizeButton->SetWidth(width2);
//   this->AutoOrganizeButton->SetCommand(this, "AutoOrganizeCallback");

  this->OnceVolumeButton->SetParent(this->ButtonFrame);
  this->OnceVolumeButton->Create();
  this->OnceVolumeButton->SetText(ks_("Select"));
  this->OnceVolumeButton->SetBalloonHelpString(ks_("Select volume(s)"));
  this->OnceVolumeButton->SetWidth(width2);
  this->OnceVolumeButton->SetCommand(this, "OnceVolumeCallback");

  this->InteractiveModeButton->SetParent(this->MRightFrame);
  this->InteractiveModeButton->Create();
  this->InteractiveModeButton->SetLabelText("Interactive");
  this->InteractiveModeButton->GetWidget()->Deselect();
  this->InteractiveModeButton->GetWidget()->SetCommand(this, "InteractiveModeCallback");
  
  this->SequenceFrame->SetParent(this->MRightFrame);
  this->SequenceFrame->Create();
  this->SequenceFrame->SetLabelText("3D+T Sequence");
  this->SequenceFrame->SetBorderWidth(2);
  this->SequenceFrame->SetWidth(50);
  
  this->SequenceCheckButton->SetParent(this->SequenceFrame->GetFrame());
  this->SequenceCheckButton->Create();
  this->SequenceCheckButton->SetLabelText("Image Set is a 3D+T Sequence");
  this->SequenceCheckButton->SetLabelPositionToRight();
  this->SequenceCheckButton->GetWidget()->Deselect();
  this->SequenceCheckButton->GetWidget()->SetCommand(this, "SequenceCheckButtonCallback");

  this->SequenceDurationEntry->SetParent (this->SequenceFrame->GetFrame());
  this->SequenceDurationEntry->Create();
  this->SequenceDurationEntry->SetLabelPositionToRight ();
  this->SequenceDurationEntry->SetLabelText("s.");
  this->SequenceDurationEntry->GetWidget()->SetWidth(12);
  this->SequenceDurationEntry->GetWidget()->SetValue("duration...");
  this->SequenceDurationEntry->GetWidget()->SetBackgroundColor(1,1,1);
  this->SequenceDurationEntry->GetWidget()->ReadOnlyOff();
  this->SequenceDurationEntry->GetWidget()->SetRestrictValueToDouble();
  this->SequenceDurationEntry->GetWidget()->SetStateToDisabled();

  
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::CreateWidget()
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

  this->SetTitle ("DICOM importer");
  this->SetStyleToOkCancel();
  this->SetOKButtonText("Import");

  this->MLeftFrame->SetParent(this->GetTopFrame());
  this->MLeftFrame->Create();

  this->MRightFrame->SetParent(this->GetTopFrame());
  this->MRightFrame->Create();

  this->TopFrame->SetParent(this->MLeftFrame);
  this->TopFrame->Create();

  this->BottomFrame->SetParent(this->MLeftFrame);
  this->BottomFrame->Create();
  
  this->CreateMultiColumnList();
  this->CreateButtons();
  this->CreatePreview();
  std::cout<<"pack..."<<std::endl;
  
  this->PackSelf();
  
  

}


//-----------------------------------------------------------------------------
void vtkKWDICOMImporter2::PackSequenceFrame (void)
{


  this->Script( "pack %s -side top -expand false -anchor nw -fill none -padx 5 -pady 5", 
		this->SequenceCheckButton->GetWidgetName());
  this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		this->SequenceDurationEntry->GetWidgetName());

  this->SequenceFrame->CollapseFrame();
  
}

//-----------------------------------------------------------------------------
void vtkKWDICOMImporter2::PackSelf()
{

  
  

  this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 10 -pady 2",
	       this->ButtonFrame->GetWidgetName());
//   this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 5 -pady 2",
// 	       this->OpenDirectoryButton->GetWidgetName());
  this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 5 -pady 2",
	       this->ResetButton->GetWidgetName());
  this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 5 -pady 2",
	       this->RemoveVolumeButton->GetWidgetName());
  this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 5 -pady 2",
	       this->AutoOrganizeButton->GetWidgetName());
  this->Script("pack %s -side left -expand false -anchor sw -fill none -padx 5 -pady 2",
	       this->OnceVolumeButton->GetWidgetName());

  if (this->RenderWidget && this->RenderWidget->IsCreated())
  {
    this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		  this->RenderWidget->GetWidgetName());
  }

  if (this->InteractiveModeButton && this->InteractiveModeButton->IsCreated())
  {
    this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		  this->InteractiveModeButton->GetWidgetName());
  }

  this->PackSequenceFrame();
  if (this->SequenceFrame && this->SequenceFrame->IsCreated())
  {
    this->Script( "pack %s -side top -expand false -anchor ne -fill none -padx 5 -pady 5", 
		  this->SequenceFrame->GetWidgetName());
  }
  
  this->Script("grid %s -sticky nwse -column 0 -row 0 -pady 2",
	       this->TopFrame->GetWidgetName());
  this->Script("grid %s -sticky nwse -column 0 -row 1 -pady 2",
	       this->BottomFrame->GetWidgetName());

  this->Script("grid rowconfigure %s 0 -weight 1", this->GetWidgetName());
  this->Script("grid rowconfigure %s 1 -weight 4", this->GetWidgetName());

  this->Script("grid %s -sticky nwse -column 0 -row 0 -pady 2",
	       this->MLeftFrame->GetWidgetName());
  this->Script("grid %s -sticky nwse -column 1 -row 0 -pady 2",
	       this->MRightFrame->GetWidgetName());

  this->Script("grid columnconfigure %s 0 -weight 4", this->GetWidgetName());
  this->Script("grid columnconfigure %s 1 -weight 1", this->GetWidgetName());


}



//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::Update()
{
  this->UpdateMultiColumnList();
  
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::UpdateMultiColumnList()
{

  if (!this->MultiColumnList->IsCreated())
    return;

  this->MultiColumnList->DeleteAllRows();

  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (win)
  {
    win->SetStatusText(ks_("Progress|Updating DICOM tree..."));
  }
  
  if (win)
  {
    win->GetProgressGauge()->SetValue(0);
  }
  
  for (unsigned int i=0; i<this->GDCMImporter->GetNumberOfOutputs(); i++)
  {
    unsigned int id_to_insert = this->MultiColumnList->GetNumberOfRows();
    
    if (win)
    {
      win->GetProgressGauge()->SetValue((int)(100.0 * i / this->GDCMImporter->GetNumberOfOutputs()));
    }
    
    DICOMImageType::Pointer image =
      static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (i));
    if (image.IsNull())
      continue;
    
    std::string name = image->GetName();
    unsigned int nfiles = image->GetFileList()->size();
    unsigned int dim[3] = {0,0,0};
    if (nfiles)
    {
      dim[0] = (*image->GetFileList())[0]->GetXSize();
      dim[1] = (*image->GetFileList())[0]->GetYSize();
      dim[2] = nfiles;
    }
    std::ostringstream dimensions;
    dimensions<<dim[0]<<"x"<<dim[1]<<"x"<<dim[2];

    this->MultiColumnList->InsertCellText(id_to_insert, 0, name.c_str());
    this->MultiColumnList->InsertCellText(id_to_insert, 1, dimensions.str().c_str());
//     this->MultiColumnList->InsertCellTextAsInt(i, 2, (int)(dcmvolumelist[i]->IsSequence()));    
    this->MultiColumnList->SetCellWindowCommandToCheckButton(id_to_insert, 2);
    
  }

  if (win)
  {
    vtksys_stl::string end_msg(win->GetStatusText());
    end_msg += " -- ";
    end_msg += ks_("Progress|Done");  
    win->SetStatusText(end_msg.c_str());
    win->GetProgressGauge()->SetValue(0);
  }
}


//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::UpdatePreview()
{
}



//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::OpenFileCallback()
{
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->MultipleSelectionOn();

  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Open DICOM file(s)");

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }
  
  for (int i=0; i<dialog->GetNumberOfFileNames(); i++)
  {
    const char* filename = dialog->GetFileNames()->GetValue (i);
    try
    {
      this->GDCMImporter->ReadFile (filename);
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e<<std::endl;
      vtkErrorMacro(<<"Error when reading file, skipping ("<<filename<<")"<<endl);
      
    }      
  }
  
  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();

  this->Update();

}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::OpenDirectoryCallback()
{
  if ( this->InteractiveStatus == STATUS_PLAY)
    this->InteractiveStatus = STATUS_STOP;
  
  vtkKWLoadSaveDialog *dialog = vtkKWLoadSaveDialog::New() ;
  
  dialog->SetParent(this);
  dialog->Create();
  dialog->ChooseDirectoryOn();
  
  dialog->RetrieveLastPathFromRegistry("DataPath");
  dialog->SetTitle ("Open a DICOM exam root directory");

  if ( dialog->Invoke () == 0 )
  {
    dialog->Delete();
    return ;
  }
  
  const char* directory = dialog->GetFileName();
  
  this->GDCMImporter->RecursiveScanOn();
  this->GDCMImporter->SetInputDirectory(directory);

  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (win)
  {
    win->SetStatusText(ks_("Progress|Scanning DICOMs..."));
  }

  if (win)
  {
    win->GetProgressGauge()->SetValue(10);
  }
  
  try
  {
    std::cout<<"scanning...  ";
    this->GDCMImporter->Scan();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
    return;
    
  }
  std::cout<<"done."<<std::endl;
  
  if (win)
  {
    vtksys_stl::string end_msg(win->GetStatusText());
    end_msg += " -- ";
    end_msg += ks_("Progress|Done");  
    win->SetStatusText(end_msg.c_str());
    win->GetProgressGauge()->SetValue(0);
  }
  
  this->Update();
  
  dialog->SaveLastPathToRegistry("DataPath");
  dialog->Delete();
  
  
  

}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::ResetCallback()
{

  if ( this->InteractiveStatus == STATUS_PLAY)
    this->InteractiveStatus = STATUS_STOP;
  try
  {
    this->GDCMImporter->InitializeOutputs();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
    return;
    
  }

  this->Update();

}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::RemoveVolumeCallback()
{

  if ( this->InteractiveStatus == STATUS_PLAY)
    this->InteractiveStatus = STATUS_STOP;

  const unsigned int Number_Of_Selected = this->MultiColumnList->GetNumberOfSelectedRows();
  int *indices = new int[Number_Of_Selected];
  this->MultiColumnList->GetSelectedRows(indices);
  this->MultiColumnList->ClearSelection();
  std::vector<DICOMImageType::Pointer> dcmlist;
    
  for (unsigned int i=0; i<Number_Of_Selected; i++)
  {
    DICOMImageType::Pointer image =
      static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (indices[i]));
    dcmlist.push_back(image);      
  }

  for (unsigned int i=0; i<dcmlist.size(); i++)
  {
    this->GDCMImporter->RemoveDICOMImage (dcmlist[i]);
  }
  delete [] indices;

  this->Update();
      
  
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::AutoOrganizeCallback()
{
  try
  {
    this->GDCMImporter->AutoOrganization();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }

  
  this->Update();
}


//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::AutoOrganizePositionBasedCallback()
{
  try
  {
    this->GDCMImporter->AutoOrganizationPositionBased();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }
  
  this->Update();
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::OnceVolumeCallback()
{

  if ( this->InteractiveStatus == STATUS_PLAY)
    this->InteractiveStatus = STATUS_STOP;
  
  const unsigned int Number_Of_Selected = this->MultiColumnList->GetNumberOfSelectedRows();
    
  int* indices = new int[Number_Of_Selected];
  this->MultiColumnList->GetSelectedRows(indices);
  this->MultiColumnList->ClearSelection();
  std::vector<DICOMImageType::Pointer> dcmlist;
  
  for (unsigned int i=0; i<this->GDCMImporter->GetNumberOfOutputs(); i++)
  {
    bool isselected = false;
      
    for (unsigned int j=0; j<Number_Of_Selected; j++)
    {
      if (indices[j] == (int)i)
      {
	isselected = true;
	break;
      }
    }
    if (!isselected)
    {
      DICOMImageType::Pointer image =
	static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (i));
      dcmlist.push_back(image);
    }
  }

  for (unsigned int i=0; i<dcmlist.size(); i++)
    this->GDCMImporter->RemoveDICOMImage (dcmlist[i]);  

  this->Update();

  delete [] indices;
}


//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::SelectionChangedCallback()
{

  const unsigned int Number_Of_Selected = this->MultiColumnList->GetNumberOfSelectedRows();
  if (Number_Of_Selected != 1)
  {
    if ( this->InteractiveStatus != STATUS_DISABLED)
      this->InteractiveStatus = STATUS_STOP;
    
    return;
  }
  
  int indices[1];
  this->MultiColumnList->GetSelectedRows(indices);
    
  if ( this->InteractiveStatus != STATUS_DISABLED)
  {
    this->StartInteractive();
    return;
  }
  
  DICOMImageType::Pointer image =
    static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (indices[0]));
  if (!image.IsNull())
    this->UpdatePreview(image);

  this->InteractiveVolume = indices[0];

}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::InteractiveModeCallback(int val)
{
  this->InteractiveMode = val;
  if (val)
  {
    this->InteractiveStatus = STATUS_PLAY;
    this->StartInteractive();
  }
  else
  {
    this->InteractiveStatus = STATUS_DISABLED;
    this->StopInteractive();
  }
  
}


//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::StartInteractive()
{

  const unsigned int Number_Of_Selected = this->MultiColumnList->GetNumberOfSelectedRows();
  if (Number_Of_Selected != 1)
  {
    if ( this->InteractiveStatus != STATUS_DISABLED)
      this->InteractiveStatus = STATUS_STOP;
    
    return;
  }

  int indices[1];
  this->MultiColumnList->GetSelectedRows(indices);
  
  DICOMImageType::Pointer image =   
    static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (indices[0]));
  
  if (image.IsNull())
    return;

  try
  {
//     image->DisconnectPipeline();
    image->Build();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr<< e <<std::endl;
    vtkErrorMacro (<<"cannot build DICOM volume !"<<endl);
    return;
  }
  
  this->Preview->SetITKImage (static_cast<ImageType*>(image));
  this->Preview->Reset();
  this->Preview->Render();    
  
  unsigned int currentvolumeid = this->InteractiveVolume;
  vtkTimerLog* TimerLog = vtkTimerLog::New();
  double time = 0.0;
  double absolutetime = 0.0;
  int slice = this->Preview->GetZSlice();
  int e = 1;
  double timebetweenslices = 0.1;
  
  TimerLog->StartTimer();

  this->InteractiveStatus = STATUS_PLAY;  
  
  this->GetApplication()->ProcessPendingEvents();
  while( (absolutetime < 60)  &&
	 (this->InteractiveStatus == STATUS_PLAY) &&
	 (this->InteractiveVolume == currentvolumeid) &&
	 (this->IsMapped()) &&
	 (this->RenderWidget->IsMapped()))
  {

    this->GetApplication()->ProcessPendingEvents();
    
    TimerLog->StopTimer();
    double delay = TimerLog->GetElapsedTime();
    TimerLog->StartTimer();

    
    time         += delay;
    absolutetime += delay;
    if (time < timebetweenslices)
      continue;
    time = 0;

    this->Preview->SetZSlice(slice);
    this->Preview->Render();
    slice+=e;
    if (slice == this->Preview->GetWholeZMin())
      e = 1;
    if (slice == this->Preview->GetWholeZMax())
      e = -1;
    
  }
  TimerLog->StopTimer();
  TimerLog->Delete();
      
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::StopInteractive()
{
  if (this->InteractiveStatus ==  STATUS_PLAY)
    this->InteractiveStatus = STATUS_STOP;
}


void vtkKWDICOMImporter2::DoubleClickCallback()
{
  if ( this->InteractiveStatus == STATUS_PLAY)
    this->InteractiveStatus = STATUS_STOP;

  int row = this->MultiColumnList->GetIndexOfFirstSelectedRow();

  DICOMImageType::Pointer image =
    static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (row));

  vtkKWTopLevel* toplevel = vtkKWTopLevel::New();
  
  toplevel->SetMasterWindow(this);
  toplevel->SetApplication(this->GetApplication());
  toplevel->Create();
  toplevel->SetTitle(image->GetName());
  toplevel->SetDeleteWindowProtocolCommand(toplevel, "Withdraw");

  vtkKWDicomInfoWidget* widget = vtkKWDicomInfoWidget::New();
  widget->SetParent(toplevel);
  widget->Create();
  this->Script("pack %s -fill both -side top -expand t", 
	       widget->GetWidgetName());
  

  itk::DicomTagManager::Pointer tagmanager = itk::DicomTagManager::New();
  tagmanager->ImportTagList ((*image->GetFileList())[0]);
  widget->SetDicomTagList(tagmanager->GetTagList());
  
  // Get the position of the mouse, the size of the top level window.

  int px, py, tw, th, sw, sh;

  vtkKWTkUtilities::GetMousePointerCoordinates(this, &px, &py);
  vtkKWTkUtilities::GetWidgetRequestedSize(toplevel, &tw, &th);
  vtkKWTkUtilities::GetScreenSize(toplevel, &sw, &sh);

  px -= tw / 2;
  if (px + tw > sw)
  {
    px -= (px + tw - sw);
  }
  if (px < 0)
  {
    px = 0;
  }

  py -= th / 2;
  if (py + th > sh)
  {
    py -= (py + th - sh);
  }
  if (py < 0)
  {
    py = 0;
  }

  toplevel->SetPosition(px, py);
  toplevel->DeIconify();
  toplevel->Raise();

  widget->Delete();
}



void vtkKWDICOMImporter2::UpdatePreview (DICOMImageType::Pointer volume)
{

//   if ( !(volume.IsNull()) )
//   {
//     this->Preview->SetITKImage(static_cast<ImageType*>(volume));
//     this->Preview->Reset();
//     this->Preview->Show2DAxisOff();
//     this->Preview->Render();
//     return;
//   }

  DICOMImageType::DICOMFileList* filelist = volume->GetFileList();
  if (!filelist->size())
    return;
  
  unsigned int middleid = (unsigned int)( (double)(filelist->size()) / 2.0 );
  std::string filename = (*filelist)[middleid]->GetFileName();
  ImporterType::ReaderType::Pointer reader = ImporterType::ReaderType::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  
  this->Preview->Reset();
  this->Preview->Show2DAxisOff();
  this->Preview->Render();
}



  
void vtkKWDICOMImporter2::SequenceCheckButtonCallback (int val)
{
//   const unsigned int Number_Of_Selected = this->MultiColumnList->GetNumberOfSelectedRows();
//   unsigned int id = 0;
//   if (Number_Of_Selected)
//   {
//     int *indices = new int[Number_Of_Selected];    
//     id = indices[0];
//     delete [] indices;
//   }
  
  if (val)
  {
    this->SequenceDurationEntry->GetWidget()->SetStateToNormal();

    DICOMImageType::Pointer dcmvolume = static_cast<DICOMImageType*>(this->GDCMImporter->GetOutput (0));
    if (dcmvolume.IsNull())
      return;
    
    unsigned int tagid = 1000;

    itk::DicomTagManager::Pointer tagmanager = itk::DicomTagManager::New();
    tagmanager->ImportTagList ((*dcmvolume->GetFileList())[0]);
    
    if (tagmanager->HasTag(0x0018, 0x0072, tagid))
    {
      double T = atof (tagmanager->GetTagValue(tagid));
      std::cout<<"found the extended duration : "<<(double)(T)<<" sec"<<std::endl;
      this->SequenceDurationEntry->GetWidget()->SetValueAsDouble(T);
    }
    else if (tagmanager->HasTag(0x0018, 0x1088, tagid))
    {
      double T = 60.0/atof (tagmanager->GetTagValue(tagid));
      std::cout<<"found the heart rate : "<<(double)60/(double)(T)<<" pulse/min"<<std::endl;
      this->SequenceDurationEntry->GetWidget()->SetValueAsDouble(T);
    }
  }
  else
  {
    this->SequenceDurationEntry->GetWidget()->SetStateToDisabled();
    
  }
  
}


//----------------------------------------------------------------------------
int vtkKWDICOMImporter2::PreInvoke()
{
  int ret = this->Superclass::PreInvoke();
  if(!ret)
    return 0;

  this->SetResizable (1,1);
  
  return 1;
}


//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::SetOutputsAsVolumes(void)
{
  
  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (!win)
    return;
  

  for (unsigned int i=0; i<this->GDCMImporter->GetNumberOfOutputs(); i++)
  {
    win->GetProgressGauge()->SetValue((int)(100.0 * i / this->GDCMImporter->GetNumberOfOutputs()));

    DICOMImageType::Pointer dcmvolume = static_cast<DICOMImageType*>(this->GDCMImporter->GetOutput (i));
    if (dcmvolume.IsNull())
      continue;
    
    try
    {
      dcmvolume->Build();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      vtkWarningMacro(<<"error when building volume "<<dcmvolume->GetName()<<endl);
      continue;
    }

//     if (outputVolumes[i]->IsSequence())
//     {
//       // Instanciate the 2D+t -> 3D reshaper
	
//       itk::ImageSeriesReshapeFilter<itk::GDCMImporter::ImageType, itk::GDCMImporter::ImageType >::Pointer reshaper =
// 	itk::ImageSeriesReshapeFilter<itk::GDCMImporter::ImageType, itk::GDCMImporter::ImageType>::New();
      
//       std::vector<itk::GDCMImporter::ImageType::Pointer> ReshaperInputList;
//       std::vector<itk::GDCMImporter::ImageType::Pointer> ReshaperOutputList;
      
//       ReshaperInputList.push_back(outputVolumes[i]->GetImage());
//       reshaper->SetInputImageList(ReshaperInputList);
//       reshaper->SetInterlaceSpacing (1.0);
//       reshaper->SetInput(ReshaperInputList[0]);
//       reshaper->Update();
      
//       ReshaperOutputList = reshaper->GetOutputImageList();
      
//       vtkMetaDataSetSequence* seq = vtkMetaDataSetSequence::New();

//       double duration = 2;
	
//       unsigned int tagid = 1000;
//       if (dcmvolume->GetTagManager()->HasTag(0x0018, 0x0072, tagid))
//       {
// 	double T = atof (dcmvolume->GetTagManager()->GetTagValue(tagid));
// 	std::cout<<"found the extended duration : "<<(double)(T)<<" sec"<<std::endl;
// 	duration = T;
//       }
//       else if (dcmvolume->GetTagManager()->HasTag(0x0018, 0x1088, tagid))
//       {
// 	double T = 60.0/atof (dcmvolume->GetTagManager()->GetTagValue(tagid));
// 	std::cout<<"found the heart rate : "<<(double)60/(double)(T)<<" pulse/min"<<std::endl;
// 	duration = T;
//       }
	
//       seq->SetSequenceDuration (duration);

//       // 	std::cout<<"got "<<ReshaperOutputList.size()<<std::endl;
//       for (unsigned int j=0; j<ReshaperOutputList.size(); j++)
//       {
// 	vtkMetaImageData* metadataset = vtkMetaImageData::New();
	
// 	try
// 	{
// 	  //metadataset->SetDataSetAsItkImage(ReshaperOutputList[j]);
// 	  metadataset->SetItkImage<itk::GDCMImporter::ImageComponentType>(ReshaperOutputList[j]);
// 	}
// 	catch (vtkErrorCode::ErrorIds)
// 	{
// 	  metadataset->Delete();
// 	  continue;
// 	}
	
// 	double time = (double)j*(duration/(double)(ReshaperOutputList.size()));
// 	std::ostringstream name;
// 	name<<outputVolumes[i]->GetDescription().c_str()<<"("<<j<<")";
	
// 	metadataset->SetName(name.str().c_str());
// 	metadataset->SetTime(time);
// 	metadataset->SetDicomDictionary (outputVolumes[i]->GetImage()->GetMetaDataDictionary());
// 	try
// 	{
// 	  seq->AddMetaDataSet(metadataset);
// 	}
// 	catch (vtkErrorCode::ErrorIds)
// 	{
// 	  metadataset->Delete();
// 	  continue;
// 	}
// 	metadataset->Delete();
//       }
      
//       seq->SetName(outputVolumes[i]->GetDescription().c_str());
//       this->OutputList.push_back (seq);
//       // to do : better
//       // seq->Delete();
//     }
//     else
//     {
      
      vtkMetaImageData* metadataset = vtkMetaImageData::New();
      try
      {
	
	// 	  metadataset->SetDataSetAsItkImage(outputVolumes[i]->GetImage());
	metadataset->SetItkImage<ImageComponentType>(static_cast<ImageType*>(dcmvolume));

      }
      catch (vtkErrorCode::ErrorIds)
      {
	vtkWarningMacro(<<"error when parsing volume "<<dcmvolume->GetName()<<endl);
	metadataset->Delete();
	continue;
      }
      
      metadataset->SetDicomDictionary (dcmvolume->GetMetaDataDictionary());
      metadataset->SetName (dcmvolume->GetName());
      this->OutputList.push_back (metadataset);

      // to do : better
      // metadataset->Delete();
 //    }

    dcmvolume->Initialize();
      
  }
    
  vtksys_stl::string end_msg(win->GetStatusText());
  end_msg += " -- ";
  end_msg += ks_("Progress|Done");  
  win->SetStatusText(end_msg.c_str());
  win->GetProgressGauge()->SetValue(0);
  
}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::SetOutputsAs2DtSequence(void)
{
//   vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

//   if (!win)
//     return;
  
  
//   std::vector<itk::DICOMVolume::Pointer> outputVolumes = this->GetGDCMImporter()->GetDICOMVolumeList();
//   if (!outputVolumes.size())
//     return;


//   vtkMetaDataSetSequence* seq = vtkMetaDataSetSequence::New();
//   double duration = this->SequenceDurationEntry->GetWidget()->GetValueAsDouble();
//   if ((duration > 100) || (duration <=0))
//     duration = 2;
  
//   seq->SetSequenceDuration (duration);


//   unsigned int tagid = 1000;
//   double slicethickness = 1;
  
//   if (outputVolumes[0]->GetTagManager()->HasTag(0x0018, 0x0050, tagid))
//   {
//     slicethickness = atof (outputVolumes[0]->GetTagManager()->GetTagValue(tagid));
//   }

//   itk::MetaDataDictionary dictionary = outputVolumes[0]->GetImage()->GetMetaDataDictionary();
  
//   // Instanciate the 2D+t -> 3D reshaper
	
//   itk::ImageSeriesReshapeFilter<itk::GDCMImporter::ImageType, itk::GDCMImporter::ImageType >::Pointer reshaper =
//     itk::ImageSeriesReshapeFilter<itk::GDCMImporter::ImageType, itk::GDCMImporter::ImageType>::New();
  
//   std::vector<itk::GDCMImporter::ImageType::Pointer> ReshaperInputList;
//   std::vector<itk::GDCMImporter::ImageType::Pointer> ReshaperOutputList;
  

//   win->SetStatusText(ks_("Progress|Building DICOM volumes..."));
//   win->GetProgressGauge()->SetValue(0);

  
//   for (unsigned int i=0; i<outputVolumes.size(); i++)
//   {
//     win->GetProgressGauge()->SetValue((int)(100.0 * i / outputVolumes.size()));
  
//     try
//     {
//       outputVolumes[i]->Build();
//     }
//     catch (itk::ExceptionObject & e)
//     {
//       std::cerr << e;
//       vtkWarningMacro(<<"error when building volume "<<outputVolumes[i]->GetDescription().c_str()<<endl);
//       continue;
//     }

//     ReshaperInputList.push_back(outputVolumes[i]->GetImage());
//     outputVolumes[i]->ReleaseMemory();
//   }

//   try
//   {
//     reshaper->SetInputImageList(ReshaperInputList);
//     reshaper->SetInterlaceSpacing (slicethickness);
//     reshaper->SetInput(ReshaperInputList[0]);
//     reshaper->Update();
//   }
//   catch (itk::ExceptionObject & e)
//   {
//     std::cerr << e;
//     vtkWarningMacro(<<"error when reshaping sequence "<<endl);
//     return;
//   }

//   ReshaperOutputList = reshaper->GetOutputImageList();

//   for (unsigned int i=0; i<ReshaperOutputList.size(); i++)
//   {
//     vtkMetaImageData* metadataset = vtkMetaImageData::New();
    
//     try
//     {
//       metadataset->SetItkImage<itk::GDCMImporter::ImageComponentType>(ReshaperOutputList[i]);
//     }
//     catch (vtkErrorCode::ErrorIds)
//     {
//       metadataset->Delete();
//       continue;
//     }
    
//     double time = (double)i*(duration/(double)(ReshaperOutputList.size()));
//     std::ostringstream name;
//     name<<outputVolumes[0]->GetDescription().c_str()<<"("<<i<<")";
    
//     metadataset->SetName(name.str().c_str());
//     metadataset->SetTime(time);
//     metadataset->SetDicomDictionary (dictionary);
    
//     try
//     {
//       seq->AddMetaDataSet(metadataset);
//     }
//     catch (vtkErrorCode::ErrorIds)
//     {
//       metadataset->Delete();
//       continue;
//     }
    
//     metadataset->Delete();
//   }

//   seq->SetName(outputVolumes[0]->GetDescription().c_str());
//   this->OutputList.push_back (seq);

  
//   vtksys_stl::string end_msg(win->GetStatusText());
//   end_msg += " -- ";
//   end_msg += ks_("Progress|Done");  
//   win->SetStatusText(end_msg.c_str());
//   win->GetProgressGauge()->SetValue(0);
  
  


}

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::SetOutputsAs3DSequence(void)
{
  vtkKWWindowBase *win = this->GetApplication()->GetNthWindow(0);

  if (!win)
    return;
  
  win->SetStatusText(ks_("Progress|Building DICOM volumes..."));
  win->GetProgressGauge()->SetValue(0);

  vtkMetaDataSetSequence* seq = vtkMetaDataSetSequence::New();
  double duration = this->SequenceDurationEntry->GetWidget()->GetValueAsDouble();
  if ((duration > 100) || (duration <=0))
    duration = 2;
  
  seq->SetSequenceDuration (duration);

  DICOMImageType::Pointer image0 =
    static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (0));

  for (unsigned int i=0; i<this->GDCMImporter->GetNumberOfOutputs(); i++)
  {
    win->GetProgressGauge()->SetValue((int)(100.0 * i / this->GDCMImporter->GetNumberOfOutputs()));
    DICOMImageType::Pointer image =
      static_cast< DICOMImageType*>(this->GDCMImporter->GetOutput (i));

    if (image.IsNull())
      continue;

    try
    {
      image->Build();
    }
    catch (itk::ExceptionObject & e)
    {
      std::cerr << e;
      vtkWarningMacro(<<"error when building volume "<<image->GetName()<<endl);
      continue;
    }
    
    vtkMetaImageData* metadataset = vtkMetaImageData::New();
    
    try
    {
      metadataset->SetItkImage<ImageComponentType>(static_cast<ImageType*>(image));
    }
    catch (vtkErrorCode::ErrorIds)
    {
      metadataset->Delete();
      continue;
    }
    
    double time = (double)i*(duration/(double)(this->GDCMImporter->GetNumberOfOutputs()));
    metadataset->SetName(image->GetName());
    metadataset->SetTime(time);
    metadataset->SetDicomDictionary (image->GetMetaDataDictionary());    

    try
    {
      seq->AddMetaDataSet(metadataset);
    }
    catch (vtkErrorCode::ErrorIds)
    {
      metadataset->Delete();
      continue;
    }
    
    metadataset->Delete();
    
    image->Initialize();
    
  }  
  
  
  if (!image0.IsNull())
    seq->SetName(image0->GetName());
  this->OutputList.push_back (seq);
  
  vtksys_stl::string end_msg(win->GetStatusText());
  end_msg += " -- ";
  end_msg += ks_("Progress|Done");  
  win->SetStatusText(end_msg.c_str());
  win->GetProgressGauge()->SetValue(0);  

}

  

//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::PostInvoke()
{


  this->Superclass::PostInvoke();

  if (this->Done == vtkKWDialog::StatusCanceled)
    return;

  
  bool IsGlobalSequence = this->SequenceCheckButton->GetWidget()->GetSelectedState();
  bool IsGlobalSequence2D = false;

//   if (IsGlobalSequence)
//   {
//     IsGlobalSequence2D = true;
//     for (unsigned int i=0; i<outputVolumes.size(); i++)
//     {
//       if (!outputVolumes[i]->IsSequence())
//       {
// 	IsGlobalSequence2D = false;
// 	break;
//       }
//     }
//   }
  

  if (IsGlobalSequence2D)
    this->SetOutputsAs2DtSequence();
  else if (IsGlobalSequence)
    this->SetOutputsAs3DSequence();
  else
    this->SetOutputsAsVolumes();
  
  
  
  
}
 
 
 
//----------------------------------------------------------------------------
void vtkKWDICOMImporter2::CellUpdateCallback (int row, int col, int val)
{
  
  if (col != 2)
    return;

  if (!this->MultiColumnList->GetEnabled())
    return;
  
  
//   this->GDCMImporter->GetDICOMVolume (row)->SetSequenceFlag ((bool)(val));

//   if (this->SequenceCheckButton->GetWidget()->GetSelectedState())
//   {
//     std::vector<itk::DICOMVolume::Pointer> outputVolumes = this->GetGDCMImporter()->GetDICOMVolumeList();
//     if (!outputVolumes.size())
//       return;

//     for (unsigned int i=0; i<outputVolumes.size(); i++)
//     {
//       this->GDCMImporter->GetDICOMVolume (i)->SetSequenceFlag ((bool)(val));
//     }

//     this->Update();
  
//   }

  
    
}
 
