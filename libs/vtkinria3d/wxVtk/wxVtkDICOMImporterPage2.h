/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMImporterPage2.h 1193 2009-07-20 07:44:11Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-20 09:44:11 +0200 (Mon, 20 Jul 2009) $
Version:   $Revision: 1193 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkDICOMImporterPage2_h_
#define _wx_VtkDICOMImporterPage2_h_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include "wx/wizard.h"
#include "wx/treectrl.h"
#include <string>
#include <vector>

#include <itkGDCMImporter2.h>

#include <wxVtkDICOMImporter.h>

class wxVTKRenderWindowInteractor;
class vtkViewImage2D;
class wxVtkDICOMImporterPage2;

#include <vtkINRIA3DConfigure.h>


/**
   \class DICOMImageItemData wxVtkDICOMImporterPage2.h "wxVtkDICOMImporterPage2.h"
   \brief see wxVtkDICOMImporter

   
   \author Nicolas Toussaint
 */
class WX_VTK_EXPORT DICOMImageItemData: public wxTreeItemData
{
  
 public:
  typedef wxVtkDICOMImporter::DICOMImageType DICOMImageType;
  
  DICOMImageItemData()
  {
    this->DICOMImage = NULL;
  }


  void SetDICOMImage (DICOMImageType::Pointer gdcm_v)
  { this->DICOMImage = gdcm_v; }
  DICOMImageType::Pointer GetDICOMImage (void) const
  { return this->DICOMImage; }

 private:

  DICOMImageType::Pointer DICOMImage;  
};



class WX_VTK_EXPORT wxNavigationThread: public wxThread
{
  
 public:
  typedef wxVtkDICOMImporter::DICOMImageType DICOMImageType;
  
  wxNavigationThread(DICOMImageType::Pointer dcmvolume, wxVtkDICOMImporterPage2* frame)
    //: wxThread(wxTHREAD_JOINABLE)
	: wxThread()
  {
    m_DCMVolume = dcmvolume;
    m_Frame = frame;
  }
  virtual ~wxNavigationThread(){};

  virtual void *Entry();
  virtual void OnExit();

  void Navigate();
  
 protected:

 private:

  DICOMImageType::Pointer m_DCMVolume;
  wxVtkDICOMImporterPage2*  m_Frame;
  
};




/**
   \class DICOMFileItemData wxVtkDICOMImporterPage2.h "wxVtkDICOMImporterPage2.h"
   \brief see wxVtkDICOMImporter

   
   \author Nicolas Toussaint
 */
class WX_VTK_EXPORT DICOMFileItemData: public wxTreeItemData
{
  
 public:

  DICOMFileItemData()
  {
    this->Filename = "";
  }


  void SetFilename (std::string filename)
  { this->Filename = filename; }
  std::string GetFilename (void) const
  { return this->Filename; }

 private:

  std::string Filename;
  
  
};


/**
   \class wxVtkDICOMImporterPage2 wxVtkDICOMImporterPage2.h "wxVtkDICOMImporterPage2.h"
   \brief see wxVtkDICOMImporter

   
   \author Nicolas Toussaint
 */
class WX_VTK_EXPORT wxVtkDICOMImporterPage2: public wxWizardPageSimple
{

 public:
  wxVtkDICOMImporterPage2 (wxWizard* parent);
  virtual ~wxVtkDICOMImporterPage2();

  typedef wxVtkDICOMImporter::ImageType ImageType;
  typedef wxVtkDICOMImporter::GDCMImporterType GDCMImporterType;
  typedef GDCMImporterType::DICOMImageType DICOMImageType;
  
  GDCMImporterType::Pointer GetImporter (void)
  { return this->Importer; }

  
  enum
  {
    BUTTON_OPEN_DIR,
    BUTTON_DELETE,
    BUTTON_REORGANIZE,
    BUTTON_RESET,
    BUTTON_SAVEALL,
    BUTTON_SELECT,
    TREECTRL,
    TREECTRL_MENU_SAVEAS,
    TREECTRL_MENU_PREVIEW,
    TREECTRL_MENU_SHOWFLAGS,
    AUTO_ORGANIZATION_MENU,
    AUTO_ORGANIZATION_CLASSIC,
    AUTO_ORGANIZATION_POSITION_BASED,
    COMBOSPLIT
  };

  enum StatusIds
  {
    STATUS_PLAY,
    STATUS_STOP
  };
  

  void OnButtonOpenDir        (wxCommandEvent &event);
  void OnButtonDelete         (wxCommandEvent &event);
  void OnButtonReset          (wxCommandEvent &event);
  void OnButtonSaveAll        (wxCommandEvent &event);
  void OnComboSplit           (wxCommandEvent &event);
  void OnButtonClearAll       (wxCommandEvent &event);
  void OnButtonSelect         (wxCommandEvent &event);
  void OnKeyDown              (wxTreeEvent &event);
  void OnSelectionChanged     (wxTreeEvent &event);
  void OnItemMenu             (wxTreeEvent &event);
  void OnPageChanging         (wxWizardEvent &event);
  void OnTreeCtrlMenuSaveAs   (wxCommandEvent& event);
  void OnTreeCtrlMenuPreview  (wxCommandEvent& event);
  void OnTreeCtrlMenuShowFlags(wxCommandEvent& event);
  void OnAutoOrganizeVolumes  (wxCommandEvent& event);
  void OnAutoOrganizeClassic  (wxCommandEvent& event);
  void OnAutoOrganizePositionBased  (wxCommandEvent& event);

  void UpdateTree (void);
  void UpdateCombo (void);
  
  void DeleteSelectedItems(void);

  void ShowMenu (wxTreeItemId, const wxPoint&);
  void ShowAutoOrganizeMenu (void);

  void DeleteCurrentNavigationThread(void);

  void OpenDirectory (const char* directory);
  

  wxStaticText* Text;
  wxButton*     OpenDirButton;
  wxButton*     DeleteButton;
  wxButton*     ReorganizeButton;
  wxButton*     ResetButton;
  wxButton*     SaveAllButton;
  wxButton*     SelectButton;

  wxChoice*     ComboSplit;
  wxTreeCtrl*   TreeCtrl;
  
  wxThread*     CurrentNavigationThread;
  
  wxVTKRenderWindowInteractor* ViewRWin;
  vtkViewImage2D*              View;

 
  
 protected:
  
  DECLARE_EVENT_TABLE()
  void SetProperties (void);
  void DoLayout(void);

  void NavigateInVolume(GDCMImporterType::DICOMImageType::Pointer dcmvolume);  

 private:
  
  GDCMImporterType::Pointer Importer;
  
  StatusIds m_Status; 
  
};



#endif
