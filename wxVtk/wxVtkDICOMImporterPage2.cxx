/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMImporterPage2.cxx 1282 2009-10-08 17:00:40Z acanale $
Language:  C++
Author:    $Author: acanale $
Date:      $Date: 2009-10-08 19:00:40 +0200 (Thu, 08 Oct 2009) $
Version:   $Revision: 1282 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxVtkDICOMImporterPage2.h"

#include <wx/bmpbuttn.h>
#include <wx/filename.h>
#include <wx/colour.h>
#include <pixmaps/fileopen.xpm>
#include <wxVTKRenderWindowInteractor.h>
#include <wxItkProgressDialog.h>
#include "wxDialogVtkViewImage2D.h"

#include <stdio.h>

#include "vtkViewImage2D.h"
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMedicalImageReader2.h>
#include <vtkDICOMImageReader.h>
#include <vtkMedicalImageProperties.h>

#include <itkGDCMImageIO.h>
#include <itkGDCMImporter2.h>
#include <itkImageToVTKImageFilter.h>

#include <wxDicomInfoDialog.h>

#include <vtkTimerLog.h>
#include <wx/utils.h>
#include <sstream>


wxVtkDICOMImporterPage2::wxVtkDICOMImporterPage2 (wxWizard* parent)
  : wxWizardPageSimple (parent)
{


  this->Importer = GDCMImporterType::New();
  this->Importer->RecursiveScanOn();
  
  
  
  this->Text = new wxStaticText(this, wxID_ANY,
                                wxT ("Click on the 'Open directory' button and choose the DICOM root\n")
                                wxT ("folder to be scanned. The sorting modes will help you creating\n")
                                wxT ("the volumes for specific acquisitions (DTI, etc.)."));
  
  this->OpenDirButton = new wxBitmapButton (this, BUTTON_OPEN_DIR, wxBitmap (fileopen));
  this->OpenDirButton->SetToolTip (wxT ("Open the DICOM root directory here"));

  this->DeleteButton = new wxButton (this, BUTTON_DELETE, wxT("Remove"));
  this->DeleteButton->SetToolTip (wxT ("Delete the selected volumes"));
  this->SelectButton = new wxButton (this, BUTTON_SELECT, wxT("Select"));
  this->SelectButton->SetToolTip (wxT ("Select the selected volumes"));

  this->ReorganizeButton = new wxButton (this, BUTTON_REORGANIZE, wxT("Auto Reorganization"));
  this->ReorganizeButton->SetToolTip (wxT ("Attempt an automatic re-organization of the volumes, considering several consistency rules"));

  this->ResetButton = new wxButton (this, BUTTON_RESET, wxT("Reset"));
  this->ResetButton->SetToolTip (wxT ("Reset All"));
  
  this->SaveAllButton = new wxButton (this, BUTTON_SAVEALL, wxT("Save All"));
  this->SaveAllButton->SetToolTip( wxT("Save all DICOM volumes"));

//   this->ComboSplit = new wxChoice(this, COMBOSPLIT);
//   this->ComboSplit->Append (wxT ("Split volume by..."));
//   this->ComboSplit->SetSelection(0);
//   this->ComboSplit->SetToolTip (wxT ("Choose the DICOM tag to split this volume by"));

  this->TreeCtrl = new wxTreeCtrl (this, TREECTRL, wxDefaultPosition, wxDefaultSize,
                               wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_NO_LINES|wxTR_FULL_ROW_HIGHLIGHT|wxTR_MULTIPLE|wxTR_EXTENDED|wxTR_DEFAULT_STYLE);
  this->TreeCtrl->AddRoot (wxT ("DICOM"));

  this->ViewRWin = new wxVTKRenderWindowInteractor(this , wxID_ANY,
						   wxDefaultPosition,
						   wxSize (240,240),
						   wxWANTS_CHARS | wxNO_FULL_REPAINT_ON_RESIZE,
						   wxT (""));
  this->View = vtkViewImage2D::New();
  this->View->SetShowAnnotations(0);
  this->View->SetShowDirections(0);
  this->View->SetShow2DAxis(0);
  this->View->SetLeftButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  this->View->SetRightButtonInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);
  this->View->SetMiddleButtonInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);
  

  vtkRenderer* rend = vtkRenderer::New();
  this->ViewRWin->GetRenderWindow()->AddRenderer (rend);

  this->View->SetRenderWindow (this->ViewRWin->GetRenderWindow());
  this->View->SetRenderer (rend);
  rend->Delete();
  
  //this->CurrentNavigationThread = 0;
  this->View->SetBackgroundColor(0,0,0);
  this->View->SetRulerWidgetVisibility(0);
  this->SetProperties();
  this->DoLayout();

}



