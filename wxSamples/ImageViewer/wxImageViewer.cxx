/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: wxImageViewer.cxx 1217 2009-08-03 18:48:06Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-08-03 20:48:06 +0200 (Mon, 03 Aug 2009) $
Version:   $Revision: 1217 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "wxImageViewer.h"

#include "pixmaps/importdicom.xpm"
#include "pixmaps/fileopen.xpm"
#include "pixmaps/filesave.xpm"
#include "pixmaps/save_all.xpm"
#include "pixmaps/snapshot.xpm"
#include "pixmaps/axial.xpm"
#include "pixmaps/coronal.xpm"
#include "pixmaps/sagittal.xpm"
#include <pixmaps/fullscreen.xpm>
#include <pixmaps/volumerendering.xpm>
#include <pixmaps/rotate.xpm>
#include <pixmaps/togglelink.xpm>
#include <pixmaps/cdsave.xpm>
#ifdef __WXMAC__
#include "pixmaps/selector2.xpm"
#else
#include "pixmaps/selector.xpm"
#endif
#include "pixmaps/greyscale.xpm"
#include "pixmaps/zoom.xpm"

#include "vtkLookupTableManager.h"
#include "vtkViewImage2D.h"

wxImageViewer::wxImageViewer (wxWindow* parent, int id,
                              const wxPoint& pos, const wxSize& size,
                              long styl, const wxString& title)
  : wxPanel (parent, id, pos, size, styl, title)
{

  // 2D Viewer Tool Bar
  m_ToolBarOpenSave = new wxToolBar (this, wxID_ANY,
                                   wxDefaultPosition,								   
                                   wxDefaultSize,
                                   wxTB_HORIZONTAL|wxTB_FLAT|wxTB_NODIVIDER);
  m_ToolBarOpenSave->SetToolBitmapSize(wxSize(32,32));
  m_ToolBarOpenSave->AddTool (TOOLBAR_OPENDICOM, wxT("Open DICOM"),
                              wxBitmap (importdicom),
                              wxNullBitmap, wxITEM_NORMAL, wxT ("Open a DICOM folder"),
                              wxT ("Open a DICOM folder."));
  m_ToolBarOpenSave->AddTool (TOOLBAR_OPEN, wxT("Open"),
                              wxBitmap (fileopen),
                              wxNullBitmap, wxITEM_NORMAL, wxT ("Open an image(s)"),
                              wxT ("Open an image(s)."));
  m_ToolBarOpenSave->AddTool (TOOLBAR_SAVE, wxT("Save"),
                              wxBitmap (filesave),
                              wxNullBitmap, wxITEM_NORMAL, wxT ("save current image"),
                              wxT ("Save current image"));
  m_ToolBarOpenSave->AddTool (TOOLBAR_SAVEALL, wxT("Save all"),
                              wxBitmap (save_all),
                              wxNullBitmap, wxITEM_NORMAL, wxT ("Save all images into a directory"),
                              wxT ("Save all images into a directory"));
  m_ToolBarOpenSave->AddTool (TOOLBAR_SAVEDICOM, wxT("Save DICOM"),
                              wxBitmap (cdsave),
                              wxNullBitmap, wxITEM_NORMAL, wxT ("Save images as DICOM exam"),
                              wxT ("Save all images into a directory"));
  m_ToolBarOpenSave->AddTool (TOOLBAR_SNAP, wxT("Snapshot"),
                              wxBitmap (snapshot),
                              wxNullBitmap, wxITEM_NORMAL, wxT ("Snapshot"),
                              wxT ("Take a snap shot of the full screen image"));
  m_ToolBarOpenSave->Realize();


  m_ToolBarView = new wxToolBar (this, wxID_ANY,
				 wxDefaultPosition,								   
				 wxDefaultSize,
				 wxTB_HORIZONTAL|wxTB_FLAT|wxTB_NODIVIDER);
  m_ToolBarView->SetToolBitmapSize(wxSize(32,32));
  m_ToolBarView->AddTool (TOOLBAR_AX, wxT("Axial"),
			  wxBitmap (axial_xpm),
			  wxNullBitmap, wxITEM_CHECK, wxT ("Axial view full screen"),
			  wxT ("Axial view full screen"));
  m_ToolBarView->AddTool (TOOLBAR_COR, wxT("Coronal"),
			  wxBitmap (coronal_xpm),
			  wxNullBitmap, wxITEM_CHECK, wxT ("Coronal view full screen"),
			  wxT ("Coronal view full screen"));
  m_ToolBarView->AddTool (TOOLBAR_SAG, wxT("Sagittal"),
			  wxBitmap (sagittal_xpm),
			  wxNullBitmap, wxITEM_CHECK, wxT ("Sagittal view full screen"),
			  wxT ("Sagittal view full screen"));
  wxCheckBox* cb4 = new wxCheckBox (m_ToolBarView, TOOLBAR_AXISCB, wxT("2D Axis"));
  m_ToolBarView->AddControl (cb4);
  
  m_ToolBarView->AddTool (TOOLBAR_3D, wxT("3D"),
			  wxBitmap (fullscreen),
			  wxNullBitmap, wxITEM_CHECK, wxT ("3D view full screen"),
			  wxT ("3D view full screen"));

  m_ToolBarView->AddTool (TOOLBAR_3D_VR, wxT("Toggle MPR/VR"),
			  wxBitmap (volumerendering),
			  wxNullBitmap, wxITEM_NORMAL, wxT ("3D rendering (toggle)"),
			  wxT ("Toggle the rendering mode of the 3D view "));
  
  wxChoice* cb1 = new wxChoice (m_ToolBarView, TOOLBAR_VRCHOICE, wxDefaultPosition, wxSize(100,-1));
  cb1->Append (wxT("3D mode:"));
  cb1->Append (wxT("Texture"));
  //cb1->Append (wxT("MIP"));
  cb1->SetSelection (0);
  
  m_ToolBarView->AddControl (cb1);

  wxCheckBox* cb2 = new wxCheckBox (m_ToolBarView, TOOLBAR_SHADECB, wxT("Shading"));
  m_ToolBarView->AddControl (cb2);
  
  m_ToolBarView->AddTool (TOOLBAR_FLIP, wxT("Rotate/Flip"),
			  wxBitmap (rotate),
			  wxNullBitmap, wxITEM_NORMAL, wxT ("Rotate/Flip current image"),
			  wxT ("Rotate/Flip the current image"));
  m_ToolBarView->AddTool (TOOLBAR_LINK, wxT("Link/Unlink"),
			  wxBitmap (togglelink),
			  wxNullBitmap, wxITEM_CHECK, wxT ("Link views in preview tab"),
			  wxT ("Link all views in the preview tab"));
  m_ToolBarView->Realize();

#ifdef __WXMAC__
  cb1->Show(false);
  cb2->Show(false);
  m_ToolBarView->Realize();
  cb1->Show(true);
  cb2->Show(true);
  m_ToolBarView->Realize();
#endif


  m_ToolBarInteraction = new wxToolBar (this, wxID_ANY,
					wxDefaultPosition,								   
					wxDefaultSize,
					wxTB_HORIZONTAL|wxTB_FLAT|wxTB_NODIVIDER);
  m_ToolBarInteraction->SetToolBitmapSize(wxSize(32,32));
#ifdef __WXMAC__
  m_ToolBarInteraction->AddTool (TOOLBAR_SELECTOR, wxT("Select"),
                                 wxBitmap (selector2_xpm),
                                 wxNullBitmap, wxITEM_RADIO, wxT ("Select a 2D slice"),
                                 wxT ("Select a 2D slice"));
#else
  m_ToolBarInteraction->AddTool (TOOLBAR_SELECTOR, wxT("Select"),
                                 wxBitmap (selector_xpm),
                                 wxNullBitmap, wxITEM_RADIO, wxT ("Select a 2D slice"),
                                 wxT ("Select a 2D slice"));
#endif
  m_ToolBarInteraction->AddTool (TOOLBAR_GREYSCALE, wxT("Windowing"),
                                 wxBitmap (greyscale_xpm),
                                 wxNullBitmap, wxITEM_RADIO, wxT ("Change the window/level"),
                                 wxT ("Change the window/level"));
  m_ToolBarInteraction->AddTool (TOOLBAR_ZOOM, wxT("Zoom"),
                                 wxBitmap (zoom_xpm),
                                 wxNullBitmap, wxITEM_RADIO, wxT ("Zoom"),
                                 wxT ("Zoom"));

  wxChoice* cb = new wxChoice (m_ToolBarInteraction, TOOLBAR_CLUT, wxDefaultPosition, wxSize(100,-1));
  cb->Append (wxT("Color Map:"));
  
  std::vector<std::string> lutNames = vtkLookupTableManager::GetAvailableLookupTables();
  for( unsigned int i=0; i<lutNames.size(); i++)
  {
    cb->Append(wxString(lutNames[i].c_str(), wxConvUTF8));
  }
  cb->SetSelection (0);

  m_ToolBarInteraction->AddControl (cb);


  wxChoice* cb3 = new wxChoice (m_ToolBarInteraction, TOOLBAR_PRESET, wxDefaultPosition, wxSize(100,-1));
  cb3->Append (wxT("3D Preset:"));
  cb3->Append(wxT("VR Muscles and Bones"));
  cb3->SetSelection (0);

  m_ToolBarInteraction->AddControl (cb3);
  
  m_ToolBarInteraction->Realize();

#ifdef __WXMAC__
  cb->Show(false);
  m_ToolBarInteraction->Realize();
  cb->Show(true);
  m_ToolBarInteraction->Realize();
#endif

  // Create a right click menu
  /*wxMenu *rmenu = new wxMenu();
  wxMenuItem* item = new wxMenuItem(rmenu, MENU_EDIT_DELETE_PAGE, wxT("Close Tab\tCtrl+F4"), wxT("Close Tab"), wxITEM_NORMAL);
  rmenu->Append(item);*/
  
  //long bookStyle = wxFNB_TABS_BORDER_SIMPLE|wxFNB_VC8;
  long bookStyle = wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxNO_BORDER | wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_WINDOWLIST_BUTTON;
  m_MainNotebook = new wxAuiNotebook (this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);
  
  
  this->SetProperties();
  this->DoLayout();
}

