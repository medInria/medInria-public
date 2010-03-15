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
#include <wx/styled_notebook.h>
#include <wx/arrimpl.cpp>

//----------------------------------------------------------------
// utils function
//----------------------------------------------------------------
static wxColor LightColour(const wxColour& color, int percent)
{
	int rd, gd, bd, high = 0;
	wxColor end_color = wxT("WHITE");
	rd = end_color.Red() - color.Red();
	gd = end_color.Green() - color.Green();
	bd = end_color.Blue() - color.Blue();
	high = 100;

	// We take the percent way of the color from color --> white
	int i = percent;
	int r = color.Red() +  ((i*rd*100)/high)/100;
	int g = color.Green() + ((i*gd*100)/high)/100;
	int b = color.Blue() + ((i*bd*100)/high)/100;
	return wxColor(r, g, b);
}

static void PaintStraightGradientBox(wxDC& dc, const wxRect& rect, const wxColour& startColor, const wxColour& endColor, bool  vertical = true)
{
	int rd, gd, bd, high = 0;
	rd = endColor.Red() - startColor.Red();
	gd = endColor.Green() - startColor.Green();
	bd = endColor.Blue() - startColor.Blue();

	/// Save the current pen and brush
	wxPen savedPen = dc.GetPen();
	wxBrush savedBrush = dc.GetBrush();

	if ( vertical )
		high = rect.GetHeight()-1;
	else
		high = rect.GetWidth()-1;

	if( high < 1 )
		return;

	for (int i = 0; i <= high; ++i)
	{
		int r = startColor.Red() +  ((i*rd*100)/high)/100;
		int g = startColor.Green() + ((i*gd*100)/high)/100;
		int b = startColor.Blue() + ((i*bd*100)/high)/100;

		wxPen p(wxColor(r, g, b));
		dc.SetPen(p);

		if ( vertical )
			dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
		else
			dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
	}

	/// Restore the pen and brush
	dc.SetPen( savedPen );
	dc.SetBrush( savedBrush );
}

static wxColor RandomColor() 
{
	int r, g, b;
	r = rand() % 256; // Random value betweem 0-255
	g = rand() % 256; // Random value betweem 0-255
	b = rand() % 256; // Random value betweem 0-255
	return wxColor(r, g, b);
}

//--------------------------------------------------------------------
// styled notebook - a notebook with look n feel of Visual Studio 2005
//--------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxStyledTabsContainer, wxPageContainerBase)
EVT_PAINT(wxStyledTabsContainer::OnPaint)
END_EVENT_TABLE()

WX_DEFINE_OBJARRAY(wxRectArray);

wxStyledTabsContainer::wxStyledTabsContainer(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
: wxPageContainerBase(parent, id, pos, size, style) 
, m_factor(1)
{
	/// Override colors 
	m_colorTo   = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 0); 
	m_colorFrom = LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 60);
}

void wxStyledTabsContainer::NumberTabsCanFit(wxDC& dc, wxRectArray& vTabInfo)
{
	int height, width, pom, tabHeight, clientWidth;
	
	wxRect rect = GetClientRect();
	clientWidth = rect.width;

	/// Empty results
	vTabInfo.Empty();

	/// We take the maxmimum font size, this is 
	/// achieved by setting the font to be bold
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	dc.SetFont(font);

	wxString stam = wxT("Tp");	// Temp data to get the text height;
	dc.GetTextExtent(stam, &width, &height);

	tabHeight = height + wxFNB_HEIGHT_SPACER; // We use 8 pixels
	// The drawing starts from posx
	int posx = ((wxFlatNotebookBase *)m_pParent)->GetPadding();
	
	for(int i=m_nFrom; i<(int)m_pagesInfoVec.GetCount(); i++)
	{
		dc.GetTextExtent(GetPageText(i), &width, &pom);
		
		// Set a minimum size to a tab
		if(width < 20)
			width = 20;

		int tabWidth = ((wxFlatNotebookBase *)m_pParent)->GetPadding() * 2 + width;
		
		/// Add the image width if it exist
		if( TabHasImage(i) )
			tabWidth += (16 + ((wxFlatNotebookBase*)m_pParent)->GetPadding());

		int vc8glitch = tabHeight + wxFNB_HEIGHT_SPACER;
		if(posx + tabWidth + vc8glitch + GetButtonsAreaLength() >= clientWidth)
			break;

		/// Add a result to the returned vector
		wxRect tabRect(posx, VERTICAL_BORDER_PADDING, tabWidth , tabHeight);
		vTabInfo.Add(tabRect);

		/// Advance posx
		posx += tabWidth + wxFNB_HEIGHT_SPACER;
	}
}

