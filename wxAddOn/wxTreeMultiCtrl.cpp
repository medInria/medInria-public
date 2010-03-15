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
// $RCSfile: wxTreeMultiCtrl.cpp,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/wxTreeMultiCtrl.cpp,v $
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

#include "wx/wxTreeMultiCtrl.h"
#include "wx/arrimpl.cpp"
#include "wx/tmcimages.h"

//----------------------------------------------------------------------------
// wxTreeMultiCtrl
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeMultiCtrl, wxScrolledWindow)

// WDR: event table for wxTreeMultiCtrl
BEGIN_EVENT_TABLE(wxTreeMultiCtrl, wxScrolledWindow)
  EVT_LEFT_DOWN(wxTreeMultiCtrl::OnMouseClick)
  EVT_LEFT_DCLICK(wxTreeMultiCtrl::OnMouseClick)
  EVT_PAINT(wxTreeMultiCtrl::OnPaint)
  EVT_SIZE(wxTreeMultiCtrl::OnSize)
//  EVT_KEY_UP(wxTreeMultiCtrl::OnKey)
END_EVENT_TABLE()

bool wxTreeMultiCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                               const wxSize& size, long style, const wxValidator &validator,
                               const wxString& name )
{
    wxScrolledWindow::Create( parent, id, pos, size, style | wxTAB_TRAVERSAL);

	_create_called = true;
	
	// do the init
	Init();

    return TRUE;
}

void wxTreeMultiCtrl::Init()
{
	_root.Clear();

	_expandBmp = 0;
	_collBmp = 0;

#if(CHECKBOXVIEW)
	_checkBmp = 0;
	_uncheckBmp = 0;
	_tristateBmp = 0;

	_checkHeight = 11;
	_checkWidth = 11;

	_checkboxView = false;
#endif

	_gutterWidth = WXTMC_GUTTER_DEFAULT;
	_iconWidth = 11;
	_iconHeight = 11;
	_maxHeight = 1;;
	_iconDeltaY = 2;
	_spacingY = WXTMC_YSPACING_DEFAULT;
	_captionHeight = 13;

	// create two bitmap nodes for drawing

	_expandBmp = new wxBitmap(expand_xpm);
	_collBmp = new wxBitmap(collapse_xpm);

	// calculate average font height for bitmap centering

	_iconWidth = _expandBmp->GetWidth();
	_iconHeight = _expandBmp->GetHeight();

#if(CHECKBOXVIEW)
	// create bitmaps for checkboxes
	_checkBmp = new wxBitmap(checked_icon);
	_uncheckBmp = new wxBitmap(unchecked_icon);
	_tristateBmp = new wxBitmap(tristate_icon);

	// adjust the height if the checkboxes are higher
	// so that everything is alligned properly
	_checkHeight = _checkBmp->GetHeight();
	_checkWidth = _checkBmp->GetWidth();
#endif

	// remember the highest of the two bitmaps so there is
	// always enough room
	_maxHeight = _iconHeight;

#if(CHECKBOXVIEW)
	if(_maxHeight < _checkHeight)
		_maxHeight = _checkHeight;
#endif

	// set standard DC font
    _captionFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	//_captionFont = GetFont();

	// adjust bitmap icon y position so they are centered
	AdjustIconsDeltaY();

	// set virtual size to this window size
	if (_create_called) {
		wxSize wndsize = GetSize();
		SetVirtualSize(wndsize.GetWidth(), wndsize.GetWidth());
	}
}

void wxTreeMultiCtrl::SetCaptionFont(const wxFont &font)
{
	_captionFont = font;

	// adjust the icons so that they are in the middle
	AdjustIconsDeltaY();

	RedrawFromNode(0);
};

void wxTreeMultiCtrl::AdjustIconsDeltaY()
{
	int x = 0, y = 0;

	if(_captionFont.Ok())
      GetTextExtent(wxT("jG"), &x, &y, 0, 0, &_captionFont);
	_captionHeight = y;

	if(_maxHeight < _captionHeight)
		_maxHeight = _captionHeight;

	// determine the center pos for the [+] 
	_iconDeltaY = abs(_maxHeight - _iconHeight) / 2 + 1;
	if(_iconDeltaY < 1)
		_iconDeltaY = 1;
	
#if(CHECKBOXVIEW)
	// determine the center pos for the checkbox
	_checkDeltaY = abs(_maxHeight - _checkHeight) / 2 + 1;
	if(_checkDeltaY < 1)
		_checkDeltaY = 1;
#endif
}

wxTreeMultiCtrl::~wxTreeMultiCtrl()
{
	// delete the bitmap resources
	delete _expandBmp;
	delete _collBmp;

#if(CHECKBOXVIEW)
	delete _checkBmp;
	delete _uncheckBmp;
	delete _tristateBmp;
#endif
}

wxTreeMultiItem wxTreeMultiCtrl::AddRoot(const wxString &caption, const wxString &name)
{
	wxTreeMultiItem result((TreeMultiItemBase *)&_root);
	result = AppendNode(result, caption, name);

	return result;
}