wxVtkDICOMImporterPage2::~wxVtkDICOMImporterPage2()
{
  //this->DeleteCurrentNavigationThread();
  this->ViewRWin->Delete();
  this->View->Delete();
}



BEGIN_EVENT_TABLE (wxVtkDICOMImporterPage2, wxWizardPageSimple)
  EVT_BUTTON(BUTTON_OPEN_DIR,         wxVtkDICOMImporterPage2::OnButtonOpenDir)
  EVT_BUTTON(BUTTON_DELETE,           wxVtkDICOMImporterPage2::OnButtonDelete)
  EVT_BUTTON(BUTTON_REORGANIZE,            wxVtkDICOMImporterPage2::OnAutoOrganizeVolumes)
  EVT_BUTTON(BUTTON_RESET,            wxVtkDICOMImporterPage2::OnButtonReset)
  EVT_BUTTON(BUTTON_SAVEALL,          wxVtkDICOMImporterPage2::OnButtonSaveAll)
  EVT_BUTTON(BUTTON_SELECT,           wxVtkDICOMImporterPage2::OnButtonSelect)
//   EVT_CHOICE(COMBOSPLIT,              wxVtkDICOMImporterPage2::OnComboSplit )
  EVT_WIZARD_PAGE_CHANGING (wxID_ANY, wxVtkDICOMImporterPage2::OnPageChanging)
  EVT_TREE_KEY_DOWN(TREECTRL,         wxVtkDICOMImporterPage2::OnKeyDown)
  EVT_TREE_SEL_CHANGED(TREECTRL,      wxVtkDICOMImporterPage2::OnSelectionChanged)
  EVT_TREE_ITEM_MENU(TREECTRL,        wxVtkDICOMImporterPage2::OnItemMenu)
  EVT_MENU(TREECTRL_MENU_SAVEAS,      wxVtkDICOMImporterPage2::OnTreeCtrlMenuSaveAs)
  EVT_MENU(TREECTRL_MENU_PREVIEW,     wxVtkDICOMImporterPage2::OnTreeCtrlMenuPreview)
  EVT_MENU(TREECTRL_MENU_SHOWFLAGS,   wxVtkDICOMImporterPage2::OnTreeCtrlMenuShowFlags)
  EVT_MENU(AUTO_ORGANIZATION_MENU,    wxVtkDICOMImporterPage2::OnAutoOrganizeVolumes)
  EVT_MENU(AUTO_ORGANIZATION_CLASSIC, wxVtkDICOMImporterPage2::OnAutoOrganizeClassic)
  EVT_MENU(AUTO_ORGANIZATION_POSITION_BASED,     wxVtkDICOMImporterPage2::OnAutoOrganizePositionBased)
END_EVENT_TABLE()




void wxVtkDICOMImporterPage2::SetProperties()
{
//   this->View->SetBackgroundColor (0.0, 0.0, 0.0);
//   this->View->SetInteractionStyle (vtkViewImage2D::NO_INTERACTION);

//   this->View->SetShowAnnotations(false);
//   this->View->SetShowDirections(false);
//   this->View->SetShowSliceNumber(false);
//   this->View->Show2DAxis (false);

//   this->View->SetLeftButtonInteractionStyle (vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
//   this->View->SetRightButtonInteractionStyle (vtkViewImage2D::ZOOM_INTERACTION);
//   this->View->SetWheelInteractionStyle (vtkViewImage2D::SELECT_INTERACTION);
  
  this->DeleteButton->Enable (false);
  this->SelectButton->Enable (false);
  this->ReorganizeButton->Enable (false);
  this->SaveAllButton->Enable (false);

//   //  D_SERIESUID,
//   this->ComboSplit->Append (wxT ("Series UID"));
//   //  D_STUDYDESCRIPTION,
//   this->ComboSplit->Append (wxT ("Study Description"));
//   //  D_SERIESDESCRIPTION,
//   this->ComboSplit->Append (wxT ("Series Description"));
//   //  D_ACQUISITIONTIME
//   this->ComboSplit->Append (wxT ("Acquisition Time"));
//   //  D_MODALITY
//   this->ComboSplit->Append (wxT ("Modality"));

}


void wxVtkDICOMImporterPage2::DeleteCurrentNavigationThread()
{
  if( this->CurrentNavigationThread )
  {
    this->CurrentNavigationThread->Delete();
    this->CurrentNavigationThread->Wait();
    delete this->CurrentNavigationThread;
    this->CurrentNavigationThread = 0;
  }

}