int wxStyledTabsContainer::GetNumTabsCanScrollLeft()
{
	int i;

	// Reserved area for the buttons (<>x)
	wxRect rect = GetClientRect();
	int clientWidth = rect.width;
	int posx = ((wxFlatNotebookBase *)m_pParent)->GetPadding(), numTabs = 0, pom = 0, width, shapePoints,
		height, tabHeight, tabWidth;

	wxClientDC dc(this);

	// Incase we have error prevent crash
	if(m_nFrom < 0)
		return 0;

	long style = GetParent()->GetWindowStyleFlag();
	for(i=m_nFrom; i>=0; i--)
	{
		wxFont boldFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		boldFont.SetWeight(wxFONTWEIGHT_BOLD);
		dc.SetFont(boldFont);

		wxString stam = wxT("Tp");	// Temp data to get the text height;
		dc.GetTextExtent(stam, &width, &height);

		tabHeight = height + wxFNB_HEIGHT_SPACER; // We use 6 pixels as padding
		if(style & wxFNB_VC71)
			tabHeight = HasFlag( wxFNB_BOTTOM ) ? tabHeight - 4 :  tabHeight;
		else if(style & wxFNB_FANCY_TABS)
			tabHeight = HasFlag( wxFNB_BOTTOM ) ? tabHeight - 3 :  tabHeight;

		dc.GetTextExtent(GetPageText(i), &width, &pom);
		if(style != wxFNB_VC71)
			shapePoints = (int)(tabHeight*tan((double)m_pagesInfoVec[i].GetTabAngle()/180.0*M_PI));
		else
			shapePoints = 0;

		tabWidth = ((wxFlatNotebookBase *)m_pParent)->GetPadding() * 2 + width;
		if(!(style & wxFNB_VC71))
			// Default style
			tabWidth += 2 * shapePoints;

		// For VC71 style, we only add the icon size (16 pixels)
		if( TabHasImage(i) )
		{
			if( !IsDefaultTabs() )
				tabWidth += (16 + ((wxFlatNotebookBase*)m_pParent)->GetPadding());
			else
				// Default style
				tabWidth += (16 + ((wxFlatNotebookBase*)m_pParent)->GetPadding()) + shapePoints / 2;
		}

		int vc8glitch = (style & wxFNB_VC8) ? tabHeight + wxFNB_HEIGHT_SPACER : 0;
		if(posx + tabWidth + vc8glitch + GetButtonsAreaLength() >= clientWidth)
			break;

		numTabs++;
		posx += tabWidth;
	}
	return numTabs;
}

bool wxStyledTabsContainer::CanDrawXOnTab()
{
	long style = GetParent()->GetWindowStyleFlag();
	bool isVC8 = style & wxFNB_VC8 ? true : false;
	return !isVC8;
}

bool wxStyledTabsContainer::IsDefaultTabs()
{
	long style = GetParent()->GetWindowStyleFlag();
	bool res = (style & wxFNB_VC71) || (style & wxFNB_FANCY_TABS) || (style & wxFNB_VC8);
	return !res;
}