wxTreeMultiItem wxTreeMultiCtrl::AppendWindow(const wxTreeMultiItem &item, wxWindow *window, const wxString &name,
										      const wxTreeMultiWindowInfo &info, int flags)
{
	// get flags from info class, else override
	int wflags = info.GetFlags();
	if(flags != 0)
		wflags = flags;

	// add window only if this node (parent) is valid, and a node
	wxCHECK(window, wxTreeMultiItem(0));
	wxCHECK(item.IsNodeItem(), wxTreeMultiItem(0));
	TreeMultiItemNode *parent = (TreeMultiItemNode *)item.GetItem();

	// now create a window which is added to this node
	TreeMultiItemWindow *wnd = new TreeMultiItemWindow(parent, name);
	parent->AddNode(wnd);

#if(CHECKBOXVIEW)
	// if checkbox view is desired, tag this item as a checkbox
	// and set the state as 'false'
	wnd->SetCheckbox(_checkboxView);
#endif

	// if style wants us to change background, set it to our background
	if(wflags & wxTMC_BG_ADJUST_ALL)
	{
		// go through all children of this window, and set the
		// background of it (recursively)
		SetWindowBackgroundColour(window, GetBackgroundColour(), wflags);
	}

	wnd->SetTopSpacing(info.GetTopSpacing());

#if(CHECKBOXVIEW)
	// make sure that the checkboxes are at least indented enough
	if(_checkboxView)
		wnd->SetFrontSpacing(info.GetFrontSpacing() + _checkWidth);
	else
#endif
		wnd->SetFrontSpacing(info.GetFrontSpacing());

	wnd->AssignWindow(window);

#if(CHECKBOXVIEW)
	// set the checkbox state after the window is assigned
	wnd->SetCheckboxState(info.GetDefaultCheckState());
#endif

	// when flags allow, tell this window item to expand
	wnd->SetHorizontalSpan((flags & wxTMC_SPAN_WIDTH) != 0);

	// if the window is not visible, set hide flag
	ShowTreeMultiWindow(wnd, wnd->IsVisible());

	// if any parent of this window is not collapsed then we
	// don't need to redraw this node nor recalculate
	RedrawFromNode(parent);

	return wxTreeMultiItem(wnd);
}

wxTreeMultiItem wxTreeMultiCtrl::AppendNode(const wxTreeMultiItem &item, const wxString &caption, const wxString &name)
{
	// add window only if this node (parent) is valid, and a node
	wxCHECK(item.IsOk(), wxTreeMultiItem(0));
	TreeMultiItemNode *parent = item.GetItem()->IsTreeMultiItemNode();

	// if parent is null, then try if the owner
	// is a root. This is also a valid parent
	if(!parent)
	{
		parent = item.GetItem()->IsTreeMultiItemRoot();
		wxCHECK(parent != 0, wxTreeMultiItem(0));
	}

	// now create a window which is added to this node

	TreeMultiItemNode *node = new TreeMultiItemNode(parent, caption, name);
	parent->AddNode(node);

#if(CHECKBOXVIEW)
	// if checkbox view is desired, tag this item as a checkbox
	// and set the state as 'false'
	node->SetCheckbox(_checkboxView);
	node->SetCheckboxState(0);
#endif

	// calculate the height and width
	int extX = 0, extY = 0;
	GetTextExtent(caption, &extX, &extY, 0, 0, &_captionFont);
	node->SetHeight(extY);
	node->SetWidth(extX);

	// if any parent of this node is not collapsed then we
	// don't need to redraw this node nor recalculate

	RedrawFromNode(parent);

	return wxTreeMultiItem(node);
}

bool wxTreeMultiCtrl::Delete(wxTreeMultiItem &item)
{
	bool redraw = true;
	wxCHECK(item.IsOk(), false);

	wxTreeMultiItem nullItem(0);

	// get parent, to delete item from
	TreeMultiItemNode *p = item.GetItem()->GetParent();

	// first check if it was visible, if so from the parent off
	// it needs redrawing
	redraw = item.GetItem()->IsVisible();
	if(p)
		p->DeleteNode(item.GetItem());
	else
		_root.DeleteNode(item.GetItem());

	item = nullItem;

	// do redraw when node was visible
	if(redraw)
		RedrawFromNode(p);

	return true;
}

void wxTreeMultiCtrl::DeleteChildren(const wxTreeMultiItem &item)
{
	if(item.IsNodeItem())
	{
		TreeMultiItemNode *n = (TreeMultiItemNode *)item.GetItem();
		n->Clear();
		RedrawFromNode(n);
	}
}

void wxTreeMultiCtrl::Expand(const wxTreeMultiItem &item, bool recursive)
{
	if(item.IsOk())
	{
        TreeMultiItemNode *n = item.GetItem()->IsTreeMultiItemNode();
		if(!n)
			n = item.GetItem()->GetParent();
        DoFold(n, true, recursive);
		RedrawFromNode(item.GetItem()->IsTreeMultiItemNode());
	}
}

void wxTreeMultiCtrl::Collapse(const wxTreeMultiItem &item, bool recursive)
{
	if(item.IsOk())
	{
        TreeMultiItemNode *n = item.GetItem()->IsTreeMultiItemNode();
		if(!n)
			n = item.GetItem()->GetParent();
		DoFold(n, false, recursive);
		RedrawFromNode(item.GetItem()->IsTreeMultiItemNode());
	}
}


void wxTreeMultiCtrl::ExpandNodes(bool recursive)
{
	// go through all children and call DoFold recursively
	for(int i = 0; i < _root.GetNodeCount(); i++)
		DoFold(_root.GetNode(i), true, recursive);
	RedrawFromNode(0);
}

void wxTreeMultiCtrl::CollapseNodes(bool recursive)
{
	// go through all children and call DoFold recursively
	for(int i = 0; i < _root.GetNodeCount(); i++)
		DoFold(_root.GetNode(i), false, recursive);
	RedrawFromNode(0);
}

