/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkDICOMExporter.h 1 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkDICOMExporter_h_
#define _wx_VtkDICOMExporter_h_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include <vtkINRIA3DConfigure.h>

#include <wx/checklst.h>
#include <string>
#include <itkGDCMExporter.h>


/**
   \class wxVtkDICOMExporter wxVtkDICOMExporter.h "wxVtkDICOMExporter.h"

   \author  Nicolas Toussaint,
   \date    Nov. 2007
*/


class vtkMetaImageData;
class vtkDataManager;


class WX_VTK_EXPORT wxVtkDICOMExporter: public wxDialog
{

 public:
  wxVtkDICOMExporter (wxWindow* parent, int id,
		      const wxString& title=wxT (""),
		      const wxPoint& pos=wxDefaultPosition,
		      const wxSize& size=wxDefaultSize,
		      long style=wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
  virtual ~wxVtkDICOMExporter();


  typedef itk::GDCMExporter::ImageComponentType ImageComponentType;
  typedef itk::GDCMExporter::ImageType ImageType;
  

  /** NOT USED YET */
  void OnEditDICOMInfo(wxCommandEvent &event){};
  /** NOT USED YET */
  void OnPropagateDICOMInfo(wxCommandEvent &event){};

  
  /** callback : internal use only */
  void OnCheckboxSelectAll(wxCommandEvent &event);
  void OnButtonCANCEL (wxCommandEvent &event);
  void OnButtonOK     (wxCommandEvent &event);

  /**
     Add an image to the exam
  */
  void AddVolume (const char* name, ImageType::Pointer image);

  wxString Title;
  wxSize   Size;
  wxPoint  Pos;
  long     Style;

  wxButton* ButtonEditDICOMInfo;
  wxButton* ButtonPropagateDICOMInfo;
  wxButton* ButtonCANCEL;
  wxButton* ButtonOK;

  wxCheckBox* CheckboxSelectAll;
  wxCheckListBox* CheckListBox;
  
  
 protected:
  
  DECLARE_EVENT_TABLE()
  void SetProperties (void);
  void DoLayout(void);

 private:
  
  itk::GDCMExporter::Pointer GDCMExporter;


  enum
  {
    CHECKBOX_SELECTALL,
    CHECKLISTBOX_VOLUMES,
    BUTTON_EDIT_DICOM,
    BUTTON_PROPAGATE_DICOM,
    BUTTON_CANCEL,
    BUTTON_OK
  };
  
  
};



#endif