int wxStyledTabsContainer::HitTest(const wxPoint& pt, wxPageInfo& pageInfo, int &tabIdx)
{
	wxRect rect = GetClientRect();
	int btnLeftPos = GetLeftButtonPos();
	int btnRightPos = GetRightButtonPos();
	int btnXPos = GetXPos();
	long style = GetParent()->GetWindowStyleFlag();
	tabIdx = -1;
	if(m_pagesInfoVec.IsEmpty())
	{
		return wxFNB_NOWHERE;
	}

	rect = wxRect(btnXPos, 8, 16, 16);
	if(rect.Contains(pt))
	{
		return (style & wxFNB_NO_X_BUTTON) ? wxFNB_NOWHERE : wxFNB_X;
	}

	rect = wxRect(btnRightPos, 8, 16, 16);
	if(rect.Contains(pt))
	{
		return (style & wxFNB_NO_NAV_BUTTONS) ? wxFNB_NOWHERE : wxFNB_RIGHT_ARROW;
	}
	rect = wxRect(btnLeftPos, 8, 16, 16);
	if(rect.Contains(pt))
	{
		return (style & wxFNB_NO_NAV_BUTTONS) ? wxFNB_NOWHERE : wxFNB_LEFT_ARROW;
	}

	// Test whether a left click was made on a tab
	bool bFoundMatch = false;
	for(size_t cur=m_nFrom; cur<m_pagesInfoVec.GetCount(); cur++)
	{
		wxPageInfo pgInfo = m_pagesInfoVec[cur];
		if(pgInfo.GetPosition() == wxPoint(-1, -1))
			continue;
		if(style & wxFNB_VC8)
		{
			if(m_pagesInfoVec[cur].GetRegion().Contains(pt) == wxInRegion)
			{
				if(bFoundMatch || (int)cur == GetSelection())
				{
					pageInfo = pgInfo;
					tabIdx = (int)cur;
					return wxFNB_TAB;
				}
				pageInfo = pgInfo;
				tabIdx = (int)cur;
				bFoundMatch = true;
			}
		}
		else
		{
			if(style & wxFNB_X_ON_TAB && (int)cur == GetSelection())
			{
				// 'x' button exists on a tab
				if(m_pagesInfoVec[cur].GetXRect().Contains(pt))
				{
					pageInfo = pgInfo;
					tabIdx = (int)cur;
					return wxFNB_TAB_X;
				}
			}

			wxRect tabRect = wxRect(pgInfo.GetPosition().x, pgInfo.GetPosition().y, 
				pgInfo.GetSize().x, pgInfo.GetSize().y);
			if(tabRect.Contains(pt))
			{
				// We have a match
				// wxMessageBox(pgInfo.m_strCaption);
				pageInfo = pgInfo;
				tabIdx = (int)cur;
				return wxFNB_TAB;
			}
		}
	}

	if(bFoundMatch)
		return wxFNB_TAB;

	// Default
	return wxFNB_NOWHERE;
}