void wxTreeMultiCtrl::CollapseAndReset(const wxTreeMultiItem &item)
{
	if(item.IsNodeItem())
	{
		 TreeMultiItemNode *n = (TreeMultiItemNode *)item.GetItem();

		 // delete all kids
		 n->Clear();
		 Collapse(item, false);
	}
}

void wxTreeMultiCtrl::DoFold(TreeMultiItemBase *item, bool expand, bool recursive)
{

	// go through all node objects on this level, and expand or
	// collapse them

	if(item == 0)
		return;

	// if this is a node, use it to go through all the subnodes (if needed)
	// if not, then just exit.

	TreeMultiItemNode *node = item->IsTreeMultiItemNode();
	if(node)
	{
		node->Fold(expand);

		// go recursive
		if(recursive)
		{
			TreeMultiItemNode *p;
			for(int i = 0; i < node->GetNodeCount(); i++)
			{
				// get node, and if a real node, then call fold
				p = node->GetNode(i)->IsTreeMultiItemNode();
				if(p)
					p->Fold(expand);

				// go recursive for every node
				DoFold(p, expand, recursive);
			}
		}
	}
}

void wxTreeMultiCtrl::Exclude(const wxTreeMultiItem &item)
{
	wxCHECK2(item.IsOk(), return);

	// exclude the item, and refresh
	// if already excluded, skip

	if(!item.GetItem()->IsExcluded())
	{
		item.GetItem()->SetExcluded(true);
		RedrawFromParentNode(item.GetItem());
	}
}

void wxTreeMultiCtrl::Include(const wxTreeMultiItem &item)
{
	wxCHECK2(item.IsOk(), return);

	// include the item, and refresh. If not
	// excluded, do nothing

	if(item.GetItem()->IsExcluded())
	{
		item.GetItem()->SetExcluded(false);
		RedrawFromParentNode(item.GetItem());
	}
}

wxTreeMultiItem wxTreeMultiCtrl::GetExcludedParent(const wxTreeMultiItem &item)
{
	wxCHECK(item.IsOk(), wxTreeMultiItem(0));

	// go find the parent (including this one) that
	// can be the excluded one

	TreeMultiItemNode *n = item.GetItem()->IsTreeMultiItemNode();
	if(n && n->IsExcluded())
		return wxTreeMultiItem(n);

	n = item.GetItem()->GetParent();
	while(n)
	{
		if(n->IsExcluded())
			return wxTreeMultiItem(n);
		else
			n = n->GetParent();
	}

	return wxTreeMultiItem(0);
}

void wxTreeMultiCtrl::OnSize(wxSizeEvent &event)
{
	RecalculateSpanSizes();
}

void wxTreeMultiCtrl::OnPaint(wxPaintEvent &event )
{
	wxPaintDC dc(this);
	PrepareDC(dc);

	dc.SetFont(_captionFont);

	// go recursive and draw the whole visible tree.
	for(int i = 0; i < _root.GetNodeCount(); i++)
		DrawNode(_root.GetNode(i), dc);
}

void wxTreeMultiCtrl::DrawNode(TreeMultiItemBase *b, wxDC &dc)
{
	// go through this item .. if it is a node, draw
	// the caption, else reposition the window.

	if(!b)
		return;

	// forget it if this node is not visible
	if(b->IsVisible())
	{
		int bmpOffsetX = b->GetX() - (_gutterWidth + _iconWidth);

#if(CHECKBOXVIEW)
		// now draw the checkbox if there is any, in the proper state
		if(b->GetCheckbox())
		{
			DrawCheckbox(b, dc);
 
			// adjust the bmpOffset because we also have a checkbox			
			bmpOffsetX -= _checkWidth;
		}
#endif

		if(b->IsTreeMultiItemNode())
		{
			// draw the node icon and the caption
			TreeMultiItemNode *n = (TreeMultiItemNode *)b;
			dc.DrawText(n->GetCaption(), n->GetX(), n->GetY());

			// draw the bitmap for the state
			if(n->IsExpanded())
				dc.DrawBitmap(*_expandBmp, bmpOffsetX, n->GetY() + _iconDeltaY, true);
			else
				dc.DrawBitmap(*_collBmp, bmpOffsetX, n->GetY() + _iconDeltaY, true);

			// now go through all the subnodes
			for(int i = 0; i < n->GetNodeCount(); i++)
				DrawNode(n->GetNode(i), dc);

		}
	}
}

#if(CHECKBOXVIEW)

void wxTreeMultiCtrl::DrawCheckbox(TreeMultiItemBase *b, wxDC &dc, bool convertScrolled)
{
	wxCHECK2(b, return);
	
	wxBitmap *bmp;
	int bmpOffsetX = b->GetX() - (_gutterWidth + _iconWidth);

	switch(b->GetCheckboxState())
	{
	case 0:
		bmp = _uncheckBmp;
		break;
	case 1:
		bmp = _checkBmp;
		break;
	default:
		bmp = _tristateBmp;
		break;
	}

	int x, xx, y, yy;

	if(b->IsTreeMultiItemWindow())
	{
		xx = x = bmpOffsetX - ((TreeMultiItemWindow *)b)->GetFrontSpacing() + _checkWidth;
		yy = y = b->GetY() + _checkDeltaY;
	}
	else
	{
		xx = x = bmpOffsetX;	
		yy = y = b->GetY() + _checkDeltaY;
	}
	
	if(convertScrolled)
		CalcScrolledPosition(x, y, &xx, &yy);
	
	dc.DrawBitmap(*bmp, xx, yy, true);
}

#endif // #if(CHECKBOXVIEW)