void wxVtkDICOMImporterPage2::DoLayout()
{
  
  wxBoxSizer* sizer1 = new wxBoxSizer (wxHORIZONTAL);
  sizer1->Add (this->OpenDirButton, 0, wxALL, 5);
  sizer1->Add (this->Text, 0, wxALL, 5);

  
  wxBoxSizer* sizermiddle = new wxBoxSizer (wxHORIZONTAL);
  sizermiddle->Add (this->ResetButton, 0, wxALL, 5);
  sizermiddle->Add (this->SelectButton, 0, wxALL, 5);
  sizermiddle->Add (this->DeleteButton, 0, wxALL, 5);
  sizermiddle->Add (this->ReorganizeButton, 0, wxALL, 5);
  sizermiddle->AddStretchSpacer();
  // sizermiddle->Add (this->ComboSplit, 0, wxALL, 5);
  sizermiddle->Add (this->SaveAllButton, 0, wxALL, 5);
  
  wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);
  sizer2->Add(sizer1, 1, wxALL|wxEXPAND, 0);
  sizer2->AddStretchSpacer();
  sizer2->Add(sizermiddle, 0, wxALL, 0);

  wxBoxSizer* sizertop = new wxBoxSizer (wxHORIZONTAL);
  sizertop->Add (sizer2,          0, wxALL|wxEXPAND, 0);
  sizertop->AddStretchSpacer();
  sizertop->Add (this->ViewRWin,  0, wxALL, 5);
  
  wxBoxSizer* main = new wxBoxSizer (wxVERTICAL);
  main->Add (sizertop,        0, wxALL|wxEXPAND, 0);
  //main->Add (sizermiddle,     0, wxALL|wxEXPAND, 0);
  main->Add (this->TreeCtrl,  1, wxALL|wxEXPAND, 5);
  
  this->SetSizer (main);
  this->Layout();

}




void wxVtkDICOMImporterPage2::OnButtonOpenDir(wxCommandEvent &event)
{
  
  wxDirDialog* myDirDialog = new wxDirDialog(this, wxT ("Choose the DICOM directory"),
                                             wxT(""),
                                             wxDD_DIR_MUST_EXIST|wxDD_DEFAULT_STYLE|wxDD_CHANGE_DIR, wxDefaultPosition);
  
  int OK = myDirDialog->ShowModal();
  if( OK!=wxID_OK )
  {
    myDirDialog->Destroy();
    return;
  }  
    
  wxString directory = myDirDialog->GetPath();
  this->Importer->SetInputDirectory( directory.char_str() );
  
  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Adding files..."), 100, this,wxPD_AUTO_HIDE);
  
  this->Importer->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->Disable();
    this->GetParent()->Disable();
    this->Importer->Scan();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
    
  }

  this->Importer->RemoveAllObservers();
  progressDialog->Destroy();
  myDirDialog->Destroy();
  
  this->UpdateTree();
  this->UpdateCombo();
  
  this->Enable();
  this->SaveAllButton->Enable();
  this->ReorganizeButton->Enable();
  this->GetParent()->Enable();
}