BEGIN_EVENT_TABLE(wxImageViewer, wxWindow)
  EVT_MENU(TOOLBAR_OPEN,			wxImageViewer::OnToolBarOpenVol)
  EVT_MENU(TOOLBAR_OPENDICOM,			wxImageViewer::OnToolBarOpenDICOM)
  EVT_MENU(TOOLBAR_SAVE,			wxImageViewer::OnToolBarSave)
  EVT_MENU(TOOLBAR_SAVEALL,			wxImageViewer::OnToolBarSaveAll)
  EVT_MENU(TOOLBAR_SAVEDICOM,			wxImageViewer::OnToolBarSaveDicom)
  EVT_MENU(TOOLBAR_SNAP,			wxImageViewer::OnToolBarSnapShot)
  EVT_MENU(TOOLBAR_AX, 			        wxImageViewer::OnFullScreenAxial)
  EVT_MENU(TOOLBAR_COR,			        wxImageViewer::OnFullScreenCoronal)
  EVT_MENU(TOOLBAR_SAG,			        wxImageViewer::OnFullScreenSagittal)
  EVT_MENU(TOOLBAR_3D,			        wxImageViewer::OnFullScreen3D)
  EVT_MENU(TOOLBAR_3D_VR,                       wxImageViewer::On3DVolumeRendering)
  EVT_MENU(TOOLBAR_FLIP,			wxImageViewer::OnToolBarFlip)
  EVT_MENU(TOOLBAR_LINK,			wxImageViewer::OnToolBarLink)
  EVT_MENU(TOOLBAR_SELECTOR,		        wxImageViewer::OnSelectInteractionEvent)
  EVT_MENU(TOOLBAR_GREYSCALE,		        wxImageViewer::OnWindowLevelInteractionEvent)
  EVT_MENU(TOOLBAR_ZOOM,			wxImageViewer::OnZoomInteractionEvent)
  EVT_CHOICE(TOOLBAR_CLUT,                      wxImageViewer::OnToolBarCLUT)
  EVT_CHOICE(TOOLBAR_PRESET,                    wxImageViewer::OnToolBarPreset)
  EVT_CHOICE(TOOLBAR_VRCHOICE,                  wxImageViewer::OnToolBarVRMode)
  EVT_CHECKBOX(TOOLBAR_SHADECB,                 wxImageViewer::OnToolBarShading)
  EVT_CHECKBOX(TOOLBAR_AXISCB,                  wxImageViewer::OnToolBarAxis)
  EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY,          wxImageViewer::OnDeletePage)
  EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY,        wxImageViewer::OnPageChanged)