void wxStyledTabsContainer::OnPaint(wxPaintEvent& event)
{
	if( !HasFlag(wxFNB_VC8) )
	{
		wxPageContainerBase::OnPaint(event);
		return;
	}

	/// Visual studio 8 style
	wxBufferedPaintDC dc(this);

#ifndef __WXMAC__ 
	// Works well on MSW & GTK, however this lines should be skipped on MAC
	if(m_pagesInfoVec.empty() || m_nFrom >= (int)m_pagesInfoVec.GetCount())
	{
		Hide();
		event.Skip();
		return;
	}
#endif

	// Get the text hight
	int height, width, tabHeight;

	// Set the font for measuring the tab height
	wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont boldFont = normalFont;
	boldFont.SetWeight(wxFONTWEIGHT_BOLD);

#ifdef __WXGTK__
	dc.SetFont( boldFont );
#endif

	wxString stam = wxT("Tp");	// Temp data to get the text height;
	dc.GetTextExtent(stam, &width, &height);

	tabHeight = height + wxFNB_HEIGHT_SPACER; // We use 8 pixels as padding

	// Calculate the number of rows required for drawing the tabs
	wxRect rect = GetClientRect();

	// Set the maximum client size
	SetSizeHints(wxSize(GetButtonsAreaLength(), tabHeight));
	wxPen borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));

	/// Create brushes
	wxBrush backBrush;
	backBrush = wxBrush(m_tabAreaColor);
	wxBrush noselBrush = wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	wxBrush selBrush = wxBrush(m_activeTabColor);
	wxSize size = GetSize();

	// Background
	dc.SetTextBackground(GetBackgroundColour());
	dc.SetTextForeground(m_activeTextColor);
	
	// If border style is set, set the pen to be border pen
	if( HasFlag(wxFNB_TABS_BORDER_SIMPLE) )
		dc.SetPen(borderPen);
	else
		dc.SetPen(*wxTRANSPARENT_PEN);

	int lightFactor = HasFlag(wxFNB_BACKGROUND_GRADIENT) ? 70 : 0;
	/// For VC8 style, we color the tab area in gradient coloring
	PaintStraightGradientBox(dc, GetClientRect(), m_tabAreaColor, LightColour(m_tabAreaColor, lightFactor));

	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(0, 0, size.x, size.y);

	// Take 3 bitmaps for the background for the buttons
	{
		wxMemoryDC mem_dc;
		//wxRect rect;
		
		//---------------------------------------
		// X button
		//---------------------------------------
		rect = wxRect(GetXPos(), 6, 16, 14);
		mem_dc.SelectObject(m_xBgBmp);
		mem_dc.Blit(0, 0, rect.width, rect.height, &dc, rect.x, rect.y);
		mem_dc.SelectObject(wxNullBitmap);

		//---------------------------------------
		// Right button
		//---------------------------------------
		rect = wxRect(GetRightButtonPos(), 6, 16, 14);
		mem_dc.SelectObject(m_rightBgBmp);
		mem_dc.Blit(0, 0, rect.width, rect.height, &dc, rect.x, rect.y);
		mem_dc.SelectObject(wxNullBitmap);

		//---------------------------------------
		// Left button
		//---------------------------------------
		rect = wxRect(GetLeftButtonPos(), 6, 16, 14);
		mem_dc.SelectObject(m_leftBgBmp);
		mem_dc.Blit(0, 0, rect.width, rect.height, &dc, rect.x, rect.y);
		mem_dc.SelectObject(wxNullBitmap);
	}

	// We always draw the bottom/upper line of the tabs
	// regradless the style
	dc.SetPen(borderPen);
	DrawTabsLine(dc);

	// Restore the pen
	dc.SetPen(borderPen);

	// Draw labels
	int pom;
	dc.SetFont(boldFont);
	int posx;
	int i = 0, cur = 0;
	int activeTabPosx = 0;

	// Update all the tabs from 0 to 'm_nFrom' to be non visible
	for(i=0; i<m_nFrom; i++)
	{
		m_pagesInfoVec[i].SetPosition(wxPoint(-1, -1));
		m_pagesInfoVec[i].GetRegion().Clear();
	}

	// Draw the visible tabs, in VC8 style, we draw them from right to left
	wxRectArray vTabsInfo;
	NumberTabsCanFit(dc, vTabsInfo);
	
	for(cur=(int)vTabsInfo.GetCount() - 1; cur>=0; cur--) 
	{
		/// 'i' points to the index of the currently drawn tab
		/// in m_pagesInfoVec vector
		i = m_nFrom + cur;
		dc.SetPen(borderPen);
		dc.SetBrush((i==GetSelection()) ? selBrush : noselBrush);

		// Calculate the text length using the bold font, so when selecting a tab
		// its width will not change
		dc.SetFont(boldFont);
		dc.GetTextExtent(GetPageText(i), &width, &pom);

		// Now set the font to the correct font
		dc.SetFont((i==GetSelection()) ? boldFont : normalFont);

		// Set a minimum size to a tab
		if(width < 20)
			width = 20;

		// Add the padding to the tab width
		// Tab width:
		// +-----------------------------------------------------------+
		// | PADDING | IMG | IMG_PADDING | TEXT | PADDING | x |PADDING |
		// +-----------------------------------------------------------+

		int tabWidth = ((wxFlatNotebookBase *)m_pParent)->GetPadding() * 2 + width;
		int imageYCoord = HasFlag(wxFNB_BOTTOM) ? 6 : 8;

		if( TabHasImage(i) )
			tabWidth += (16 + ((wxFlatNotebookBase*)m_pParent)->GetPadding());

		posx = vTabsInfo[cur].x;

		// By default we clean the tab region
		// incase we use the VC8 style which requires
		// the region, it will be filled by the function
		// drawVc8Tab
		m_pagesInfoVec[i].GetRegion().Clear();
		
		// Clean the 'x' buttn on the tab 
		// 'Clean' rectanlge is a rectangle with width or height
		// with values lower than or equal to 0
		m_pagesInfoVec[i].GetXRect().SetSize(wxSize(-1, -1));

		// Draw the tab
		// Incase we are drawing the active tab
		// we need to redraw so it will appear on top
		// of all other tabs
		if(i == GetSelection())
		{
			activeTabPosx = posx;
		}
		else
		{
			DrawVC8Tab(dc, posx, i, tabWidth, tabHeight);
		}

		// Text drawing offset from the left border of the 
		// rectangle
		int textOffset;
	
		// The width of the images are 16 pixels
		int vc8ShapeLen = tabHeight;
		
		if( TabHasImage(i) )
			textOffset = ((wxFlatNotebookBase *)m_pParent)->GetPadding() * 2 + 16 + vc8ShapeLen; 
		else
			textOffset = ((wxFlatNotebookBase *)m_pParent)->GetPadding() + vc8ShapeLen;

		/// Set the non-active text color
		if(i != GetSelection())
			dc.SetTextForeground(m_nonActiveTextColor);

		if( TabHasImage(i) )
		{
			int imageXOffset = textOffset - 16 - ((wxFlatNotebookBase *)m_pParent)->GetPadding();
			dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()], 
				posx + imageXOffset, imageYCoord, true);
		}

		dc.DrawText(GetPageText(i), posx + textOffset, imageYCoord);
		
		int textWidth, textHeight;
		dc.GetTextExtent(GetPageText(i), &textWidth, &textHeight);

		// Restore the text forground
		dc.SetTextForeground(m_activeTextColor);

		// Update the tab position & size
		m_pagesInfoVec[i].SetPosition(wxPoint(posx, VERTICAL_BORDER_PADDING));
		m_pagesInfoVec[i].SetSize(wxSize(tabWidth, tabHeight));
	}

	// Incase we are in VC8 style, redraw the active tab (incase it is visible)
	if(GetSelection() >= m_nFrom && GetSelection()< m_nFrom + (int)vTabsInfo.GetCount() )
	{
                int tabWidth; //int width, tabWidth, pom;
		bool hasImage = TabHasImage(GetSelection());

		dc.SetFont(boldFont);
		dc.GetTextExtent(GetPageText(GetSelection()), &width, &pom);

		tabWidth = ((wxFlatNotebookBase *)m_pParent)->GetPadding() * 2 + width;

		if( hasImage )
			tabWidth += (16 + ((wxFlatNotebookBase*)m_pParent)->GetPadding());

		// Set the active tab font, pen brush and font-color
		dc.SetPen(borderPen);
		dc.SetBrush(selBrush);
		dc.SetFont(boldFont);
		dc.SetTextForeground(m_activeTextColor);
		DrawVC8Tab(dc, activeTabPosx, GetSelection(), tabWidth, tabHeight);

		// Text drawing offset from the left border of the 
		// rectangle
		int textOffset;

		// The width of the images are 16 pixels
		int vc8ShapeLen = tabHeight - VERTICAL_BORDER_PADDING - 2;
		if( hasImage )
			textOffset = ((wxFlatNotebookBase *)m_pParent)->GetPadding() * 2 + 16 + vc8ShapeLen; 
		else
			textOffset = ((wxFlatNotebookBase *)m_pParent)->GetPadding() + vc8ShapeLen;

		// Draw the image for the tab if any
		int imageYCoord = HasFlag(wxFNB_BOTTOM) ? 6 : 8;

		if( hasImage )
		{
			int imageXOffset = textOffset - 16 - ((wxFlatNotebookBase *)m_pParent)->GetPadding();
			dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[GetSelection()].GetImageIndex()], 
				activeTabPosx + imageXOffset, imageYCoord, true);
		}

		dc.DrawText(GetPageText(GetSelection()), activeTabPosx + textOffset, imageYCoord);
	}

	// Update all tabs that can not fit into the screen as non-visible
	int xx;
	for(xx = m_nFrom + (int)vTabsInfo.GetCount(); xx<(int)m_pagesInfoVec.GetCount(); xx++)
	{
		m_pagesInfoVec[xx].SetPosition(wxPoint(-1, -1));
		m_pagesInfoVec[xx].GetRegion().Clear();
	}

	// Draw the left/right/close buttons 
	// Left arrow
	DrawLeftArrow(dc);
	DrawRightArrow(dc);
	DrawX(dc);
}

