/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxVtkImageFlip.h 1288 2009-10-14 09:50:01Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-10-14 11:50:01 +0200 (Wed, 14 Oct 2009) $
Version:   $Revision: 1288 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wx_VtkImageFlip_h_
#define _wx_VtkImageFlip_h_

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
#include <string>
#include <vtkINRIA3DConfigure.h>

/**
   \class wxVtkImageFlip wxVtkImageFlip.h "wxVtkImageFlip.h"
   \brief This class is a dialog window that helps flipping\n
   and rotating a 3D Image to correct geometry problems.\n
   It takes a VTK image as input and pops up 3 views of it.\n
   Flipping the image is possible in the 3 orientations.\n
   Rotations are possible by choosing an acquisition flag\n
   
   You can set the input \n
   SetImage() or SetInput() \n
   Get the flipped image !with GetOutput()\n
   Get the acquisiton flag with GetAcquisitionFlag()\n
   Get the axes flipped with GetAxesFlipped()\n

   You can also directly get the vtk reslicer \n
   in order to plug it (for instance before a vtkViewImage)\n
   with GetReslicer()\n
   
   \author  Nicolas Toussaint,
   \date    Sept. 2006
*/


#include "vtkViewImage2D.h"
#include <vtkImageReslice.h>


class wxVTKRenderWindowInteractor;
class vtkImageData;
class vtkImageFlip;
class vtkViewImageCollection;
class vtkMatrix4x4;


class WX_VTK_EXPORT wxVtkImageFlip: public wxDialog
{

 public:
  wxVtkImageFlip (wxWindow* parent, int id,
		  const wxString& title=wxT (""),
		  const wxPoint& pos=wxDefaultPosition,
		  const wxSize& size=wxDefaultSize,
		  long style=wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
  virtual ~wxVtkImageFlip();


  void OnComboAcquisition (wxCommandEvent &event);
  void OnButtonFlipRL (wxCommandEvent &event);
  void OnButtonFlipFH (wxCommandEvent &event);
  void OnButtonFlipAP (wxCommandEvent &event);
  void OnButtonCANCEL (wxCommandEvent &event);
  void OnButtonOK     (wxCommandEvent &event);
  void OnButtonSave   (wxCommandEvent &event);

  void UpdateReslicer(void);
  void Synchronize (void);
  

  /** Set the input image to flip with this method */
  void SetImage (vtkImageData* image, vtkMatrix4x4* mat=0);
  
  /** Set the input image to flip with this method */
  void SetInput (vtkImageData* input, vtkMatrix4x4* mat=0)
  { this->SetImage (input, mat); }

  vtkImageData* GetOutput (void) const
  { return this->View1->GetImage(); }

  template <class TImage>
    void SaveImage (vtkImageData*, const char*);
  
  /** Get the Output flipped image with this method */
  vtkImageData* GetImage (void) const
  { return this->Image; }

  vtkImageData* GetInput (void) const
  { return this->Image; }

  /** Get the axes that have been flipped */
  void SetAxesFlipped (bool axes[3])
  {
    this->AxesFlipped[0]=axes[0];
    this->AxesFlipped[1]=axes[1];
    this->AxesFlipped[2]=axes[2];
  }
    
  bool* GetAxesFlipped (void) 
  { return AxesFlipped; }
  bool GetAxisFlipped (int i) 
  { return AxesFlipped[i]; }

  enum AcquisitionOrientation
  {
    ACQ_AXIAL,
    ACQ_CORONAL,
    ACQ_SAGITTAL
  };
  /**
     Get/Set the acquisition orientation flag :
     0 : AXIAL
     1 : SAGITTAL
     2 : CORONAL
  */
  void SetAcquisitionFlag (unsigned int flag)
  { this->AcquisitionFlag = flag;
    this->ComboAcquisition ->SetSelection (flag); }

  unsigned int GetAcquisitionFlag (void) const
  { return this->AcquisitionFlag; }

  /** Get the output reslicer */
  vtkImageReslice* GetReslicer (void) const
  { return this->Reslicer;}
  
  double* GetReslicerDirectionCosines (void) const
  { return this->Reslicer->GetResliceAxesDirectionCosines(); }

  double* GetReslicerAxesOrigin (void) const
  { return this->Reslicer->GetResliceAxesOrigin(); }
  
  
  wxString Title;
  wxSize   Size;
  wxPoint  Pos;
  long     Style;

  wxStaticText* AcquisitionTxt;
  wxComboBox*   ComboAcquisition;

  wxButton* ButtonFlipRL;
  wxButton* ButtonFlipFH;
  wxButton* ButtonFlipAP;
  wxButton* ButtonCANCEL;
  wxButton* ButtonOK;
  wxButton* ButtonSave;
  
 protected:
  
  DECLARE_EVENT_TABLE()
  void SetProperties (void);
  void DoLayout(void);

 private:

  vtkImageData*     Image;
  vtkImageReslice*  Reslicer;
  
  wxVTKRenderWindowInteractor* VtkView1;
  wxVTKRenderWindowInteractor* VtkView2;
  wxVTKRenderWindowInteractor* VtkView3;
  
  
  vtkViewImage2D* View1;
  vtkViewImage2D* View2;
  vtkViewImage2D* View3;

  vtkViewImageCollection* m_Pool;
  
  int               ImageDim[3];
  double            ImageOrigin[3];
  double            ImageCenter[3];
  int               ImageExtent[6];
  double            ImageSpacing[3];
  
  

  bool              AxesFlipped[3];
  unsigned int      AcquisitionFlag;
  
  bool              FirstRender;
  
  enum
  {
    COMBOBOX_ACQ,
    BUTTON_FLIPRL,
    BUTTON_FLIPFH,
    BUTTON_FLIPAP,
    BUTTON_CANCEL,
    BUTTON_OK,
    BUTTON_SAVE
  };
  
  
};


#include "wxVtkImageFlip.txx"

#endif