END_EVENT_TABLE()

void wxImageViewer::SetProperties()
{
  m_ToolBarView->ToggleTool (TOOLBAR_LINK, true);
}


void wxImageViewer::DoLayout()
{
  m_FrameManager.SetManagedWindow (this);

#if defined(__WXGTK__) || defined(__WXMAC__)
  m_FrameManager.AddPane (m_ToolBarView, wxAuiPaneInfo().
                          Name(wxT("tb2")).Caption(wxT("View ToolBar")).
                          ToolbarPane().Top().
                          LeftDockable(false).RightDockable(false).CloseButton (false));
  m_FrameManager.AddPane (m_ToolBarInteraction, wxAuiPaneInfo().
                          Name(wxT("tb3")).Caption(wxT("2D interaction ToolBar")).
                          ToolbarPane().Top().
                          LeftDockable(false).RightDockable(false).CloseButton (false));
  m_FrameManager.AddPane (m_ToolBarOpenSave, wxAuiPaneInfo().
                          Name(wxT("tb1")).Caption(wxT("Open/Save ToolBar")).
                          ToolbarPane().Top().
                          LeftDockable(false).RightDockable(false).CloseButton (false));
#else
  m_FrameManager.AddPane (m_ToolBarOpenSave, wxAuiPaneInfo().
                          Name(wxT("tb1")).Caption(wxT("Open/Save ToolBar")).
                          ToolbarPane().Top().
                          LeftDockable(false).RightDockable(false).CloseButton (false));
  m_FrameManager.AddPane (m_ToolBarInteraction, wxAuiPaneInfo().
                          Name(wxT("tb3")).Caption(wxT("2D interaction ToolBar")).
                          ToolbarPane().Top().
                          LeftDockable(false).RightDockable(false).CloseButton (false));
  m_FrameManager.AddPane (m_ToolBarView, wxAuiPaneInfo().
                          Name(wxT("tb2")).Caption(wxT("View ToolBar")).
                          ToolbarPane().Top().
                          LeftDockable(false).RightDockable(false).CloseButton (false));
#endif
  m_FrameManager.AddPane (m_MainNotebook, wxAuiPaneInfo().Name(wxT("MainWindow")).
                          CenterPane());

  m_FrameManager.Update();

  this->SetSize (wxSize (100,100));
  
}