void wxVtkDICOMImporterPage2::UpdateTree (void)
{
  
  this->TreeCtrl->Hide();
  this->TreeCtrl->DeleteAllItems();
  this->TreeCtrl->AddRoot (wxT ("DICOM"));

  wxColour* colour1 = new wxColour ((unsigned char)(int)(255.0*0.8),
				    (unsigned char)(int)(255.0*0.8),
				    (unsigned char)(int)(255.0*0.95));
  wxColour* colour2 = new wxColour ((unsigned char)(255),
				    (unsigned char)(255),
				    (unsigned char)(255));
  
  
  for (unsigned int i=0; i<this->GetImporter()->GetNumberOfOutputs(); i++)
  {
    wxColour* c = NULL;
    if (i%2)
    {
      c = colour1;
    }
    else
    {  
      c = colour2;
    }
    

    DICOMImageType::Pointer dcmvolume = static_cast<DICOMImageType*>(this->GetImporter()->GetOutput (i));
    if (dcmvolume.IsNull())
      continue;

    DICOMImageItemData* DICOMImageData = new DICOMImageItemData();
    DICOMImageData->SetDICOMImage(dcmvolume);
    
    std::string volumetitle = dcmvolume->GetName();
    unsigned int nfiles = dcmvolume->GetFileList()->size();
    unsigned int dim[3] = {0,0,0};
    if (nfiles)
    {
      dim[0] = (*dcmvolume->GetFileList())[0]->GetXSize();
      dim[1] = (*dcmvolume->GetFileList())[0]->GetYSize();
      dim[2] = nfiles;
    }
    
    
    std::ostringstream os;
#ifdef WIN32
    os << volumetitle.c_str() << "      " << dim[2] << " slices.  (" <<dim[0]<<"x"<<dim[1]<<")";
#else
    os << volumetitle.c_str() << "\t\t" << dim[2] << " slices. \t (" <<dim[0]<<"x"<<dim[1]<<")";
#endif //WIN32
	wxString s_os (os.str().c_str(), wxConvUTF8);
    wxTreeItemId id = this->TreeCtrl->AppendItem ( this->TreeCtrl->GetRootItem(), s_os, -1, -1, DICOMImageData);
    this->TreeCtrl->SetItemBackgroundColour (id, *c);

    for( unsigned int n=0; n<nfiles; n++)
    {
      DICOMFileItemData* DICOMFileData = new DICOMFileItemData();
      DICOMFileData->SetFilename((*dcmvolume->GetFileList())[n]->GetFileName());
      
      std::string name = itksys::SystemTools::GetFilenameName ((*dcmvolume->GetFileList())[n]->GetFileName());
      wxString s_name (name.c_str(), wxConvUTF8);
      wxTreeItemId id2 = this->TreeCtrl->AppendItem (id, s_name,-1, -1, DICOMFileData);
      this->TreeCtrl->SetItemBackgroundColour (id2, *c);
    }

    

  }
  
  this->TreeCtrl->Show();

}


void wxVtkDICOMImporterPage2::UpdateCombo (void)
{
}


void wxVtkDICOMImporterPage2::DeleteSelectedItems()
{

  wxTreeItemIdValue cookie;
  wxTreeItemId root = this->TreeCtrl->GetRootItem();
  
  wxTreeItemId child = this->TreeCtrl->GetFirstChild(root, cookie);
  
  std::vector<wxTreeItemId> IdsToRemove;
  
  while( child.IsOk() )
  {
    if( this->TreeCtrl->IsSelected(child) )
    {
      IdsToRemove.push_back( child );
    }
    child = this->TreeCtrl->GetNextChild(root, cookie);
  }

  /*
  if( IdsToRemove.size()>0 )
  {
    this->DeleteCurrentNavigationThread();
  }
  */
  
  for( unsigned int i=0; i<IdsToRemove.size(); i++)
  {
    DICOMImageItemData* volume = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData ( IdsToRemove[i]) );
    if( volume )
    {
      this->Importer->RemoveDICOMImage(volume->GetDICOMImage());
    }
    //this->TreeCtrl->Delete( IdsToRemove[i] );
  }

  this->UpdateTree();
  
  

}
  

void wxVtkDICOMImporterPage2::OnKeyDown (wxTreeEvent &event)
{
    
  int key = event.GetKeyCode();

#ifndef __WXMSW__
  wxTreeItemIdValue cookie;
#endif
  
  wxArrayTreeItemIds arrayIds;
  unsigned int n = this->TreeCtrl->GetSelections(arrayIds);
  if (n==0)
  {
    return;
  }
  
  wxTreeItemId child = arrayIds[n-1];
  if( !child.IsOk() )
  {
    return;
  }
  
  
  switch(key)
  {
      case WXK_DELETE :
        this->DeleteSelectedItems();
        break;
        
#ifndef __WXMSW__ // Up and Down keys are already working on MSW
      case WXK_UP :
	if( this->TreeCtrl->GetPrevSibling(child).IsOk() )
        {
          this->TreeCtrl->UnselectAll();
          this->TreeCtrl->SelectItem (this->TreeCtrl->GetPrevSibling(child));
        }
	break;
      case WXK_DOWN :
	if( this->TreeCtrl->GetNextSibling(child).IsOk() )
        {
          this->TreeCtrl->UnselectAll();
          this->TreeCtrl->SelectItem (this->TreeCtrl->GetNextSibling(child));
        }
	break;
      case WXK_LEFT :
	if( this->TreeCtrl->GetItemParent(child).IsOk())
        {
	  if (this->TreeCtrl->GetItemParent(child) != this->TreeCtrl->GetRootItem())
	  {
	    this->TreeCtrl->UnselectAll();
	    this->TreeCtrl->Collapse(this->TreeCtrl->GetItemParent(child));
	    this->TreeCtrl->SelectItem (this->TreeCtrl->GetItemParent(child));
	  }
	}
	break;
      case WXK_RIGHT :
	if( this->TreeCtrl->GetFirstChild(child, cookie).IsOk() )
        {
          this->TreeCtrl->UnselectAll();
          this->TreeCtrl->SelectItem (this->TreeCtrl->GetFirstChild(child, cookie));
        }
	break;
#endif
      default:
        break;
  }
  
}



