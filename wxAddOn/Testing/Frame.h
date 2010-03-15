/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: Frame.h 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 18:46:10 +0100 (mar, 20 nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef Frame_h_
#define Frame_h_

#include <wx/wx.h>
#include <wx/styled_notebook.h>

class Frame :
	public wxFrame
{
public:
	// Construction of MainFrame
	Frame(wxWindow* parent, const wxChar *title);
	virtual ~Frame(void);
	bool Create(wxFrame * parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style );
	wxWindow* CreatePage();

	// Menus
	wxMenu* m_fileMenu;
	wxMenu* m_editMenu;
	wxMenuBar* m_menuBar;
	wxFlatNotebookImageList m_ImageList;

#ifdef DEVELOPMENT
	wxLog *m_logTargetOld;
#endif

	// Event handlers

	// Menu
	virtual void OnQuit(wxCommandEvent& WXUNUSED(event));
	virtual void OnDeleteAll(wxCommandEvent& WXUNUSED(event));
	virtual void OnAddPage(wxCommandEvent& WXUNUSED(event));
	virtual void OnDeletePage(wxCommandEvent& WXUNUSED(event));
	virtual void OnSetSelection(wxCommandEvent& WXUNUSED(event));
	virtual void OnAdvanceSelectionFwd(wxCommandEvent& WXUNUSED(event));
	virtual void OnAdvanceSelectionBack(wxCommandEvent& WXUNUSED(event));
	virtual void OnShowImages(wxCommandEvent& WXUNUSED(event));
	virtual void OnVC71Style(wxCommandEvent& WXUNUSED(event));
	virtual void OnVC8Style(wxCommandEvent& WXUNUSED(event));
	virtual void OnDefaultStyle(wxCommandEvent& WXUNUSED(event));
	virtual void OnFancyStyle(wxCommandEvent& WXUNUSED(event));
	virtual void OnSelectColor(wxCommandEvent& event);
	virtual void OnSetPageImageIndex(wxCommandEvent& event);
	virtual void OnStyle(wxCommandEvent& event);
	virtual void OnDrawTabX(wxCommandEvent& event);
	virtual void OnGradientBack(wxCommandEvent& event);
	virtual void OnColorfullTabs(wxCommandEvent& event);

	virtual void OnEnableTab(wxCommandEvent& event);
	virtual void OnDisableTab(wxCommandEvent& event);
	virtual void OnEnableDrag(wxCommandEvent& event);
	virtual void OnDClickCloseTab(wxCommandEvent& event);

	// Notebook
	virtual void OnPageChanging(wxFlatNotebookEvent& event);
	virtual void OnPageChanged(wxFlatNotebookEvent& event);
	virtual void OnPageClosing(wxFlatNotebookEvent& event);
	virtual void OnSetAllPagesShapeAngle(wxCommandEvent& WXUNUSED(event));

	wxStyledNotebook *book;
	wxStyledNotebook *secondBook;
	bool m_bShowImages;
	bool m_bVCStyle;

	DECLARE_EVENT_TABLE()
};

#endif