void wxImageViewer::OnToolBarOpenVol(wxCommandEvent &event)
{
  std::cout << "wxImageViewer::OnToolBarOpenVol not implemented yet." << std::endl;
}
void wxImageViewer::OnToolBarOpenDICOM(wxCommandEvent &event)
{
  std::cout << "wxImageViewer::OnToolBarOpenDICOM not implemented yet." << std::endl;
}

void wxImageViewer::OnToolBarSave(wxCommandEvent &event)
{
  std::cout << "wxImageViewer::OnToolBarSave not implemented yet." << std::endl;
}
void wxImageViewer::OnToolBarSaveAll(wxCommandEvent &event)
{
  std::cout << "wxImageViewer::OnToolBarSaveAll not implemented yet." << std::endl;
}
void wxImageViewer::OnToolBarSaveDicom(wxCommandEvent &event)
{
  std::cout << "wxImageViewer::OnToolBarSaveDicom not implemented yet." << std::endl;
}
void wxImageViewer::OnToolBarSnapShot(wxCommandEvent &event)
{
  std::cout << "wxImageViewer::OnToolBarSnapShot not implemented yet." << std::endl;
}
void wxImageViewer::OnFullScreenAxial(wxCommandEvent &event)
{
	std::cout << "wxImageViewer::OnFullScreenAxial not implemented yet." << std::endl;
}
void wxImageViewer::OnFullScreenCoronal(wxCommandEvent &event)
{
	std::cout << "wxImageViewer::OnFullScreenCoronal not implemented yet." << std::endl;
}
void wxImageViewer::OnFullScreenSagittal(wxCommandEvent &event)
{
	std::cout << "wxImageViewer::OnFullScreenSagittal not implemented yet." << std::endl;
}
void wxImageViewer::OnFullScreen3D(wxCommandEvent &event)
{
	std::cout << "wxImageViewer::OnFullScreen3D not implemented yet." << std::endl;
}
void wxImageViewer::OnToolBarFlip(wxCommandEvent &event)
{
	std::cout << "wxImageViewer::OnToolBarFlip not implemented yet." << std::endl;
}
void wxImageViewer::OnToolBarLink(wxCommandEvent &event)
{
	std::cout << "wxImageViewer::OnToolBarLink not implemented yet." << std::endl;
}