void wxVtkDICOMImporterPage2::OnSelectionChanged (wxTreeEvent &event)
{
  //this->DeleteCurrentNavigationThread();

  wxArrayTreeItemIds ids;
  unsigned int n = this->TreeCtrl->GetSelections(ids);
  if( n==0 )
  {
    return;
  }
  if( !ids[n-1].IsOk() )
  {
    return;
  }
  
  wxTreeItemId id = ids[n-1]; // only process the very last selection

  DICOMImageItemData* volume = NULL;
  volume = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData (id) );
  DICOMFileItemData* file = NULL;
  file = dynamic_cast<DICOMFileItemData*>( this->TreeCtrl->GetItemData (id) );

  if (!volume && !file)
  {
    return;
  }

  if (volume)
  {
    this->DeleteButton->Enable (true);
    this->SelectButton->Enable (true);

    DICOMImageType::Pointer dcmvolume = volume->GetDICOMImage();
    if (dcmvolume)
    {
      
      //wxNavigationThread* thread = new wxNavigationThread (dcmvolume, this);
      //this->CurrentNavigationThread = thread;
      
      /*
        if ( thread->Create() != wxTHREAD_NO_ERROR )
        {
        std::cerr << "Can't create thread!" << std::endl;
        }
        if ( thread->Run() != wxTHREAD_NO_ERROR )
        {
        std::cerr << "Can't start thread!" << std::endl;
        }   
        return;
      */
    }
    
    return;
  }

  this->DeleteButton->Enable (false);
  this->SelectButton->Enable (false);
//   this->ComboSplit->Enable (false);
  
  vtkDICOMImageReader* reader = vtkDICOMImageReader::New();
  std::string filename = file->GetFilename();
    
  if (!reader->CanReadFile (filename.c_str()))
  {
    std::cerr << "Cannot preview file : " << filename.c_str() << std::endl;
    reader->Delete();
    return;
  }
    
  reader->SetFileName (filename.c_str());
  reader->Update();
  
  this->View->SetImage (reader->GetOutput());
//   this->View->Reset();
//   this->View->ResetCurrentPoint();  
//   this->View->ResetWindowLevel();
  this->View->Render();
  
  reader->Delete();
  
}


void wxVtkDICOMImporterPage2::OnPageChanging(wxWizardEvent &event)
{
}


void wxVtkDICOMImporterPage2::OnButtonDelete (wxCommandEvent &event)
{
  this->DeleteSelectedItems();
  this->TreeCtrl->SelectItem (this->TreeCtrl->GetFirstVisibleItem());
}

void wxVtkDICOMImporterPage2::OnButtonSelect (wxCommandEvent &event)
{

  wxTreeItemIdValue cookie;
  wxTreeItemId root = this->TreeCtrl->GetRootItem();
  
  wxTreeItemId child = this->TreeCtrl->GetFirstChild(root, cookie);
  
  std::vector<wxTreeItemId> IdsToRemove;
  
  while( child.IsOk() )
  {
    if( !this->TreeCtrl->IsSelected(child) )
    {
      IdsToRemove.push_back( child );
    }
    child = this->TreeCtrl->GetNextChild(root, cookie);
  }

  /*
  if( IdsToRemove.size()>0 )
  {
    this->DeleteCurrentNavigationThread();
  }
  */
  
  for( unsigned int i=0; i<IdsToRemove.size(); i++)
  {
    DICOMImageItemData* volume = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData ( IdsToRemove[i]) );
    if( volume )
    {
      this->Importer->RemoveDICOMImage(volume->GetDICOMImage());
    }
    //this->TreeCtrl->Delete( IdsToRemove[i] );
  }

  this->UpdateTree();
  

  this->TreeCtrl->SelectItem (this->TreeCtrl->GetFirstVisibleItem());
}


void wxVtkDICOMImporterPage2::OnButtonReset (wxCommandEvent &event)
{
  
  //this->DeleteCurrentNavigationThread();
  
  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Restoring files..."),100, this,wxPD_AUTO_HIDE);
  
  this->Importer->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->Importer->InitializeOutputs();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }

  this->Importer->RemoveAllObservers();
  progressDialog->Destroy();
  
  this->UpdateTree();
  this->UpdateCombo();
}