void wxStyledTabsContainer::DrawVC8Tab(wxBufferedPaintDC& dc,
								 const int& posx, 
								 const int &tabIdx, 
								 const int &tabWidth, 
								 const int &tabHeight)
{
	wxPen borderPen = wxPen(m_colorBorder);
	wxPoint tabPoints[8];

	// If we draw the first tab or the active tab, 
	// we draw a full tab, else we draw a truncated tab
	//
	//             X(2)                  X(3)
	//        X(1)                            X(4)
	//                                          
	//                                           X(5)
	//                                           
	// X(0),(7)                                  X(6)
	//
	//

	tabPoints[0].x = HasFlag( wxFNB_BOTTOM ) ? posx : posx+m_factor;
	tabPoints[0].y = HasFlag( wxFNB_BOTTOM ) ? 2 : tabHeight - 3;

	tabPoints[1].x = tabPoints[0].x + tabHeight - VERTICAL_BORDER_PADDING - 3 - m_factor;
	tabPoints[1].y = HasFlag( wxFNB_BOTTOM ) ? tabHeight  - (VERTICAL_BORDER_PADDING+2) : (VERTICAL_BORDER_PADDING+2);

	tabPoints[2].x = tabPoints[1].x + 4;
	tabPoints[2].y = HasFlag( wxFNB_BOTTOM ) ? tabHeight  - VERTICAL_BORDER_PADDING : VERTICAL_BORDER_PADDING;

	tabPoints[3].x = tabPoints[2].x + tabWidth - 2;
	tabPoints[3].y = HasFlag( wxFNB_BOTTOM ) ? tabHeight  - VERTICAL_BORDER_PADDING : VERTICAL_BORDER_PADDING;

	tabPoints[4].x = tabPoints[3].x + 1;
	tabPoints[4].y = HasFlag( wxFNB_BOTTOM ) ? tabPoints[3].y - 1 : tabPoints[3].y + 1;

	tabPoints[5].x = tabPoints[4].x + 1;
	tabPoints[5].y = HasFlag( wxFNB_BOTTOM ) ? (tabPoints[4].y - 1 ): tabPoints[4].y + 1;

	tabPoints[6].x = tabPoints[2].x + tabWidth;
	tabPoints[6].y = tabPoints[0].y;

	tabPoints[7].x = tabPoints[0].x;
	tabPoints[7].y = tabPoints[0].y;

	m_pagesInfoVec[tabIdx].SetRegion(8, tabPoints);

	// Draw the polygon
	wxBrush br = dc.GetBrush();
	dc.SetBrush(wxBrush(tabIdx == GetSelection() ? m_activeTabColor : m_colorTo ));
	dc.SetPen(wxPen(tabIdx == GetSelection() ? wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW) : m_colorBorder));
	dc.DrawPolygon(8, tabPoints);

	// Restore the brush
	dc.SetBrush(br);

	wxRect rect = GetClientRect();

	if(tabIdx != GetSelection() && !HasFlag( wxFNB_BOTTOM ))
	{
		// Top default tabs
		dc.SetPen(wxPen(m_colorBorder));
		int lineY = rect.height;
		wxPen curPen = dc.GetPen();
		curPen.SetWidth(1);
		dc.SetPen(curPen);
		dc.DrawLine(posx, lineY, posx+rect.width, lineY);
	}

	// Incase we are drawing the selected tab, we draw the border of it as well
	// but without the bottom (upper line incase of wxBOTTOM)
	if(tabIdx == GetSelection())
	{
		borderPen = wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW));
		wxBrush brush(*wxTRANSPARENT_BRUSH);
		dc.SetPen(borderPen);
		dc.SetBrush(brush);
		dc.DrawPolygon(8, tabPoints);

		// Delete the bottom line (or the upper one, incase we use wxBOTTOM) 
		dc.SetPen(wxPen(wxT("WHITE")));
		dc.DrawLine(tabPoints[0], tabPoints[6]);
	}


	FillVC8GradientColor(dc, tabPoints, tabIdx == GetSelection(), tabIdx);

	// Draw a thin line to the right of the non-selected tab
	if(tabIdx != GetSelection())
	{
		dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
		dc.DrawLine(tabPoints[4].x-1, tabPoints[4].y, tabPoints[5].x-1, tabPoints[5].y);
		dc.DrawLine(tabPoints[5].x-1, tabPoints[5].y, tabPoints[6].x-1, tabPoints[6].y);
	}
}