void wxTreeMultiCtrl::OnKey(wxKeyEvent &event)
{
	// check if we need to traverse to upper or lower
	// control in the list
	if(event.GetKeyCode() == WXK_TAB)
	{
		wxTreeMultiItem item = GetFocus();
		if(item.IsOk())
		{
			// traverse down direction
			if(!event.ShiftDown())
				item = FindNextVisibleWindowItem(item.GetItem());
			//else // traverse in up direction
			//	item = FindPreviousVisibleWindowItem(item);

			if(item.IsOk())
			{
				TreeMultiItemWindow *w = item.GetItem()->IsTreeMultiItemWindow();
				if(w)
				{
					wxWindow *wnd = w->GetWindow();
					wnd->SetFocus();
				}
			}
		}
	}
	else
		event.Skip();
}

void wxTreeMultiCtrl::OnMouseClick( wxMouseEvent &event )
{
	// react on double click and left mouse down
	if(event.LeftDown() || event.LeftDClick())
	{
		// get translation point
		wxPoint pt( event.GetPosition() );

		int x = 0, y = 0;
		CalcUnscrolledPosition( pt.x, pt.y, &x, &y );

		// go check if we clicked a treenode
		int flags;
		wxPoint p(x,y);
		wxTreeMultiItem id = HitTest(p, flags);

#if(CHECKBOXVIEW)
		if(flags == wxTMC_HITTEST_CHECKBOX)
		{
			// toggle the checkbox, and redraw
			if(id.IsOk())
			{
				TreeMultiItemBase *b = id.GetItem();
				b->SetCheckboxState((b->GetCheckboxState()+1) & 0x1);

				TreeMultiItemWindow *w = b->IsTreeMultiItemWindow();
				if(w)
				{
					// try to force a focus on the window. This could
					// be extended by searching for the first edit control
					// class but for now, just a focus is tried.
					w->GetWindow()->Enable(b->GetCheckboxState() == 1);
					w->GetWindow()->SetFocus();

					// draw the checkbox in the state needed	
					wxClientDC dc(this);				
					DrawCheckbox(b, dc, true);

					// TODO: determine if the upper parents should be 
					// tristated or not
					
					ScanTristateCheckstates(b);

				}
				else if(b->IsTreeMultiItemNode())
				{
					// descend to all the children and set the state of the parent
					SetRecursiveCheckState((TreeMultiItemNode *)b, b->GetCheckboxState() == 1);
					RedrawFromNode((TreeMultiItemNode *)b);
				}
			}
		}
		else
#endif // #if(CHECKBOXVIEW)
		{	
			// react on left mouse button, to fold and on
			// right for caption doubleclick
			int area = -1;

// adjust behaviour for Linux (single click = always fold)
#ifndef LINUX
			if(event.LeftDClick())
				area = wxTMC_HITTEST_CAPTION;
			else
				area = wxTMC_HITTEST_GUTTER;
#else
			area = flags;
#endif

			// we clicked in the gutter, which means if this is a
			// tree node, we can collapse or expand it.
			if(id.IsOk() && (flags == area))
			{
				// we have a valid item, if it is a node, then fold
				TreeMultiItemNode *n = id.GetItem()->IsTreeMultiItemNode();
				if(n)
					Fold(n, !n->IsExpanded());
			}
		}
	}
}

wxTreeMultiItem wxTreeMultiCtrl::HitTest(wxPoint &pt, int &flags)
{
	// scan all nodes to see which one matches
	TreeMultiItemBase *b = 0;
	for(int i = 0; i < _root.GetNodeCount() && !b; i++)
		b = FindNodeByPoint(_root.GetNode(i), pt, flags);

	if(!b)
	{
		// none found, reset
		flags = 0;
		return wxTreeMultiItem(0);
	}

	// return an item
	return wxTreeMultiItem(b);
}

TreeMultiItemBase *wxTreeMultiCtrl::FindNodeByPoint(TreeMultiItemBase *b, wxPoint &pt, int &area)
{
	wxCHECK(b, 0);

	// if this layer is not visible, return with nothing.
	if(!b->IsVisible())
		return 0;
	
	area = 0;

	// now see if our y is matching the mouse
	if(pt.y >= b->GetY() && pt.y < (b->GetY() + b->GetHeight()))
	{
#if(CHECKBOXVIEW)
		// if we are checkboxed, calculate the checkbox position
		if(b->GetCheckbox())
		{
			int extraSpacing = 0, extraWidth = 0;

			// now for a windows item, this is minus the gutter. For a normal node it is X minus checkbox
			if(b->IsTreeMultiItemWindow())
			{
			    extraWidth = _checkWidth;
				extraSpacing = ((TreeMultiItemWindow *)b)->GetFrontSpacing();
			}
			else
				extraSpacing = 4;

			if(pt.x > (b->GetX() - extraSpacing - _checkWidth) && pt.x < (b->GetX() - extraSpacing + extraWidth))
			{
				area = wxTMC_HITTEST_CHECKBOX;
				return b;
			}
		}
#endif

		// allrighty we have something, now where and what is it (look with x)
		if(pt.x < b->GetX())
			area = wxTMC_HITTEST_GUTTER;

		/** \todo Match only the real part of the caption, window (we assume x > GetX() which is the rest)
			HOWEVER the window probably doesn't propagate the click event back to the parent, so we might
			leave it like this so the use can click behind a window so it will be selected.
		*/
		else
		{
			// inside area, return proper flag
			if(b->IsTreeMultiItemNode())
				area = wxTMC_HITTEST_CAPTION;
			else
				area = wxTMC_HITTEST_WINDOW;
		}

		return b;
	}
	else
	{
		// not found, let's try our children if we have some
		TreeMultiItemNode *n = b->IsTreeMultiItemNode();
		if(n)
		{
			TreeMultiItemBase *bb = 0;
			for(int i = 0; i < n->GetNodeCount() && !bb; i++)
				bb = FindNodeByPoint(n->GetNode(i), pt, area);

			// keep returning result to caller
			return bb;
		}
	}

	return 0;
}