void wxVtkDICOMImporterPage2::OnComboSplit (wxCommandEvent &event)
{
//   unsigned int tag = this->ComboSplit->GetSelection();

//   wxTreeItemId id = this->TreeCtrl->GetSelection();
//   if( !id.IsOk() )
//     return;

//   DICOMImageItemData* data = static_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData (id) );
//   if (!data)
//     return;

//   DICOMImageType::Pointer dcmvolume = data->GetDICOMImage();
//   if (!dcmvolume)
//     return;

//   this->Importer->SplitVolumeByPositionConsistency (dcmvolume);

//   this->UpdateTree();

  

}


void wxVtkDICOMImporterPage2::OnButtonSaveAll (wxCommandEvent &event)
{
  
  wxDirDialog* myDirDialog = new wxDirDialog(this, wxT ("Choose the output directory"),
                                             wxT(""),
                                             wxDD_DIR_MUST_EXIST|wxDD_DEFAULT_STYLE|wxDD_CHANGE_DIR, wxDefaultPosition);
  
  int OK = myDirDialog->ShowModal();
  if( OK!=wxID_OK )
  {
    myDirDialog->Destroy();
    return;
  }  
    
  wxString directory = myDirDialog->GetPath();
  myDirDialog->Destroy();

  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Saving files..."),100, this, wxPD_AUTO_HIDE);
  
  this->Importer->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->Importer->SaveOutputsInDirectory( directory.char_str() );
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }

  this->Importer->RemoveAllObservers();
  progressDialog->Destroy();
}



void wxVtkDICOMImporterPage2::OnItemMenu(wxTreeEvent& event)
{
  
  wxTreeItemId itemId = event.GetItem();
  // DICOMImageItemData *item = itemId.IsOk() ? dynamic_cast<DICOMImageItemData *>(this->TreeCtrl->GetItemData(itemId)):NULL;

  wxPoint clientpt = event.GetPoint();
  wxPoint screenpt = this->ClientToScreen(clientpt);
  
  this->ShowMenu(itemId, screenpt);
  event.Skip();
}


void wxVtkDICOMImporterPage2::OnAutoOrganizeVolumes(wxCommandEvent& event)
{
  this->ShowAutoOrganizeMenu();
  event.Skip();
}


void wxVtkDICOMImporterPage2::ShowMenu(wxTreeItemId id, const wxPoint& pt)
{
  wxString title;
  if ( id.IsOk() )
  {
    title << wxT("Menu for ") << this->TreeCtrl->GetItemText(id);
  }
  else
  {
    title = wxT("Menu for no particular item");
  }

#if wxUSE_MENUS
  DICOMImageItemData* volume = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData (id) );
  if( volume )
  {
    wxMenu menu(_T (""));
    menu.Append(TREECTRL_MENU_PREVIEW, wxT("Preview volume"));
    menu.Append(TREECTRL_MENU_SAVEAS, wxT("Save selected volumes as..."));
    menu.Append(TREECTRL_MENU_SHOWFLAGS, wxT("Show DICOM flags..."));    
    PopupMenu(&menu); //, pt);
  }
#endif // wxUSE_MENUS
}

void wxVtkDICOMImporterPage2::ShowAutoOrganizeMenu ()
{
  wxString title;
  title = wxT("Auto Organization Menu");
  
#if wxUSE_MENUS
  wxMenu menu(_T (""));
  menu.Append(AUTO_ORGANIZATION_CLASSIC, wxT("Default"), wxT ("Will attempt to merge significantly similar instances, \nKeeping coherence in size and orientation"));
  menu.Append(AUTO_ORGANIZATION_POSITION_BASED, wxT("Position Based"), wxT ("Will split instances containing several images \nat the same Patient Position, \nwhile keeping coherence in size and orientation"));
  PopupMenu(&menu); //, pt);
#endif // wxUSE_MENUS
}

void wxVtkDICOMImporterPage2::OnTreeCtrlMenuShowFlags (wxCommandEvent& event)
{
  wxArrayTreeItemIds ids;
  unsigned int n = this->TreeCtrl->GetSelections(ids);
  if( n!=1 )
    return;
  
  DICOMImageItemData* itemdata = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData (ids[0]) );
  if( !itemdata )
    return;
  
  DICOMImageType::Pointer volume = itemdata->GetDICOMImage();
  if (volume.IsNull() || !volume->GetFileList())
    return;
  

  wxString s_description( volume->GetName(), wxConvUTF8);
  wxDicomInfoDialog* dialog = new wxDicomInfoDialog (this, -1, s_description, wxDefaultPosition, wxSize(400,400));

  itk::DicomTagManager::Pointer tagmanager = itk::DicomTagManager::New();
  tagmanager->ImportTagList ((*volume->GetFileList())[0]);  
  dialog->SetDicomTagList(tagmanager->GetTagList());

  dialog->ShowModal();
  dialog->Destroy();
  
}