void wxImageViewer::OnDeletePage(wxAuiNotebookEvent& event)
{
	std::cout << "wxImageViewer::OnDeletePage not implemented yet." << std::endl;
}

void wxImageViewer::OnPageChanged(wxAuiNotebookEvent& event)
{
	std::cout << "wxImageViewer::OnPageChanged not implemented yet." << std::endl;
}

void wxImageViewer::OnSelectInteractionEvent(wxCommandEvent& event)
{
	std::cout << "wxImageViewer::OnSelectInteractionEvent not implemented yet." << std::endl;
}

void wxImageViewer::OnWindowLevelInteractionEvent(wxCommandEvent& event)
{
	std::cout << "wxImageViewer::OnWindowLevelInteractionEvent not implemented yet." << std::endl;
}

void wxImageViewer::OnZoomInteractionEvent(wxCommandEvent& event)
{
	std::cout << "wxImageViewer::OnZoomInteractionEvent not implemented yet." << std::endl;
}

void wxImageViewer::OnFullPageInteractionEvent(wxCommandEvent& event)
{
	std::cout << "wxImageViewer::OnFullPageInteractionEvent not implemented yet." << std::endl;
}

void wxImageViewer::On3DVolumeRendering(wxCommandEvent& event)
{
	std::cout << "wxImageViewer::On3DVolumeRendering not implemented yet." << std::endl;
}

void wxImageViewer::OnToolBarCLUT(wxCommandEvent& event)
{
        std::cout << "wxImageViewer::OnToolBarCLUT not implemented yet." << std::endl;
}

void wxImageViewer::OnToolBarPreset(wxCommandEvent& event)
{
        std::cout << "wxImageViewer::OnToolBarPreset not implemented yet." << std::endl;
}

void wxImageViewer::OnToolBarVRMode(wxCommandEvent& event)
{
        std::cout << "wxImageViewer::OnToolBarVRMode not implemented yet." << std::endl;
}

void wxImageViewer::OnToolBarShading(wxCommandEvent& event)
{
        std::cout << "wxImageViewer::OnToolBarShading not implemented yet." << std::endl;
}

void wxImageViewer::OnToolBarAxis(wxCommandEvent& event)
{
        std::cout << "wxImageViewer::OnToolBarAxis not implemented yet." << std::endl;
}