wxTreeMultiItem wxTreeMultiCtrl::GetFocus()
{
	wxWindow *wnd = wxWindow::FindFocus();

	// now find window that holds this item. if not
	// visible it cannot have focus (should not have)

	wxTreeMultiItem item = FindWindowNode(wnd);
	if(item.IsOk() && item.GetItem()->IsVisible())
		return item;

	return wxTreeMultiItem(0);
}

#if(CHECKBOXVIEW)

void wxTreeMultiCtrl::SetRecursiveCheckState(TreeMultiItemNode *n, bool check)
{
	int state = 0;
	if(check)
		state++;

	// go check all kids on this level
	for(int i = 0; i < n->GetNodeCount(); i++)
	{
		// check all the nodes, and go deeper 
		n->GetNode(i)->SetCheckboxState(state);
		if(n->GetNode(i)->IsTreeMultiItemNode())
			SetRecursiveCheckState((TreeMultiItemNode *)n->GetNode(i), check);
	}	
}

void wxTreeMultiCtrl::ScanTristateCheckstates(TreeMultiItemBase *b)
{
	// check from the parent on, all node entries and see if they are 
	// checked or cleared or scattered
	TreeMultiItemNode *p = b->GetParent();

	if(p && p->GetCheckbox())
	{
		bool foundcheck = false, foundclear = false;
		for(size_t i = 0; i < (size_t)p->GetNodeCount(); ++i)
		{
			// only evaluate when checkboxed
			if(p->GetNode(i)->IsTreeMultiItemWindow() && p->GetNode(i)->GetCheckbox())
			{
				// record instance of a cleared checkbox
				if(!p->GetNode(i)->GetCheckboxState())
					foundclear = true;					
				// record instance of checked checkbox
				if(p->GetNode(i)->GetCheckboxState() == 1)
					foundcheck = true;					
			}
		}

		// if we have both check and clear, go tristate
		// if all clear, clear parent and if all set, then set parent
		if(foundclear && !foundcheck)
			p->SetCheckboxState(0);
		else if(!foundclear && foundcheck)
			p->SetCheckboxState(1);
		else if(foundclear && foundcheck)
			p->SetCheckboxState(2);

		//wxClientDC dc;
		//DrawCheckbox(p, dc, false);
		RedrawFromNode(p);
	}
}

#endif // #if(CHECKBOXVIEW)

void wxTreeMultiCtrl::RedrawFromParentNode(TreeMultiItemBase *n)
{
	TreeMultiItemNode *p = 0;
	if(n)
		p = n->GetParent();

	RedrawFromNode(p);
}

void wxTreeMultiCtrl::RedrawFromNode(TreeMultiItemNode *n)
{	
	static int recalcMutex = 0;
	bool visible = true;
	
	if(recalcMutex > 0)
		return;
	
	recalcMutex ++;

	// when node is not visible or excluded 
	// then don't redraw.

	if(n)
		visible = n->IsVisible();
	
	if(visible)
	{		
		int h, h1,w, w1;
		GetVirtualSize(&w, &h);

		UpdateAllWindowVisibility();
		RecalculateNodePositions();
		RecalculateVirtualSize();

		// why is this needed? Because folding or collapsing can change
		// the state. When the virtual area gets smaller, we need to keep
		// the largest one and the other way atound. And since we do not
		// know here we are folding or collapsing, we remember the biggest
		GetVirtualSize(&w1, &h1);

		if(h1 > h)
			h = h1;
		if(w1 > w)
			w = w1;

		// only refresh the part from x,y down
		if(n)
		{
			int x, y;
			CalcScrolledPosition(n->GetX(), n->GetY(), &x, &y); 
			if(h - y > 0)
			{
				wxRect rect(0, y, w, h - y);
				RefreshRect(rect);
			}
			else
				Refresh();
		}
		else
			Refresh();	// do a full refresh
	}

	recalcMutex --;
}

void wxTreeMultiCtrl::RecalculateNodePositions()
{
	int currentY = _spacingY;
	// go recursive on every node, and store the information in the node

	for(int i = 0; i < _root.GetNodeCount(); i++)
        currentY += CalculateNodeDimensions(_root.GetNode(i), currentY, 0);
}

