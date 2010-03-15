//---------------------------------------------------------------------------
// $RCSfile: TreeMultiItemBase.h,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/wx/TreeMultiItemBase.h,v $
// $Revision: 609 $
// $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifndef __TREEMULTIITEMBASE_HPP_
#define __TREEMULTIITEMBASE_HPP_

// forward definitions
class TreeMultiItemRoot;
class TreeMultiItemWindow;
class TreeMultiItemNode;

#include "vtkINRIA3DConfigure.h"

class WX_ADDON_EXPORT TreeMultiItemBase
{
protected:
	TreeMultiItemBase(TreeMultiItemNode *parent);

	/** Name property of this item, useful for assigning / coupling
	    external variable reference to this item. It is not mandatory */
	wxString _name;

	/** Flag to indicate that this node is visible or not. The Node can be 
	    temporarily excluded by flagging it. This has also consequences for all 
		the children of this node (if any) */
	bool _excluded;

	/** Type of instanced class */
	int _type;

	/** Calculated X, Y position */
	int _x, _y;

	/** Calculated width, height */
	int _width, _height;
	
#if(CHECKBOXVIEW)
	/** Checkbox flag (draw one or not) */
	bool _checkbox;
	
	/** Internal draw state. This is a unintelligent state, and should be
	    updated when this node has some checked and some unchecked children
		then this should become a tri-stated item */
	int _checkboxState;
#endif

public:
	virtual ~TreeMultiItemBase();

	// type of tree item

    int GetType() const { return _type; };
	TreeMultiItemNode *GetParent() const { return _parent; };

	/** Get name of this node */
	wxString GetName() const {
		return _name;
	};

	/** returns the instance pointer if the current node is
        a TreeMultiItemRoot, and NULL when it's not.
	*/
	virtual TreeMultiItemRoot *IsTreeMultiItemRoot() const {
		return 0;
	};

    /** returns the instance pointer if the current node is
        a TreeMultiItemWindow, and NULL when it's not.
	*/
	virtual TreeMultiItemWindow *IsTreeMultiItemWindow() const {
		return 0;
	};

    /** returns the instance pointer if the current node is
        a TreeMultiItemNode, and NULL when it's not.
	*/
	virtual TreeMultiItemNode *IsTreeMultiItemNode() const {
		return 0;
	};

	/** Sets or resets the excluded flag. When excluded node is not visible */
	void SetExcluded(bool excluded) {
		_excluded = excluded;
	};

	/** Get / Set routine for X */
	void SetX(int x) {
		_x = x;
	};

	int GetX() const {
		return _x;
	};

	/** Get / Set routine for Y */
	void SetY(int y) {
		_y = y;
	};

	int GetY() const {
		return _y;
	};

	/** Get / Set routine for height */
	void SetHeight(int height) {
		_height = height;
	};

	int GetHeight() const {
		return _height;
	};

	/** Get / Set routine for width */
	void SetWidth(int width) {
		_width = width;
	};

	int GetWidth() const {
		return _width;
	};

	/** Returns true when this item is drawn somewhere in the
	    tree. Whenever a parent of this node is collapsed, it
	    is not visible and it is not necessary to perform i.e.
	    redraw actions. 
		
		It also returns false when this node is excluded from the
		tree. 
		
		wxTreeMultiCtrl::Exclude(), wxTreeMultiCtrl::Include())
	*/
	bool IsVisible();

	/* Returns if this node is excluded from the tree. If this item
	   is a Node, then all kids are excluded as well */
	bool IsExcluded() const {
		return _excluded;
	};

#if(CHECKBOXVIEW)
	/** Sets checkbox or not. This does not influence the state of the checkbox */
	void SetCheckbox(bool value) {
		_checkbox = value;
	};

	/** Returns current state of the checkbox view */
	bool GetCheckbox() const {
		return _checkbox;
	};

	/** Sets checkbox state. If 0 it's unchecked, 1 = checked and 2 = tristate */
	virtual void SetCheckboxState(int state) {
		wxCHECK2(state < 3 && state >= 0, return);
		_checkboxState = state;
	};

	/** Returns current state of checkbox */
	int GetCheckboxState() const {
		return _checkboxState;
	};
#endif

private:
	TreeMultiItemNode *_parent;

};

#endif

