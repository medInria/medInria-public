/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id:  1182 2009-06-30 09:19:05Z  $
Language:  C++
Author:    $Author:  $
Date:      $Date: 2009-06-30 11:19:05 +0200 (mar, 30 jun 2009) $
Version:   $Revision: 1182 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
//---------------------------------------------------------------------------
// $RCSfile: MultiCtrlTest.cpp,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/MultiCtrlTest.cpp,v $
// $Revision: 1.1 $
// $Date: 2006/08/03 15:42:43 $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

enum
{
	ID_BUTTON1=100,
	ID_BUTTON2,
	ID_BUTTON3,
	ID_RADIO1,
	ID_RADIO2,
	ID_RADIO3,
	ID_RADIO4,
	ID_RADIO5,
	ID_RADIO6,
	ID_CHKSELECT, 
	ID_COLLAPSE,
	ID_CHECK1,
	ID_SPACINGY,
	ID_ADJUST,
	ID_DELETENODE,
	ID_ADDNODE,
	ID_DELETEALL,
	ID_CHOICES1,
	ID_SETTEXT,
	ID_RESETSEL,
};

#include "wx/wxTreeMultiCtrl.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyAppFrame : public wxFrame
{
public:
    // ctor(s)
    MyAppFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            long style = wxDEFAULT_FRAME_STYLE);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnCollapse(wxCommandEvent &event);
	void OnCheck(wxCommandEvent &event);
	void OnShowAll(wxCommandEvent &event);
	void OnCollapseNodes(wxCommandEvent &event);
	void OnAdjustY(wxCommandEvent &event);
	void OnDeleteNode(wxCommandEvent &event);
	void OnAddNode(wxCommandEvent &event);
	void OnDeleteAllNodes(wxCommandEvent &event);
	void OnSelectChoice1(wxCommandEvent &event);
	void OnResetChoice1(wxCommandEvent &event);