int wxTreeMultiCtrl::CalculateNodeDimensions(TreeMultiItemBase *b, int currentY, int level)
{
	int gutter = (_gutterWidth * 2) + _iconWidth;
	int y = 0, topSpacing = 0; //, cbgutter = 0;

	// return same if no proper object
	wxCHECK(b, 0);

#if(CHECKBOXVIEW)
	if(b->GetCheckbox())
		gutter += _checkWidth;
#endif

	// if we are not visible, skip recalculation and descending
	if(b->IsVisible())
	{
		b->SetY(currentY);
		
		// if level is 0, calculate with front gutter, else without
		y = currentY + b->GetHeight();
		if(b->IsTreeMultiItemNode())
		{
			TreeMultiItemNode *n = (TreeMultiItemNode *)b;

			if(level == 0)
				b->SetX(gutter);
			else
				b->SetX(gutter + (level * (_gutterWidth + _iconWidth)));
			
			// now do children of this node

			for(int i = 0; i < n->GetNodeCount(); i++)
				y += CalculateNodeDimensions(n->GetNode(i), y + _spacingY, level+1);
		}
		else if(b->IsTreeMultiItemWindow())
		{
			TreeMultiItemWindow *w = (TreeMultiItemWindow *)b;

			if(level == 0)
				b->SetX(gutter + w->GetFrontSpacing());
			else
				b->SetX(_gutterWidth + (level * (_gutterWidth + _iconWidth)) + w->GetFrontSpacing());

			topSpacing = w->GetTopSpacing();

			// reposition the window

			wxWindow *wnd = w->GetWindow();
			if(wnd)
			{
				int x = 0, y = 0;
				CalcScrolledPosition(w->GetX(), w->GetY(), &x, &y);
				wnd->SetSize(x, y, w->GetWidth(), w->GetHeight());
			}
		}

		if(y > 0)
			return (y - currentY) + _spacingY + topSpacing;	// return delta
		else
			return 0;
	}

	return 0;	// not visible, thus we skip calculations
}

void wxTreeMultiCtrl::RecalculateSpanSizes()
{
	for(int i = 0; i < _root.GetNodeCount(); i++)
        CalculateNodeSpanning(_root.GetNode(i));
}

void wxTreeMultiCtrl::CalculateNodeSpanning(TreeMultiItemBase *b)
{
	// return same if no proper object
	wxCHECK2(b, return);

	if(b->IsTreeMultiItemNode())
	{
		TreeMultiItemNode *n = (TreeMultiItemNode *)b;
		
		// now do children of this node

		for(int i = 0; i < n->GetNodeCount(); i++)
			CalculateNodeSpanning(n->GetNode(i));
	}
	else if(b->IsTreeMultiItemWindow())
	{
		TreeMultiItemWindow *w = (TreeMultiItemWindow *)b;
		wxWindow *wnd = w->GetWindow();
		if(wnd)
		{			
			// if the window is spanning, we adjust the width to the max width of the control
			if(w->GetHorizontalSpan())
			{
				wxSize tmcsize = GetClientSize();
				int maxwidth = tmcsize.GetWidth() - w->GetX() - 8; // extract 3 for border
				
				wxSizer *sz = wnd->GetSizer();
				if(sz)
				{
					if(maxwidth < sz->GetMinSize().GetWidth())
						maxwidth = sz->GetMinSize().GetWidth();
				}
				
				// prevent a size of 0
				if(maxwidth < 1)
					maxwidth = 1;
				
				// set the size
				w->SetWidth(maxwidth);
				wnd->SetSize(w->GetWidth(), w->GetHeight());

				// layout by sizer (not sure if this is needed)
				if(wnd->GetSizer())
					wnd->GetSizer()->Layout();
			}
		}
	}
}

void wxTreeMultiCtrl::RecalculateVirtualSize()
{
	// go through all the nodes, and store the largest x and largest y

	int x = 0, y = 0;
	RecalculateVirtualSizeFromNode(&_root, x, y);

	// now adjust virtual size
	SetVirtualSize(x, y);
	AdjustScrollbars(x, y);
}

void wxTreeMultiCtrl::AdjustScrollbars(int x, int y)
{
	// adjust scrollbars
	// courtesy of treectrlg.cpp

	y += WXTMC_PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
    x += WXTMC_PIXELS_PER_UNIT+2; // one more scrollbar unit + 2 pixels
    int x_pos = GetScrollPos( wxHORIZONTAL );
    int y_pos = GetScrollPos( wxVERTICAL );
    SetScrollbars( WXTMC_PIXELS_PER_UNIT, WXTMC_PIXELS_PER_UNIT, x/WXTMC_PIXELS_PER_UNIT, 
		           y/WXTMC_PIXELS_PER_UNIT, x_pos, y_pos, true );
}

void wxTreeMultiCtrl::RecalculateVirtualSizeFromNode(const TreeMultiItemNode *node, int &x, int &y)
{
	if(node->IsExcluded())
		return;
	
	// if calulate this node's dimensions
	if(x < (node->GetWidth() + node->GetX()))
		x = node->GetWidth() + node->GetX();

	y = node->GetY() + node->GetHeight();

	// if this node is collapsed, no subnodes are visible, else
	// go through all subnodes as well, node needs to be included as well
	if(node->IsExpanded())
	{
		TreeMultiItemBase *b;
		for(int i = 0; i < node->GetNodeCount(); i++)
		{
			b = node->GetNode(i);
			
			// calculate x and y
			if(x < (b->GetWidth() + b->GetX()))
				x = b->GetWidth() + b->GetX();

			y = b->GetY() + b->GetHeight();

			if(b->IsTreeMultiItemNode())
				RecalculateVirtualSizeFromNode((TreeMultiItemNode *)b, x, y);
		}
	}	
}

void wxTreeMultiCtrl::SetWindowBackgroundColour(wxWindow *wnd, const wxColour &col, int flags)
{
	if(wnd)
	{
		// if we cannot change a button, make sure all button
		// classes are not changed

		wxButton *btn = wxDynamicCast(wnd, wxButton);
		if(!btn || ((flags & wxTMC_BG_ADJUST_BTN) != 0))
			wnd->SetBackgroundColour(col);

		// get every window, and make the background equal to the given one
		wxWindowListNode *node = wnd->GetChildren().GetFirst();
		while (node)
		{
			SetWindowBackgroundColour(node->GetData(), col, flags);
			node = node->GetNext();
		}
	}
}