void wxVtkDICOMImporterPage2::OnTreeCtrlMenuSaveAs (wxCommandEvent& event)
{
  
  wxDirDialog* myDirDialog = new wxDirDialog(this, wxT ("Choose the output directory"),
                                             wxT(""),
                                             wxDD_DIR_MUST_EXIST|wxDD_DEFAULT_STYLE|wxDD_CHANGE_DIR, wxDefaultPosition);
  
  int OK = myDirDialog->ShowModal();
  if( OK!=wxID_OK )
  {
    myDirDialog->Destroy();
    return;
  }  
  
  wxString directory = myDirDialog->GetPath();
  myDirDialog->Destroy();
  
  
  wxArrayTreeItemIds ids;
  unsigned int n = this->TreeCtrl->GetSelections(ids);
  
  if( n==0 )
  {
    return;
  }

  for( unsigned int i=0; i<n; i++)
  {
    wxTreeItemId id = ids[i];
    DICOMImageItemData* volume = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData (id) );

    if( volume )
    {
      std::string filename (directory.char_str());
#ifdef __WXMSW__
      filename += "\\" + std::string (volume->GetDICOMImage()->GetName()) + ".mha";
#else
      filename += "/" + std::string (volume->GetDICOMImage()->GetName()) + ".mha";
#endif
      try
      {
        this->Importer->SaveOutputInFile ( i, filename.c_str() );
      }
      catch (itk::ExceptionObject &e)
      {
        std::cerr << e;
      }
      
    }
  }
  

}


void wxVtkDICOMImporterPage2::OnTreeCtrlMenuPreview (wxCommandEvent& event)
{
  
  wxArrayTreeItemIds ids;
  unsigned int n = this->TreeCtrl->GetSelections(ids);
  
  if( n==0 )
  {
    return;
  }

  //  for( unsigned int i=0; i<n; i++)
  //{
    wxTreeItemId id = ids[n-1];
    DICOMImageItemData* volume = dynamic_cast<DICOMImageItemData*>( this->TreeCtrl->GetItemData (id) );

    if( volume )
    {
	
      wxString s_description (volume->GetDICOMImage()->GetName(), wxConvUTF8);
      wxDialogVtkViewImage2D* dialog = new wxDialogVtkViewImage2D (this, wxID_ANY,
                                                                   s_description,
                                                                   wxDefaultPosition,
                                                                   wxSize (200,200));

      volume->GetDICOMImage()->Build();
      
//       ImageType::Pointer image = volume->GetDICOMImage()->TemporaryBuild();

      itk::ImageToVTKImageFilter<ImageType>::Pointer converter =
        itk::ImageToVTKImageFilter<ImageType>::New();
      converter->SetInput (volume->GetDICOMImage());
      converter->Update();

      vtkImageData* vtkImage = vtkImageData::New();
      vtkImage->DeepCopy ( converter->GetOutput() );
      
      dialog->GetVtkViewImage2D()->SetImage ( vtkImage );
      dialog->GetVtkViewImage2D()->Reset();
      vtkImage->Delete();
      dialog->ShowModal();

      dialog->Destroy();
      
    }
    //}
  

}


void wxVtkDICOMImporterPage2::NavigateInVolume(DICOMImageType::Pointer dcmvolume)
{

//   if (!dcmvolume)
//     return;
  
//   std::vector<std::string> filelist = dcmvolume->GetFileList();

  
//   double time = 0.0;
//   wxKeyEvent junkevent;
  
//   vtkTimerLog* TimerLog = vtkTimerLog::New();
  

//   TimerLog->StartTimer();

//   while (1)
//   {
//     for (unsigned int i = 0; i<filelist.size(); i++)
//     {           
      
//       std::string filename = filelist[i];  
//       vtkDICOMImageReader* reader = vtkDICOMImageReader::New();
      
//       if (!reader->CanReadFile (filename.c_str()))
//       {
// 	std::cerr << "Cannot preview file : " << filename.c_str() << std::endl;
// 	reader->Delete();
// 	continue;
//       }
      
//       reader->SetFileName (filename.c_str());
//       reader->Update();
      
//       this->View->SetImage (reader->GetOutput());
//       //this->View->ResetWindowLevel();
//       this->View->Render();
      

//       if (this->m_Status == wxVtkDICOMImporterPage2::STATUS_STOP)
//       {
//         break;
//       }
            
//       reader->Delete();
//       wxTheApp->Yield();

//       while(time < 0.1)
//       {
        
//         TimerLog->StopTimer();
        
//         double delay = TimerLog->GetElapsedTime();
//         time+=delay;
        
//         TimerLog->StartTimer();
        
//       }
//     }
//   }

  
  
//   TimerLog->Delete();
}




