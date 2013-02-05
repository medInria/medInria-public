///////////////////////////////////////////////////////////////////////////////
// Name:		styled_notebook.cpp
// Purpose:     generic implementation of flat style notebook class.
// Author:      Eran Ifrah <eranif@bezeqint.net>
// Created:     01/09/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license <http://www.wxwidgets.org/licence3.txt>
///////////////////////////////////////////////////////////////////////////////

#ifndef WXSTYLESNOTEBOOK_H
#define WXSTYLESNOTEBOOK_H

#include <wx/wxFlatNotebook.h>
#include "vtkINRIA3DConfigure.h"

#define wxFNB_BACKGROUND_GRADIENT 1024

#define wxFNB_COLORFUL_TABS		  2048

WX_DECLARE_USER_EXPORTED_OBJARRAY(wxRect, wxRectArray, WX_ADDON_EXPORT);

/// The tab area window
class WX_ADDON_EXPORT wxStyledTabsContainer : public wxPageContainerBase
{
	int m_factor;
public:
	/// Parametrized constructor
	/**
	 *
	 * \param parent parent window
	 * \param id window ID
	 * \param pos window position
	 * \param size window size
	 * \param style window style
	 * \return 
	 */
	wxStyledTabsContainer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	/// Destructor
	virtual ~wxStyledTabsContainer(void) {};

	virtual int HitTest(const wxPoint& pt, wxPageInfo& pageInfo, int &tabIdx);

	void SetFactor(int factor) 
	{
		m_factor = factor;
		Refresh();
	}
	int GetFactor() const { return m_factor; }

	DECLARE_EVENT_TABLE()
	/// Override the OnPaint event
	virtual void OnPaint(wxPaintEvent& event);

private:
	void DrawVC8Tab(wxBufferedPaintDC& dc, const int& posx, const int &tabIdx, const int &tabWidth, const int &tabHeight);

	/// Fill a tab with gradient color
	/// an exception is the selected tab which will colored
	/// with solid white background
	virtual void FillVC8GradientColor(wxBufferedDC& dc, const wxPoint tabPoints[], const bool bSelectedTab, const int tabIdx);

	/// Calculate the number of tabs this page can fit, and return their
	/// position
	/// Starting from m_nFrom
	void NumberTabsCanFit(wxDC& dc, wxRectArray& vTabInfo);

	// VC8 gradient coloring helper methods
	int GetStartX(const wxPoint tabPoints[], const int &y);
	int GetEndX(const wxPoint tabPoints[], const int &y);

	// Is default tabs in use?
	virtual bool IsDefaultTabs();

	/// Some styles does not allow drawing X on the active tab
	virtual bool CanDrawXOnTab();

	/// Return the color of the single border 
	virtual wxColor GetSingleLineBorderColor();

	/// Return the number of tabs that can be scrolled left
	virtual int GetNumTabsCanScrollLeft();

	/// Generate random tab coloring
	virtual wxColor GenTabColour();
};

class WX_ADDON_EXPORT wxStyledNotebook : public wxFlatNotebookBase 
{
public:
	/**
	 *
	 * \param parent parent window
	 * \param id window ID
	 * \param pos window position
	 * \param size window size
	 * \param style window style
	 * \param name window class name
	 * \return 
	 */
	wxStyledNotebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("StyledNotebook")) : 
	  wxFlatNotebookBase(parent, id, pos, size, style, name)
	  {
		  m_pages = CreatePageContainer();
		  Init();

		  /// Custome initialization of the tab area
		  if( style & wxFNB_VC8 )
		  {
			  /// Initialise the default style colors
			  SetNonActiveTabTextColour( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT) );
		  }
	  }

	/// Destructor	
	 ~wxStyledNotebook(void)
	  {}

	  wxPageContainerBase* CreatePageContainer()
	  {
		  return new wxStyledTabsContainer(this, wxID_ANY);
	  }
};

#endif // WXSTYLESNOTEBOOK_H