void wxTreeMultiCtrl::ShowTreeMultiWindow(TreeMultiItemWindow *window, bool show)
{
	// show or hide window
	if(window && window->GetWindow())
		window->GetWindow()->Show(show);
}

void wxTreeMultiCtrl::UpdateAllWindowVisibility()
{
	// all roots are visible, but what lies beneath ... who knows
	for(int i = 0; i < _root.GetNodeCount(); i++)
		UpdateTreeMultiWindowVisibility(_root.GetNode(i), true);
}

void wxTreeMultiCtrl::UpdateTreeMultiWindowVisibility(TreeMultiItemBase *b, bool show)
{
	if(b)
	{
		// this is done for performance issues. IsVisible can go all
		// the way up the tree to check. However if show is already
		// false, there is no need to check (some higher one is collapsed)
		bool showMe = show;
		
		if(showMe)
			showMe = b->IsVisible();

		if(b->IsTreeMultiItemWindow())
		{
			// if this level must be hidden, hide
			ShowTreeMultiWindow((TreeMultiItemWindow*)b, showMe);
		}
		else if(b->IsTreeMultiItemNode())
		{
			TreeMultiItemNode *node = (TreeMultiItemNode *)b;

			// if hidden, descend and hide all windows
			for(int i = 0; i < node->GetNodeCount(); i++)
				UpdateTreeMultiWindowVisibility(node->GetNode(i), showMe);
		}
	}
}

wxTreeMultiItem wxTreeMultiCtrl::FindItem(const wxTreeMultiItem &item, const wxString &name, bool ignoreCase, bool skipFirst)
{
	if(item.IsOk())
	{
		TreeMultiItemBase *b = item.GetItem();

		// check this item first (or not)

		if(!skipFirst)
		{
			if(b->GetName().IsSameAs(name, !ignoreCase))
				return wxTreeMultiItem(b);
		}

		if(b->IsTreeMultiItemNode())
		{
			// now check whether we are a node, then go check children

			TreeMultiItemNode *n = (TreeMultiItemNode *)b;
			wxTreeMultiItem result(0);
			for(int i = 0; i < n->GetNodeCount() && !result.IsOk(); i++)
				result = FindItem(wxTreeMultiItem(n->GetNode(i)), name, ignoreCase, false);

			return result;
		}
	}

	return wxTreeMultiItem(0);
}

wxTreeMultiItem wxTreeMultiCtrl::FindWindowNode(wxWindow *wnd, TreeMultiItemNode *n)
{
	wxCHECK(wnd, wxTreeMultiItem(0));

	// take root node if not assigned one

	if(!n)
		n = (TreeMultiItemNode *)&_root;

	// check on this level for the wxWindow pointer

	TreeMultiItemWindow *w;
	wxTreeMultiItem result(0);
	for(int i = 0; i < n->GetNodeCount() && !result.IsOk(); i++)
	{
		// if window node
		w = n->GetNode(i)->IsTreeMultiItemWindow();
		if(w && w->GetWindow() == wnd)
			return wxTreeMultiItem(n);

		// if node, go deeper
		if(n->GetNode(i)->IsTreeMultiItemNode())
			result = FindWindowNode(wnd, (TreeMultiItemNode*)n->GetNode(i));
	}

	return result;
}

TreeMultiItemWindow *wxTreeMultiCtrl::FindNextVisibleWindowItem(TreeMultiItemBase *b, int index)
{
	wxCHECK(b, 0);

	// check on this level, go deeper with every node we got. When a node is not
	// visible anymore, skip the node.

	TreeMultiItemWindow *value = 0;
	if(b->IsVisible())
	{		
		// if we are already searching on a node with an index
					
		TreeMultiItemBase *bn = 0;
		TreeMultiItemNode *n = b->IsTreeMultiItemNode();
		if(n)
		{
			for(int i = index + 1; i < n->GetNodeCount() && !value; i++)
			{
				bn = n->GetNode(i);				
				value = bn->IsTreeMultiItemWindow();
				
				// assume a node, root when not a a window
				if(!value)
					value = FindNextVisibleWindowItem(bn, -1);
			}
		
		}
		else
		{
			if(b->IsTreeMultiItemWindow())
			{
				// get parent first, and locate child as ptr
				TreeMultiItemNode *p = b->GetParent();
				wxCHECK(p, 0);
				
				// go scan the parent from the given index, if
				// the index is valid else there is no child with that index

				int idx = p->Index(b);
				wxCHECK(idx >= 0, 0);

				value = FindNextVisibleWindowItem(p, idx);
			}
		}
	}

	return value;

}

bool wxTreeMultiCtrl::GetBooleanValue(int wndId)
{
	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK(wnd, false);

	// try a radio button
	wxRadioButton *b = wxDynamicCast(wnd, wxRadioButton);
	if(b)
		return b->GetValue();

	// try a check box
	wxCheckBox *c = wxDynamicCast(wnd, wxCheckBox);
	if(c)
		return c->GetValue();

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	    which can be overridden to retrieve the boolean value. It will also be passed
	    the pointer to the window, so the derived class can figure out how to get a boolean
	    value.
	*/

	// generate assert or just return with false
	wxCHECK(0, false);
}

void wxTreeMultiCtrl::SetBooleanValue(int wndId, bool value)
{
	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK2(wnd, return);

	// try a radio button
	wxRadioButton *b = wxDynamicCast(wnd, wxRadioButton);
	if(b)
	{
		b->SetValue(value);
		return;
	}

	// try a check box
	wxCheckBox *c = wxDynamicCast(wnd, wxCheckBox);
	if(c)
	{
		c->SetValue(value);
		return;
	}

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	            which can be overridden to retrieve the boolean value. It will also be passed
	            the pointer to the window, so the derived class can figure out how to get a boolean
	            value.
	*/

	// generate assert
	wxCHECK2(0, return);
}

