//---------------------------------------------------------------------------
// $RCSfile: TreeMultiItemNode.h,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/wx/TreeMultiItemNode.h,v $
// $Revision: 609 $
// $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifndef __TREEMULTIITEMNODE_HPP_
#define __TREEMULTIITEMNODE_HPP_


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
#include <wx/dynarray.h>
#include "TreeMultiItemBase.h"
#include "TreeMultiItemWindow.h"

/** TreeMultiItemNode
  * This class is the element class for WX_OBJ_ARRAY and it
  * should be filled with the member variables and methods to
  * manipulate each object inside the object array.
  *
  * See wxArray for all base methods manipulating the array
  * Clear() can be used to safely delete all objects.
  */


class WX_ADDON_EXPORT TreeMultiItemNode : public TreeMultiItemBase
{
private:
	TreeMultiItemBaseArray _items;

	/** Caption of this node. */
	wxString _caption;

	/** Indicates if this node is expanded (true) or collapsed (false) */
	bool _nodeExpanded;

public:
	TreeMultiItemNode(TreeMultiItemNode *parent, const wxString &caption = wxEmptyString, const wxString &name = wxEmptyString);
	virtual ~TreeMultiItemNode();

	// isClass method
	virtual TreeMultiItemNode *IsTreeMultiItemNode() const {
		return (TreeMultiItemNode *)this;
	};
	
	// add methods
	void AddNode(TreeMultiItemBase *node);

	// delete methods
	void Clear();
	void DeleteNode(TreeMultiItemBase *node);
	void DeleteNode(int index);
	TreeMultiItemBase *RemoveNode(TreeMultiItemBase *node);
	TreeMultiItemBase *RemoveNode(int index);

	// get methods
	int GetNodeCount() const;
	TreeMultiItemBase *GetNode(int index) const;
	int Index(TreeMultiItemBase *node,  bool searchFromEnd = false) const;
	TreeMultiItemBase *GetNodeNext(int &cookie) const;
	TreeMultiItemBase *Last() const;

	/** Fold function sets or clears the expanded flag. Note when excluded from drawing, this
	    will have no effect on redraw */
	void Fold(bool expand) {
		if(_nodeExpanded != expand)
			_nodeExpanded = expand;
	};

	/** Return caption */
	const wxString &GetCaption() const {
		return _caption;
	};

	/** Returns true if the node is expanded. Subnodes can still be collapsed though */
	bool IsExpanded() const {
		return _nodeExpanded;
	};


};

// declare the array class here
//WX_DECLARE_OBJARRAY(TreeMultiItemBase, TreeMultiItemBaseArray);

WX_DEFINE_ARRAY_WITH_DECL_PTR(TreeMultiItemBase* , TreeMultiItemBaseArray, class WXDLLIMPEXP_ADDON);

#endif

