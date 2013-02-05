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
// $RCSfile: TreeMultiItemBase.cpp,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/TreeMultiItemBase.cpp,v $
// $Revision: 1.4 $
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

#include "wx/TreeMultiItemBase.h"
#include "wx/TreeMultiItemNode.h"

/** TreeMultiItemBase
  * This class is the node of the whole tree. All the other classes in the
  * tree descend from this node. It contains the type of the class without
  * using RTTI. NOTE: this class cannot be constructed directly
  */

TreeMultiItemBase::TreeMultiItemBase(TreeMultiItemNode *parent)
	: _excluded(false)

	, _x(0)
	, _y(0)
	, _width(0)
	, _height(0)
#if(CHECKBOXVIEW)
	, _checkbox(false)
	, _checkboxState(0)
#endif
        , _parent(parent)
{
    // TODO: Enter your constructor code here
}

TreeMultiItemBase::~TreeMultiItemBase()
{
    // TODO: Enter your destructor code here
}

bool TreeMultiItemBase::IsVisible()
{
	// are we excluded? then we are not visible as well

	if(IsExcluded())
		return false;

	// check every parent up until the last and if one of them is collapsed
	// we are not visible

	TreeMultiItemNode *p = GetParent();
	while(p)
	{
		if(!p->IsExpanded())
			return false;
		else
			p = p->GetParent();
	}

	return true;
}