void wxStyledTabsContainer::FillVC8GradientColor(wxBufferedDC &dc, const wxPoint tabPoints[], const bool bSelectedTab, const int tabIdx)
{
	// calculate gradient coefficients
	wxColour col2 = HasFlag( wxFNB_BOTTOM ) ? m_colorTo : m_colorFrom;
	wxColour col1 = HasFlag( wxFNB_BOTTOM ) ? m_colorFrom : m_colorTo;

	// If colorful tabs style is set, override the tab color
	if(HasFlag(wxFNB_COLORFUL_TABS))
	{
		if( !m_pagesInfoVec[tabIdx].GetColor().Ok() )
		{
			// First time, generate color, and keep it in the vector
			wxColor tabColor(GenTabColour());
			m_pagesInfoVec[tabIdx].SetColor(tabColor);
		}

		if( HasFlag(wxFNB_BOTTOM) )
		{
			col2 = LightColour( m_pagesInfoVec[tabIdx].GetColor() , 50 );
			col1 = LightColour( m_pagesInfoVec[tabIdx].GetColor() , 80 );
		}
		else
		{
			col1 = LightColour( m_pagesInfoVec[tabIdx].GetColor() , 50 );
			col2 = LightColour( m_pagesInfoVec[tabIdx].GetColor() , 80 );
		}
	}

	int size = abs(tabPoints[2].y - tabPoints[0].y) - 1;

	double rstep = double((col2.Red() -   col1.Red())) / double(size), rf = 0,
		gstep = double((col2.Green() - col1.Green())) / double(size), gf = 0,
		bstep = double((col2.Blue() -  col1.Blue())) / double(size), bf = 0;

	wxColour currCol;
	int y = tabPoints[0].y; 

	// If we are drawing the selected tab, we need also to draw a line 
	// from 0->tabPoints[0].x and tabPoints[6].x -> end, we achieve this
	// by drawing the rectangle with transparent brush
	// the line under the selected tab will be deleted by the drwaing loop
	if(bSelectedTab)
		DrawTabsLine(dc);

	while( true )
	{
		if(HasFlag( wxFNB_BOTTOM ))
		{
			if(y > tabPoints[0].y + size)
				break;
		}
		else
		{
			if(y < tabPoints[0].y - size)
				break;
		}

		currCol.Set(
			(unsigned char)(col1.Red() + rf),
			(unsigned char)(col1.Green() + gf),
			(unsigned char)(col1.Blue() + bf)
			);

		dc.SetPen(bSelectedTab ? wxPen(m_activeTabColor) : wxPen(currCol));
		int startX = GetStartX(tabPoints, y) ;
		int endX   = GetEndX(tabPoints, y);
		dc.DrawLine(startX, y, endX, y);

		// Draw the border using the 'edge' point
		dc.SetPen(wxPen(bSelectedTab ? wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW) : m_colorBorder));
		
		dc.DrawPoint(startX, y);
		dc.DrawPoint(endX, y);
		
		// Progress the color 
		rf += rstep; gf += gstep; bf += bstep;

		HasFlag( wxFNB_BOTTOM ) ? y++ : y--;
	}
}

