//---------------------------------------------------------------------------
// $RCSfile: TreeMultiItemRoot.h,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/wx/TreeMultiItemRoot.h,v $
// $Revision: 609 $
// $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifndef __TREEMULTIITEMROOT_HPP_
#define __TREEMULTIITEMROOT_HPP_

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

#include "vtkINRIA3DConfigure.h"
#include "TreeMultiItemBase.h"
#include "TreeMultiItemNode.h"

/** This class is the root class. It can hold many of the TreeMultiItemWindow and
 * TreeMultiItemNode classes.
 */
class WX_ADDON_EXPORT TreeMultiItemRoot : public TreeMultiItemNode
{
public:
	TreeMultiItemRoot();
	virtual ~TreeMultiItemRoot();

	// IsClass method
	virtual TreeMultiItemRoot *IsTreeMultiItemRoot() const {
		return (TreeMultiItemRoot *)this;
	};
};

#endif

