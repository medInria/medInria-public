//---------------------------------------------------------------------------
// $RCSfile: TreeMultiItemWindow.h,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/wx/TreeMultiItemWindow.h,v $
// $Revision: 609 $
// $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifndef __TREEMULTIITEMWINDOW_HPP_
#define __TREEMULTIITEMWINDOW_HPP_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include <wx/dynarray.h>
#include "vtkINRIA3DConfigure.h"

#include "TreeMultiItemBase.h"

class WX_ADDON_EXPORT TreeMultiItemWindow : public TreeMultiItemBase
{
private:
	/** The simple control, or complex sizer that represents this window
		in the TreeMultiCtrl */
	wxWindow *_window;

	/** Top and front extra spacings, added to X, Y */
	int _topSpacing, _frontSpacing;

	/** Spanning flag */
	bool _span;

public:
	TreeMultiItemWindow(TreeMultiItemNode *parent, const wxString &name = wxEmptyString);
	virtual ~TreeMultiItemWindow();

	// IsClass method
	virtual TreeMultiItemWindow *IsTreeMultiItemWindow() const {
		return (TreeMultiItemWindow *)this;
	};
	
	/** Assigns the given window to this TreeMultiItemWindow, and also destroys the 
		present window. If the assigned window is 0, this item is not drawn
		and removed from the tree visually */
	void AssignWindow(wxWindow *wnd);

	/** Get window method */
	wxWindow *GetWindow() {
		return _window;
	};

	/* Sets front spacing */
	void SetFrontSpacing(int frontSpacing) {
		_frontSpacing = frontSpacing;
	};
	/* Gets front spacing */
	int GetFrontSpacing() const {
		return _frontSpacing;
	};
	/* Sets top spacing */
	void SetTopSpacing(int topSpacing) {
		_topSpacing = topSpacing;
	};
	/* Gets top spacing */
	int GetTopSpacing() const {
		return _topSpacing;
	};
	
	/* Sets horizontal span, meaning when this is set the window
	   attached may be resized horizontally when the window is resized */
	void SetHorizontalSpan(bool span) {
		_span = span;
	};

	/* Sets horizontal span, meaning when this is set the window
	   attached may be resized horizontally when the window is resized */
	bool GetHorizontalSpan() const {
		return _span;
	};

#if(CHECKBOXVIEW)
	/** Inherited from base, to enable or disable window */
	virtual void SetCheckboxState(int state);
#endif
};

#endif