private:
    wxTreeMultiCtrl *_tmc;
	// any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    MultiCtrlTest_Quit = 1,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MultiCtrlTest_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyAppFrame, wxFrame)
    EVT_MENU(MultiCtrlTest_Quit,  MyAppFrame::OnQuit)
    EVT_MENU(MultiCtrlTest_About, MyAppFrame::OnAbout)
	EVT_BUTTON(ID_BUTTON1, MyAppFrame::OnAbout)
	EVT_BUTTON(ID_BUTTON2, MyAppFrame::OnCollapse)
	EVT_BUTTON(ID_BUTTON3, MyAppFrame::OnShowAll)
	EVT_BUTTON(ID_CHKSELECT, MyAppFrame::OnCheck)
	EVT_BUTTON(ID_COLLAPSE, MyAppFrame::OnCollapseNodes)
	EVT_BUTTON(ID_ADJUST, MyAppFrame::OnAdjustY)
	EVT_BUTTON(ID_DELETENODE, MyAppFrame::OnDeleteNode)
	EVT_BUTTON(ID_ADDNODE, MyAppFrame::OnAddNode)
	EVT_BUTTON(ID_DELETEALL, MyAppFrame::OnDeleteAllNodes)
	EVT_COMBOBOX(ID_CHOICES1, MyAppFrame::OnSelectChoice1)
	EVT_CHECKBOX(ID_RESETSEL, MyAppFrame::OnResetChoice1)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
	wxString str;

	str = _T("MultiCtrlTest wxWindows App - ");
	str.Append(wxVERSION_STRING);
    
	// create the main application window
    MyAppFrame *frame = new MyAppFrame(str, wxPoint(50, 50), wxSize(500, 500));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyAppFrame::MyAppFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
       : wxFrame(NULL, -1, title, pos, size, style)
{
	wxString choices1[] = { _T("Choice1"), _T("Choice2"), _T("Choice3") } ;

	wxBoxSizer *sizer;
    
	// set the frame icon
    SetIcon(wxICON(mondrian));

	// --------------
	// Something with collapsing, tested:
	//
	//		- CollapseNodes
	//		- ExpandNodes
	//		- Collapse
	//		- GetBooleanValue
	//		- FindItem
	// --------------
	
	wxPanel *pnl;
	_tmc = new wxTreeMultiCtrl(this, -1);

	_tmc->SetBackgroundColour(*wxWHITE);

#ifndef LINUX
	wxTreeMultiWindowInfo wndinfo(wxTMC_BG_ADJUST_CNT, 8, 0);
#else
	wxTreeMultiWindowInfo wndinfo(wxTMC_BG_ADJUST_ALL, 8, 0);
#endif

	// make it a bit bigger
	wxFont somefont;
	somefont = _tmc->GetCaptionFont();
	somefont.SetFamily(wxMODERN);
	somefont.SetWeight(wxBOLD);
	somefont.SetPointSize(somefont.GetPointSize()+2);
	_tmc->SetCaptionFont(somefont);


	wxTreeMultiItem item = _tmc->AddRoot(_T("Some fun collapse stuff"), _T("COLLAPSE"));

	pnl = new wxPanel(_tmc, -1);
	sizer = new wxBoxSizer(wxHORIZONTAL);
	wxRadioButton *rb1 = new wxRadioButton( pnl, ID_RADIO4, _T("Collapse this one"));
	rb1->SetValue(true);
	sizer->Add(rb1, 0, wxALL, 2 );
	sizer->Add(new wxRadioButton( pnl, ID_RADIO5, _T("Collapse all")), 1, wxALL, 2 );
	sizer->Add(new wxRadioButton( pnl, ID_RADIO6, _T("Expand all")), 1, wxALL, 2 );
	sizer->Add(new wxCheckBox( pnl, ID_CHECK1, _T("Recursive")), 0, wxALL, 2 );
    pnl->SetSizer(sizer);

	_tmc->AppendWindow(item, pnl, _T(""), wndinfo, wxTMC_SPAN_WIDTH);

	wndinfo.Indent(4);

	_tmc->AppendWindow(item, new wxButton(_tmc, ID_COLLAPSE, _T("Activate!")), _T(""), wndinfo);
	
	wndinfo.Outdent(4);

	// --------------
	// Something with spacing, tested:
	//
	//		- GetTextValue
	//		- SetSpacingY
	// --------------

	item = _tmc->AddRoot(_T("Spacing and gutter size"));

	_tmc->AppendWindow(item, new wxTextCtrl(_tmc, ID_SPACINGY, _T("10"), wxDefaultPosition, wxSize(40,-1)), _T(""), wndinfo);
	wndinfo.Indent(4);
	_tmc->AppendWindow(item, new wxButton(_tmc, ID_ADJUST, _T("Adjust")), _T(""), wndinfo);

	wndinfo.Outdent(4);
	
	// --------------
	// Deletion / Adding:
	//
	//		- DeleteChildren
	//		- AppendWindow
	//		- Delete
	//		- CheckboxView
	// --------------
#if(CHECKBOXVIEW)
	_tmc->SetCheckboxView(true);
#endif

	item = _tmc->AddRoot(_T("Deleting and adding ..."));
	_tmc->AppendWindow(item, new wxStaticText(_tmc, -1, _T("Delete or add some extra windows")), _T(""), wndinfo);

	pnl = new wxPanel(_tmc, -1);
	sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxButton(pnl, ID_DELETENODE, _T("Delete a Node")), 0, wxALIGN_CENTER | wxALL, 2 );
	sizer->Add(new wxButton(pnl, ID_ADDNODE, _T("Add a Node")), 0, wxALIGN_CENTER | wxALL, 2 );
	sizer->Add(new wxButton(pnl, ID_DELETEALL, _T("Delete all")), 0, wxALIGN_CENTER | wxALL, 2 );
	pnl->SetSizer(sizer);

	_tmc->AppendWindow(item, pnl , _T(""), wndinfo);

	wxTreeMultiItem node1 = _tmc->AppendNode(item, _T("Delete or added nodes are shown here"), _T("ADDDEL"));

#if(CHECKBOXVIEW)
	_tmc->SetCheckboxView(false);
