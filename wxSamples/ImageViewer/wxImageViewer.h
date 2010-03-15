/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageViewer.h 1197 2009-07-20 16:35:54Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-20 18:35:54 +0200 (Mon, 20 Jul 2009) $
Version:   $Revision: 1197 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _wxImageViewer_h_
#define _wxImageViewer_h_

#include "vtkINRIA3DConfigure.h"

// For compilers that supports precompilation , includes  wx/wx.h
#include  "wx/wxprec.h"

#ifndef WX_PRECOMP
#include  "wx/wx.h"
#endif

#include "wx/aui/aui.h"

class wxVtkPageView;

class IMAGEVIEWER_EXPORT wxImageViewer : public wxPanel
{

 public:

  
  wxImageViewer (wxWindow* parent, int id=wxID_ANY,
                 const wxPoint& pos=wxDefaultPosition,
                 const wxSize& size=wxDefaultSize,
                 long style=wxTAB_TRAVERSAL,
                 const wxString& title=wxT (""));
  virtual ~wxImageViewer()
  {
	  m_FrameManager.UnInit();
  };

  enum
  {
    TOOLBAR_OPEN,
    TOOLBAR_OPENDICOM,
    TOOLBAR_SAVE,
    TOOLBAR_SAVEALL,
    TOOLBAR_SAVEDICOM,
    TOOLBAR_SNAP,
    TOOLBAR_AX,
    TOOLBAR_COR,
    TOOLBAR_SAG,
    TOOLBAR_3D,
    TOOLBAR_3D_VR,
    TOOLBAR_FLIP,
    TOOLBAR_LINK,
    TOOLBAR_SELECTOR,
    TOOLBAR_GREYSCALE,
    TOOLBAR_ZOOM,
    TOOLBAR_CLUT,
    TOOLBAR_VRCHOICE,
    TOOLBAR_SHADECB,
    MENU_EDIT_DELETE_PAGE,
    TOOLBAR_PRESET
  };

  
  virtual void OnToolBarOpenVol              (wxCommandEvent &);
  virtual void OnToolBarOpenDICOM            (wxCommandEvent &);
  virtual void OnToolBarSave                 (wxCommandEvent &);
  virtual void OnToolBarSaveAll              (wxCommandEvent &);
  virtual void OnToolBarSaveDicom            (wxCommandEvent &);
  virtual void OnToolBarSnapShot             (wxCommandEvent &);

  virtual void OnFullScreenAxial             (wxCommandEvent &);
  virtual void OnFullScreenCoronal           (wxCommandEvent &);
  virtual void OnFullScreenSagittal          (wxCommandEvent &);
  virtual void OnFullScreen3D                (wxCommandEvent &);
  virtual void On3DVolumeRendering           (wxCommandEvent &);
  virtual void OnToolBarFlip                 (wxCommandEvent &);
  virtual void OnToolBarLink                 (wxCommandEvent &);
  
  virtual void OnSelectInteractionEvent      (wxCommandEvent &);
  virtual void OnWindowLevelInteractionEvent (wxCommandEvent &);
  virtual void OnZoomInteractionEvent        (wxCommandEvent &);
  virtual void OnToolBarCLUT                 (wxCommandEvent &);
  virtual void OnToolBarPreset               (wxCommandEvent &);
  virtual void OnToolBarVRMode               (wxCommandEvent &);
  virtual void OnToolBarShading              (wxCommandEvent &);
  
  virtual void OnDeletePage                  (wxAuiNotebookEvent&);
  virtual void OnPageChanged                 (wxAuiNotebookEvent&);
  virtual void OnFullPageInteractionEvent    (wxCommandEvent &);
  
  
  //wxToolBar*      m_ToolBarOpen;
  wxToolBar*      m_ToolBarOpenSave;
  wxToolBar*      m_ToolBarView;
  wxToolBar*      m_ToolBarInteraction;

  wxAuiNotebook* m_MainNotebook;
 
 protected:	 

  DECLARE_EVENT_TABLE()

 private:

  void DoLayout (void);
  void SetProperties (void);

  wxAuiManager    m_FrameManager;
  
};

#endif