void wxTreeMultiCtrl::SetTextValue(int wndId, const wxString &value)
{
	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK2(wnd, return);

	// try a radio button
	wxTextCtrl *t = wxDynamicCast(wnd, wxTextCtrl);
	if(t)
	{
		t->SetValue(value);
		return;
	}

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	            which can be overridden to retrieve the boolean value. It will also be passed
	            the pointer to the window, so the derived class can figure out how to get a boolean
	            value.
	*/

	// generate assert
	wxCHECK2(0, return);
}

wxString wxTreeMultiCtrl::GetTextValue(int wndId)
{
	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK(wnd, wxEmptyString);

	// try a radio button
	wxTextCtrl *t = wxDynamicCast(wnd, wxTextCtrl);
	if(t)
		return t->GetValue();

	// try a choice box
	wxChoice *c1 = wxDynamicCast(wnd, wxChoice);
	if(c1)
		return c1->GetStringSelection();

	// try a combo box
	wxComboBox *c2 = wxDynamicCast(wnd, wxComboBox);
	if(c2)
		return c2->GetStringSelection();

	// try a listbox
	wxListBox *l = wxDynamicCast(wnd, wxListBox);
	if(l)
		return l->GetStringSelection();

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	    which can be overridden to retrieve the boolean value. It will also be passed
	    the pointer to the window, so the derived class can figure out how to get a boolean
	    value.
	*/

	// generate assert or just return with string
	wxCHECK(0, wxEmptyString);
}

int wxTreeMultiCtrl::GetSelectionValue(int wndId)
{
	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK(wnd, -1);

	// try a choice box
	wxChoice *c1 = wxDynamicCast(wnd, wxChoice);
	if(c1)
		return c1->GetSelection();

	// try a combo box
	wxComboBox *c2 = wxDynamicCast(wnd, wxComboBox);
	if(c2)
		return c2->GetSelection();

	// try a listbox
	wxListBox *l = wxDynamicCast(wnd, wxListBox);
	if(l)
		return l->GetSelection();

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	    which can be overridden to retrieve the boolean value. It will also be passed
	    the pointer to the window, so the derived class can figure out how to get a boolean
	    value.
	*/

	// generate assert or just return with string
	wxCHECK(0, -1);
}

void wxTreeMultiCtrl::GetSelectionValues(int wndId, wxArrayInt &sels)
{
	sels.Clear();

	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK2(wnd, return);

	// try a listbox
	wxListBox *l = wxDynamicCast(wnd, wxListBox);
	if(l)
	{
		l->GetSelections(sels);
		return;
	}

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	    which can be overridden to retrieve the boolean value. It will also be passed
	    the pointer to the window, so the derived class can figure out how to get a boolean
	    value.
	*/

	// generate assert or just return with string
	wxCHECK2(0, return);
}

void wxTreeMultiCtrl::SetSelectionValue(int wndId, int sel)
{
	wxWindow *wnd = wxWindow::FindWindow(wndId);
	wxCHECK2(wnd, return);

	// try a choice box
	wxChoice *c1 = wxDynamicCast(wnd, wxChoice);
	if(c1)
	{
		c1->SetSelection(sel);
		return;
	}

	// try a combo box
	wxComboBox *c2 = wxDynamicCast(wnd, wxComboBox);
	if(c2)
	{
		c2->SetSelection(sel);
		return;
	}

	// try a listbox
	wxListBox *l = wxDynamicCast(wnd, wxListBox);
	if(l)
	{
		l->SetSelection(sel);
		return;
	}

	/** \todo For custom controls we should put something in wxMultiTreeItemData class
	    which can be overridden to retrieve the boolean value. It will also be passed
	    the pointer to the window, so the derived class can figure out how to get a boolean
	    value.
	*/

	// generate assert or just return with string
	wxCHECK2(0, return);
}



wxTreeMultiItem wxTreeMultiCtrl::GetFirstChild(const wxTreeMultiItem &item, int &cookie) const
{
	if(item.IsNodeItem())
	{
		TreeMultiItemNode *n = (TreeMultiItemNode *)item.GetItem();

		if(n->GetNodeCount() > 0)
		{
			cookie = 0;
			return wxTreeMultiItem(n->GetNode(0));
		}
	}

	// no children or no valid node
	cookie = -1;
	return wxTreeMultiItem(0);
}

wxTreeMultiItem wxTreeMultiCtrl::GetNextChild(const wxTreeMultiItem &item, int &cookie) const
{
	if(item.IsNodeItem())
	{
		TreeMultiItemNode *n = (TreeMultiItemNode *)item.GetItem();

		if(cookie >= 0 && cookie < (n->GetNodeCount()-1))
		{
			// increment cookie, return node
			cookie ++;
			return wxTreeMultiItem(n->GetNode(cookie));
		}
	}

	// end of query, or no valid node
	cookie = -1;
	return wxTreeMultiItem(0);
}

wxTreeMultiItem wxTreeMultiCtrl::GetLastChild(const wxTreeMultiItem &item) const
{
	if(item.IsNodeItem())
	{
		TreeMultiItemNode *n = (TreeMultiItemNode *)item.GetItem();

		if(n->GetNodeCount() > 0)
			return wxTreeMultiItem(n->GetNode(n->GetNodeCount()-1));
	}

	return wxTreeMultiItem(0);
}

// WDR: handler implementations for wxTreeMultiCtrl