#endif

	//pnl = new wxPanel(_tmc, -1);
	//sizer = new wxBoxSizer(wxHORIZONTAL);
	//sizer->Add(new wxCheckBox( pnl, -1, "Add email addresses to my:"), 0, wxALIGN_CENTER, 0 );
	//sizer->Add(new wxComboBox( pnl, -1, "Item1"));
	
	//pnl->SetSizer(sizer);
	//_tmc->AppendWindow(item, pnl, "", wndinfo);

	item = _tmc->AddRoot(_T("Some get and set methods"));

	pnl = new wxPanel(_tmc, -1);
	sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxCheckBox( pnl, ID_RESETSEL, _T("Activate choice  ")), 0, wxALL, 0 );
	sizer->Add(new wxComboBox( pnl, ID_CHOICES1, _T(""), wxDefaultPosition, wxSize(150,-1), 3, choices1, wxCB_READONLY ), 0, wxALL, 0 );
	pnl->SetSizer(sizer);

	_tmc->AppendWindow(item, pnl, _T(""), wndinfo, wxTMC_SPAN_WIDTH);

	_tmc->AppendWindow(item, new wxStaticText(_tmc, -1, _T("The selection text will be placed here")), _T(""), wndinfo);

	wndinfo.Indent(2);

	_tmc->AppendWindow(item, new wxTextCtrl(_tmc, ID_SETTEXT, _T(""), wxDefaultPosition, wxSize(50,-1)), _T(""), wndinfo, wxTMC_SPAN_WIDTH);

	wndinfo.Outdent(2);

	_tmc->AppendWindow(item, new wxStaticText(_tmc, -1, _T("NB. Check the checkbox to reset the combobox with index 0, clear is index 2")), _T(""), wndinfo);

	item = _tmc->AddRoot(_T("Additional Options"));
	_tmc->Collapse(item, false);

	// add collapsed windows, they should not be visible
	_tmc->AppendWindow(item, new wxStaticText(_tmc, -1, _T("Some fun include and exclude testing")), _T(""), wndinfo);

	wxTreeMultiItem excl = _tmc->AppendNode(item, _T("Include and exclude"), _T("A01A00"));
	//_tmc->AppendWindow(item, new wxCheckBox(_tmc, -1, _T("Enable spam settings"), wxDefaultPosition, wxSize(200,20)), _T(""), wndinfo);
	
	item = _tmc->AppendNode(excl, _T("Press the button to exclude this node"), _T("EXCLUDE_ME"));
	_tmc->AppendWindow(item, new wxButton(_tmc, ID_BUTTON1, _T("Exclude")), _T(""), wndinfo);

	item = _tmc->AppendNode(excl, _T("Press the button to include!"), _T("INCLUDE_ME"));
	_tmc->AppendWindow(item, new wxButton(_tmc, ID_BUTTON2, _T("Include")), _T(""), wndinfo);
	_tmc->Exclude(item);

	item = _tmc->AppendNode(excl, _T("Press this button to show both"), _T("SHOW_ALL"));
	_tmc->AppendWindow(item, new wxButton(_tmc, ID_BUTTON3, _T("Include all!")), _T(""), wndinfo);

	// -- 

	item = _tmc->AddRoot(_T("Window finding by TreeMultiNodeItem"), _T("CHKSELECT"));

	pnl = new wxPanel(_tmc, -1);
	sizer = new wxBoxSizer(wxVERTICAL);

	sizer->Add(new wxRadioButton( pnl, ID_RADIO1, _T("Option 1")), 0, wxALL, 4 );
	sizer->Add(new wxRadioButton( pnl, ID_RADIO2, _T("Option 2")), 0, wxALL, 4 );
	sizer->Add(new wxRadioButton( pnl, ID_RADIO3, _T("Option 3")), 0, wxALL, 4 );
	sizer->Add(new wxStaticText(pnl, -1, _T("Now which item is selected? Press button ..")), 0, wxALL, 4 );

	pnl->SetSizer(sizer);

	_tmc->AppendWindow(item, pnl,_T(""), wndinfo );

	wndinfo.Indent(4);
	_tmc->AppendWindow(item,new wxButton(_tmc, ID_CHKSELECT, _T("Check!")),_T(""), wndinfo );

	wndinfo.Outdent(4);

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(MultiCtrlTest_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(MultiCtrlTest_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR

}


// event handlers

void MyAppFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void MyAppFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    // exclude one, include the other
	wxTreeMultiItem item = _tmc->FindItem(_T("EXCLUDE_ME"));
	
	if(item.IsOk())
		_tmc->Exclude(item);

	item = _tmc->FindItem(_T("INCLUDE_ME"));
	if(item.IsOk())
		_tmc->Include(item);

}