int wxStyledTabsContainer::GetStartX(const wxPoint tabPoints[], const int &y)
{
	double x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	// We check the 3 points to the left
	long style = GetParent()->GetWindowStyleFlag();
	bool bBottomStyle = style & wxFNB_BOTTOM ? true : false;
	bool match = false;

	if(bBottomStyle)
	{
		for(int i=0; i<3; i++)
		{	
			if(y >= tabPoints[i].y && y < tabPoints[i+1].y)
			{
				x1 = tabPoints[i].x;
				x2 = tabPoints[i+1].x;
				y1 = tabPoints[i].y;
				y2 = tabPoints[i+1].y;
				match = true;
				break;
			}
		}
	}
	else
	{
		for(int i=0; i<3; i++)
		{	
			if(y <= tabPoints[i].y && y > tabPoints[i+1].y)
			{
				x1 = tabPoints[i].x;
				x2 = tabPoints[i+1].x;
				y1 = tabPoints[i].y;
				y2 = tabPoints[i+1].y;
				match = true;
				break;
			}
		}
	}

	if(!match)
		return tabPoints[2].x;

	// According to the equation y = ax + b => x = (y-b)/a
	// We know the first 2 points

	double a;
	if(x2 == x1) 
          return (int)x2;
	else
		a = (y2 - y1) / (x2 - x1);

	double b = y1 - ((y2 - y1) / (x2 - x1)) * x1;

	if(a == 0) 
		return (int)x1;

	double x = (y - b) / a;
	return (int)x;
}