void *wxNavigationThread::Entry()
{
  if ( this->TestDestroy() )
  {
    return NULL;
  }


  this->Navigate();

  return NULL;
  
}



void wxNavigationThread::OnExit()
{
  //std::cout << "Exiting thread!" << std::endl;
}

void wxNavigationThread::Navigate()
{
//   if (m_DCMVolume.IsNull() || !m_Frame)
//   {
//     return;
//   }
  
//   std::vector<std::string> filelist = m_DCMVolume->GetFileList();

  
//   double time = 0.0;
//   wxKeyEvent junkevent;
  
//   vtkTimerLog* TimerLog = vtkTimerLog::New();
  

//   TimerLog->StartTimer();

//   bool KeepGoing = true;
  
//   while ( KeepGoing )
//   {
    
//     for (unsigned int i = 0; i<filelist.size(); i++)
//     {
//       if ( this->TestDestroy() )
//       {
//         KeepGoing = false;
//         break;
//       }
      
//       std::string filename = filelist[i];  
//       vtkDICOMImageReader* reader = vtkDICOMImageReader::New();

//       //std::cout << "Reading: " << filename << std::endl;
      
//       if (!reader->CanReadFile (filename.c_str()))
//       {
//         std::cerr << "Cannot preview file : " << filename.c_str() << std::endl;
//         reader->Delete();
//         continue;
//       }
      
//       reader->SetFileName (filename.c_str());
//       reader->Update();

//       wxMutexGuiEnter();
//       m_Frame->View->SetImage (reader->GetOutput());
//       m_Frame->View->ResetWindowLevel();      
//       //dynamic_cast<wxVTKRenderWindowInteractor*>(m_Frame->View->GetRenderWindowInteractor())->SetCurrent();
// 	  m_Frame->RefreshRect( m_Frame->ViewRWin->GetRect() );	  
// 	  //dynamic_cast<wxVTKRenderWindowInteractor*>(m_Frame->View->GetRenderWindowInteractor())->Refresh();
// 	  //dynamic_cast<wxVTKRenderWindowInteractor*>(m_Frame->View->GetRenderWindowInteractor())->Update();
// 	  //m_Frame->View->Render();
//       wxMutexGuiLeave();

      
//       reader->Delete();

     
//       while(time < 0.1)
//       {
        
//         TimerLog->StopTimer();
        
//         double delay = TimerLog->GetElapsedTime();
//         time+=delay;
        
//         TimerLog->StartTimer();
        
//       }
      
//       time = 0.0;
      
//     }
//   }
  
  
//   TimerLog->Delete();  
}





void wxVtkDICOMImporterPage2::OpenDirectory (const char* directory)
{

  this->Importer->SetInputDirectory(directory);
  
  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Adding files..."), 100, this,wxPD_AUTO_HIDE);
  
  this->Importer->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->Disable();
    this->GetParent()->Disable();
    this->Importer->Scan();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
    return;
    
    
  }

  this->Importer->RemoveAllObservers();
  progressDialog->Destroy();
  
  this->UpdateTree();
  this->UpdateCombo();
  
  this->Enable();
  this->SaveAllButton->Enable();
  this->ReorganizeButton->Enable();
  this->GetParent()->Enable();

}

  




void wxVtkDICOMImporterPage2::OnAutoOrganizeClassic(wxCommandEvent& event)
{
  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Restoring files..."),100, this,wxPD_AUTO_HIDE);
  
  this->Importer->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->Importer->AutoOrganization();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }

  this->Importer->RemoveAllObservers();
  progressDialog->Destroy();
  
  this->UpdateTree();
  
}
 
void wxVtkDICOMImporterPage2::OnAutoOrganizePositionBased(wxCommandEvent& event)
{
  wxItkProgressDialog* progressDialog = new wxItkProgressDialog(wxT ("Please wait"),
                                                                wxT("Restoring files..."),100, this,wxPD_AUTO_HIDE);
  
  this->Importer->AddObserver (itk::ProgressEvent(), progressDialog->GetCallback());  

  try
  {
    this->Importer->AutoOrganizationPositionBased();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e;
  }

  this->Importer->RemoveAllObservers();
  progressDialog->Destroy();
  
  this->UpdateTree();
}