void MyAppFrame::OnShowAll(wxCommandEvent& WXUNUSED(event))
{
    // exclude one, include the other
	wxTreeMultiItem item = _tmc->FindItem(_T("EXCLUDE_ME"));
	
	if(item.IsOk())
		_tmc->Include(item);

	item = _tmc->FindItem(_T("INCLUDE_ME"));
	if(item.IsOk())
		_tmc->Include(item);

}


void MyAppFrame::OnCollapse(wxCommandEvent &event)
{
    // exclude one, include the other
	wxTreeMultiItem item = _tmc->FindItem(_T("EXCLUDE_ME"));
	
	if(item.IsOk())
		_tmc->Include(item);

	item = _tmc->FindItem(_T("INCLUDE_ME"));
	if(item.IsOk())
		_tmc->Exclude(item);
}

void MyAppFrame::OnCheck(wxCommandEvent &event)
{
	wxTreeMultiItem item;

	wxString str;
	if(_tmc->GetBooleanValue(ID_RADIO1))
		str = _T("Option 1");
	else if(_tmc->GetBooleanValue(ID_RADIO2))
		str = _T("Option 2");
	else if(_tmc->GetBooleanValue(ID_RADIO3))
		str = _T("Option 3");
	else
		str = _T("No option");
		
	str.Append(_T(" is selected!"));

	wxMessageBox(str, _T("The votes are in .. "), wxOK | wxICON_INFORMATION, this);
}

void MyAppFrame::OnCollapseNodes(wxCommandEvent &event)
{
	bool recursive = _tmc->GetBooleanValue(ID_CHECK1);

	if(_tmc->GetBooleanValue(ID_RADIO4))
	{
		wxTreeMultiItem item = _tmc->FindItem(_T("COLLAPSE"));
		if(item.IsNodeItem())
			_tmc->Collapse(item, recursive);
	}
	else if(_tmc->GetBooleanValue(ID_RADIO5))
	{
		_tmc->CollapseNodes(recursive);
	}
	else if(_tmc->GetBooleanValue(ID_RADIO6))
	{
		_tmc->ExpandNodes(recursive);	
	}
}

void MyAppFrame::OnAdjustY(wxCommandEvent &event)
{
	wxString val = _tmc->GetTextValue(ID_SPACINGY);
	long value = 0;

	if(val.ToLong(&value, 10))
		_tmc->SetSpacingY(value);

}

void MyAppFrame::OnDeleteNode(wxCommandEvent &event)
{
	// find node that holds items, and delete one

	wxTreeMultiItem item = _tmc->FindItem(_T("ADDDEL"));

	if(item.IsNodeItem())
	{
		// now get children count. With no items, report it
		// and exit

		if(_tmc->GetChildrenCount(item) == 0)
			wxMessageBox(_T("All items are already deleted!"), _T("Sorry.. "), wxOK | wxICON_ERROR, this);
		else
		{
			wxTreeMultiItem delnode = _tmc->GetLastChild(item);
			_tmc->Delete(delnode);
		}
	}
}

void MyAppFrame::OnAddNode(wxCommandEvent &event)
{
	wxString str;
	wxTreeMultiItem item = _tmc->FindItem(_T("ADDDEL"));

	if(item.IsNodeItem())
	{
		// Append one
		str.Printf(_T("This is item %i"), _tmc->GetChildrenCount(item));
		_tmc->AppendWindow(item, new wxStaticText(_tmc, -1, str));
	}
}

void MyAppFrame::OnDeleteAllNodes(wxCommandEvent &event)
{
	// find node that holds items, and delete one

	wxTreeMultiItem item = _tmc->FindItem(_T("ADDDEL"));

	if(item.IsNodeItem())
		_tmc->DeleteChildren(item);
}

void MyAppFrame::OnSelectChoice1(wxCommandEvent &event)
{
	// search the index, and display it in the text

	int idx = _tmc->GetSelectionValue(ID_CHOICES1);

	if(idx >=0)
		_tmc->SetTextValue(ID_SETTEXT, _tmc->GetTextValue(ID_CHOICES1));

}

void MyAppFrame::OnResetChoice1(wxCommandEvent &event)
{
	if(_tmc->GetBooleanValue(ID_RESETSEL))
		_tmc->SetSelectionValue(ID_CHOICES1, 0);
	else
		_tmc->SetSelectionValue(ID_CHOICES1, 2);
}