int wxStyledTabsContainer::GetEndX(const wxPoint tabPoints[], const int &y)
{
	double x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	// We check the 3 points to the left
	long style = GetParent()->GetWindowStyleFlag();
	bool bBottomStyle = style & wxFNB_BOTTOM ? true : false;
	bool match = false;

	if(bBottomStyle)
	{
		for(int i=7; i>3; i--)
		{	
			if(y >= tabPoints[i].y && y < tabPoints[i-1].y)
			{
				x1 = tabPoints[i].x;
				x2 = tabPoints[i-1].x;
				y1 = tabPoints[i].y;
				y2 = tabPoints[i-1].y;
				match = true;
				break;
			}
		}
	}
	else
	{
		for(int i=7; i>3; i--)
		{	
			if(y <= tabPoints[i].y && y > tabPoints[i-1].y)
			{
				x1 = tabPoints[i].x;
				x2 = tabPoints[i-1].x;
				y1 = tabPoints[i].y;
				y2 = tabPoints[i-1].y;
				match = true;
				break;
			}
		}
	}

	if(!match)
		return tabPoints[3].x;

	// According to the equation y = ax + b => x = (y-b)/a
	// We know the first 2 points
	double a = (y2 - y1) / (x2 - x1);
	double b = y1 - ((y2 - y1) / (x2 - x1)) * x1;

	if(a == 0) 
		return (int)x1;

	// Vertical line
	if(x1 == x2)
		return (int)x1;

	double x = (y - b) / a;
	return (int)x;
}

wxColor wxStyledTabsContainer::GenTabColour()
{
	return RandomColor();
}

wxColor wxStyledTabsContainer::GetSingleLineBorderColor()
{
	if(HasFlag(wxFNB_VC8))
		return m_activeTabColor;
	else
		return wxPageContainerBase::GetSingleLineBorderColor();
}
