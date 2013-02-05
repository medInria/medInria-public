/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: CodeCopyright.txt 1182 2009-06-30 09:19:05Z acanale $
Language:  C++
Author:    $Author: $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _wx_VtkLandmarkWindow_h_
#define _wx_VtkLandmarkWindow_h_

#include <vtkINRIA3DConfigure.h>

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

//#include <wx/spinbutt.h>
#include <wx/spinctrl.h>


#include <vtkCommand.h>

class vtkLandmarkManager;
class wxVtkLandmarkWindow;
class vtkLandmark;


class WX_VTK_EXPORT wxVtkLandmarkWindowCallback : public vtkCommand
{
  
 public:
  static wxVtkLandmarkWindowCallback* New()
  { return new wxVtkLandmarkWindowCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long event, void* );  

  void SetLandmarkWindow (wxVtkLandmarkWindow* win)
  { this->LandmarkWindow = win; }
  
 protected:
  wxVtkLandmarkWindowCallback()
  {
    this->LandmarkWindow = 0;
  }
  ~wxVtkLandmarkWindowCallback(){}
  
  
 private:

  wxVtkLandmarkWindow* LandmarkWindow;
  
};



class WX_VTK_EXPORT wxVtkLandmarkWindow : public wxPanel
{

 public:
  
  wxVtkLandmarkWindow (wxWindow* parent, int id,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = 0,
                       const wxString& name = wxT (""));
  virtual ~wxVtkLandmarkWindow();


  vtkLandmarkManager* GetLandmarkManager (void) const
  { return this->LandmarkManager; }
  

  virtual void OnButtonOpenLandmarks     (wxCommandEvent &event);
  virtual void OnButtonSaveLandmarks     (wxCommandEvent &event);
  virtual void OnButtonAddLandmark     (wxCommandEvent &event);
  virtual void OnButtonRemoveLandmark  (wxCommandEvent &event);
  virtual void OnButtonRemoveAllLandmarks  (wxCommandEvent &event);
  virtual void OnButtonUpdate  (wxCommandEvent &event);

  virtual void ProcessLandmarks (void);


  virtual void OnLandmarkFlagText   (wxCommandEvent&);
  virtual void OnLandmarkFlagButton (wxSpinEvent&);

  virtual void OnLandmarkChoice (wxCommandEvent&);

  void AddLandmarkToList      (vtkLandmark*);
  void RemoveLandmarkFromList (vtkLandmark*);
  void HighlightLandmark      (vtkLandmark*);
  
 protected:

  DECLARE_EVENT_TABLE()

  void UpdateListBox (void);
  void RenderViews (void);
  
 private:
  
  void DoLayout (void);
  void SetProperties (void);
  
  enum
  {
    BUTTON_OPEN_LANDMARKS,
    BUTTON_SAVE_LANDMARKS,
    BUTTON_ADD_LANDMARK,
    BUTTON_REMOVE_LANDMARK,
	BUTTON_REMOVE_ALL_LANDMARKS,
    BUTTON_UPDATE,
    LB_LANDMARK,
    TE_LANDMARK,
    SB_LANDMARK
  };
 
  vtkLandmarkManager* LandmarkManager;

  wxVtkLandmarkWindowCallback* LandmarkWindowCallback;

  wxBitmapButton* ButtonOpenLandmarks;
  wxBitmapButton* ButtonSaveLandmarks;

  wxBitmapButton* ButtonAddLandmark;
  wxBitmapButton* ButtonRemoveLandmark;
  wxButton*       ButtonRemoveAllLandmarks;
  wxButton*       ButtonUpdate;

  wxChoice*      LandmarkList;

  wxTextCtrl*    LandmarkFlagText;
  wxSpinCtrl*    LandmarkFlagButton;
  
  
};



#endif
