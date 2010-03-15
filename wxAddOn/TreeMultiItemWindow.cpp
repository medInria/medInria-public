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
// $RCSfile: TreeMultiItemWindow.cpp,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/TreeMultiItemWindow.cpp,v $
// $Revision: 1.3 $
// $Date: 2007/01/16 13:27:33 $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

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

#include "wx/TreeMultiItemWindow.h"

/** TreeMultiItemWindow
  *	This class is a possible end node for the tree. If you want multiple
  *	types of end-nodes you should copy this file and adjust it with a 
  *	different class name.
  */

TreeMultiItemWindow::TreeMultiItemWindow(TreeMultiItemNode *parent, const wxString &name)
	:  TreeMultiItemBase(parent)
	, _window(0)
	, _topSpacing(0)
	, _frontSpacing(0)
	, _span(false)
{
    _name = name;

	// TODO: Enter your constructor code here
}

TreeMultiItemWindow::~TreeMultiItemWindow()
{
	if(_window)
		_window->Destroy();
}

void TreeMultiItemWindow::AssignWindow(wxWindow *wnd)
{
	// delete previous, assign new (even if zero)
	if(_window)
		_window->Destroy();

	_window = wnd;

	// recalculate the size
	if(wnd)
	{
		wxSize size; 

		// resize the sized control by sizer or 
		// by single window. For a sizer handled window
		// GetBestSize returns the optimal size

		/// \todo This might need rivision for retaining the sizer size
		/// maybe we need to check whether size is <> 0,0 before get best size

		if(wnd->GetSizer())
			size = wnd->GetBestSize();
		else
			size = wnd->GetSize();

		_height = size.GetHeight();
		_width = size.GetWidth();

	}
	else
	{
		_height = 0;
		_width = 0;
	}
}

#if(CHECKBOXVIEW)

void TreeMultiItemWindow::SetCheckboxState(int state) {
	TreeMultiItemBase::SetCheckboxState(state);

	// enable or disable the window
	if(GetCheckbox() && GetWindow() && state != 2)
		GetWindow()->Enable(state == 1);
};

#endif // #if(CHECKBOXVIEW)
