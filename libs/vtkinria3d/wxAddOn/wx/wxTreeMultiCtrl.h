//---------------------------------------------------------------------------
// $RCSfile: wxTreeMultiCtrl.h,v $
// $Source: /proj/epidaure/home/private/CVS/mips/SpecPkgs/FrontEndPkg/wxAddOn/wx/wxTreeMultiCtrl.h,v $
// $Revision: 609 $
// $Date: 2008-01-14 10:21:24 +0100 (Mon, 14 Jan 2008) $
//---------------------------------------------------------------------------
// Author:      Jorgen Bodde
// Copyright:   (c) Jorgen Bodde
// License:     wxWidgets License
//---------------------------------------------------------------------------

#ifndef __WXTREEMULTICTRL_H__
#define __WXTREEMULTICTRL_H__


#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/dynarray.h>


// classes for internal tree structure
#include "TreeMultiItemRoot.h"

// Flags for wxTreeMultCtrl use
#define wxTMC_DEFAULT_STYLE wxSIMPLE_BORDER

/** When set, this flag will change the background of all the panels
    and subwindows added to the wxTreeMultiCtrl. */
#define wxTMC_BG_ADJUST_CNT   0x000000001
/** When set, the background of the wxButton windows will also be changed.
    This needs wxTMC_ADJUST_BACKGROUND to be set as well. */
#define wxTMC_BG_ADJUST_BTN   0x000000002
/** When set, all backgrounds from windows and buttons will be adjusted
    to match the control background */
#define wxTMC_BG_ADJUST_ALL   (wxTMC_BG_ADJUST_CNT | wxTMC_BG_ADJUST_BTN)
/** When set, the control added is resized horizontally over the complete 
    length. When the user resizes the window, the control is resized as well */
#define wxTMC_SPAN_WIDTH      0x000000004

#define wxTreeMultiCtrlNameStr wxT("wxTreeMultiCtrl")

#define WXTMC_GUTTER_DEFAULT 6      // gutter before and after image of [+] or [-]
#define WXTMC_YSPACING_DEFAULT 10   // per item 10 pixels spacing extra for every item
#define WXTMC_PIXELS_PER_UNIT 10


enum
{
    /// wxCoord was found in the gutter of the node (in front of the [+])
    wxTMC_HITTEST_GUTTER = 1,
    /// wxCoord was found inside a window, or behind it, on a Window node
    wxTMC_HITTEST_WINDOW,
    /// wxCoord was found on a caption of a Node or Root item
    wxTMC_HITTEST_CAPTION,
    /// wxCoord was found on a checkbox (only when item really has a checkbox)
    wxTMC_HITTEST_CHECKBOX
};


/** 
    The wxTreeMultiItem is used to pass information to the user about the tree node in question. It can also be used point out where the
    subnodes should be added.

    For example:

        wxTreeMultiItem item = MyMultiTreeCtrl->AddRoot("This is the first root");

        // now add a node to the root
        if(item.IsOk())
            MyMultiTreeCtrl->AppendWindow(item, new wxStaticText(MyMultiTreeCtrl, -1, "This is a static text");


    The wxTreeMultiItem comes in three internal types which are used in the wxTreeMultiCtrl. For functions such as
    wxTreeMultiCtrl::AddRoot, a Root item is returned. For functions such as wxTreeMultiCtrl::AppendWindow, a
    Window node is returned. For functions such as wxTreeMultiCtrl::AppendNode, a Node item is returned.

    A Root item represents a top level item. Special operations on this item is adding multiple nodes to it from a
    top level.

    A Node item represents a container item which has multiple sub items which can both be Window items, or Node items.
    A root item is a special type of Node item, so all operations which are only for Node items, are also for Root items.

    A Window node represents an end node, which points to a wxWindow which represents the visual contents of this node.
    This type of node can hold any wxWindow derived class.

*/
#include "vtkINRIA3DConfigure.h"

class WX_ADDON_EXPORT wxTreeMultiItem
{
private:
    TreeMultiItemBase *_item;
public:
    /** Default declaration constructor, and should be used when declaring an item which gets assigned a new
        value by e.g. wxTreeMultiCtrl::AppendNode. */
    wxTreeMultiItem() {
        _item = 0;
    };

#ifndef _NO_DOXYGEN_

    wxTreeMultiItem(TreeMultiItemBase *ptr) {
        _item = ptr;
    };

    // Returns the TreeMultiItemBase class. This shoult *NOT* be
    // used if you don't know what you are doing! This means never use it. */
    TreeMultiItemBase *GetItem() const {
        return _item;
    };

#endif // _NO_DOXYGEN_

    /** The copy operator. Used in assigning one wxTreeMultiItem to another, usually in returning wxTreeMultiItem
        classes from the wxTreeMultiCtrl.
    */
    void operator=(const wxTreeMultiItem &item) {
        _item = item._item;
    };

	/** Returns the parent of the current wxTreeMultiItem. This means the wxTreeMultiNode is returned. It can
	    be useful to check or clear the checkbox at this level. */
	wxTreeMultiItem GetParent() {
		wxCHECK(IsOk(), wxTreeMultiItem(0));
		return wxTreeMultiItem(_item->GetParent());
	};

    /** Validates if the wxTreeMultiItem is a valid instance to use in the wxTreeMultiCtrl. Returns TRUE when there
        is a member value is associated with it, or FALSE when not. This value can also be checked when this class is returned from a wxTreeMultiCtrl operation. For example:

            wxTreeMultiItem item = MyTreeMultiCtrl->FindNode("NAME");
            if(item.IsOk())
            {
                // Found the node, now do something with it
            }
            else
            {
                // Sorry, not found
            }

    */
    bool IsOk() const {
        return _item != 0;
    };

    /** Returns TRUE when the wxTreeMultiItem is a Window item. This means the item is both valid, and points to a
        window node. This means it can be used in functions that expect a Window node. If the item is invalid or a Window
        item, FALSE is returned
        wxTreeMultiCtrl::AppendWindow
    */
    bool IsWindowItem() const {
        if(_item && _item->IsTreeMultiItemWindow())
            return true;
        return false;
    };

    /** Returns TRUE when the wxTreeMultiItem is a Node item. This means the item is both valid, and points to a
        node element (i.e. setting the caption, counting node children, etc). If the item is invalid or not a Node
        item, FALSE is returned.
        When the item is actually a root item (specialised node type), this also returns TRUE.
        wxTreeMultiCtrl::AppendNode wxTreeMultiCtrl::AddRoot
    */
    bool IsNodeItem() const {
        if(_item && (_item->IsTreeMultiItemNode() || _item->IsTreeMultiItemRoot()))
            return true;
        return false;
    };

    /** Returns True when the wxTreeMultiItem is a Root (top node) item. This means the item is both valid,
        and points to a root element (this item is always shown as first node in the tree). A root node is
        always visible, but can contain other collapsed children. If the item is invalid or not a Root node,
        FALSE is returned.
    */
    bool IsRootItem() const {
        if(_item && _item->IsTreeMultiItemRoot())
            return true;
        return false;
    };

    /** Returns TRUE if the node is expanded, else FALSE if it is collapsed. If the item is not a Root or Node
        item, an assertion failure is generated (in debug mode) or FALSE is returned (in release mode).

        wxTreeMultiCtrl::Expand, wxTreeMultiCtrl::Collapse, wxTreeMultiCtrl::CollapseAndReset
    */
    bool IsExpanded() const {
        wxCHECK(_item, false);
        TreeMultiItemNode *n = _item->IsTreeMultiItemNode();
        wxCHECK(n, false);

        return n->IsExpanded();
    };

    /** Returns true if this node is excluded from drawing. Please note that when this node is not visible
        due to a higher excluded parent node but not literally excluded itself, it will return false (not excluded).
        Use wxTreeMultiCtrl::GetExcludedParent() to get the node that hides this one. */
    bool IsExcluded() {
        wxCHECK(_item, false);
        return _item->IsExcluded();
    }

    /** Returns true if this node is visible. Please note that when this node is a child node of a collapsed
        node, it is not visible. Also if this node is a child node of an excluded node, it is also not visible.
        It does <b>NOT</b> return false when it's drawn somewhere outside of the visible area. */
    bool IsVisible() {
        wxCHECK(_item, false);
        return _item->IsVisible();
    }

};

/** 
    This class can be used to modify the behaviour of the Window node to be added, and can be reused to pass along
    upon every wxTreeMultiCtrl::AppendWindow call. For example:

        wxTreeMultiWindowInfo wndinfo;

        MyTreeMultiCtrl->AppendWindow(item, new wxStaticText(MyTreeMultiCtrl,
                                      -1, "Press this button"), "", wndinfo));

        // now adjust the spacing of our next window to add, by indenting
        // 4 times the indent size

        wndinfo.Indent(4);
        MyTreeMultiCtrl->AppendWindow(item, new wxButton(MyTreeMultiCtrl,
                                      -1, "Apply"), "", wndinfo);

    The class is created to prevent long parameter lists in the functions. Other ways to manipulate a newly added
    Window node are;

    Change top spacing of the Window node to be added
    Change front spacing (indent) of the Window node
    Set extra flags for this Window node
*/

//-------------------------------------------------------------------------------------------------------------------

class WX_ADDON_EXPORT wxTreeMultiWindowInfo
{
private:
    /** Flag that indicates that every window background in the assigned wxWindow needs te be overriden with the
        current colour of the TreeMultiCtrl. This is a deep adjustment and will do every subwindow until none are
        left. If 'false' no adjustment is done. For AppendWindow only */
    int _flags;
    /** Extra indent for every item that is added. This will add front spacing to the item's x calculation.
        For AppendNode and AppendWindow. Spacing is added behind the collapse image not in front. */
    int _frontSpacing,
        _frontSpacingOrg;

    /** Extra top spacing for every item that is added. This will add top spacing to the item's y calculation */
    int _topSpacing;

#if(CHECKBOXVIEW)
    /** Checkstate for checkboxed property sheets */
    bool _checkState;
#endif

public:
    wxTreeMultiWindowInfo(int flags, int frontSpacing, int topSpacing, bool checkState = false)
        : _flags(flags)
        , _frontSpacing(frontSpacing)
        , _frontSpacingOrg(frontSpacing)
        , _topSpacing(topSpacing)
#if(CHECKBOXVIEW)
        , _checkState(checkState)
#endif
    {
        // constructor
    }
    /** Adds indent to original front spacing and increments it with that value (quick extra indent).
        The original value of FrontSpacing (see SetFrontSpacing() gets indented by multiplying
        the 'num' factor (default = 1) times the indent value. So if the FrontSpacing was set to 10,
        and Indent(2) is called, the new indent will be 30. (10 + 2 * 10). The original FrontSpacing
        value is remembered, so calling Outdent(2) will bring it back to 10 (30 - (2*10). Calling
        SetFrontSpacing with a new value, deletes the old indent.

        Outdent, SetFrontSpacing
    */
    int Indent(int num = 1) {
        if(num > 0)
            _frontSpacing += (_frontSpacingOrg * num);
        return _frontSpacing;
    };
    /** Reduces indent a number of times, by subtracting 'num' times the original FrontSpacing value from
        the current FrontSpacing value (quick extra indent). Thus if the original FrontSpacing was set to
        10, and the current indent value is 40, Outdent(2) will do 40 - (2 * 10) = 20

        Indent, SetFrontSpacing
    */
    int Outdent(int num = 1) {
        _frontSpacing -= (_frontSpacingOrg * num);
        if(_frontSpacing < 0)
            _frontSpacing = 0;
        return _frontSpacing;
    };

    /** Sets FrontSpacing value. This also resets the current indent level.  */
    void SetFrontSpacing(int frontSpacing) {
        _frontSpacing = frontSpacing;
        _frontSpacingOrg = frontSpacing;
    };
    /** Gets front spacing. This will return the current indent level set. If Indent or Outdent is used,
        this level represents the current Indent level.*/
    int GetFrontSpacing() const {
        return _frontSpacing;
    };

    /** Sets top spacing. This means the number of pixels spacing between the last node and the next one to
        be added.
    */
    void SetTopSpacing(int topSpacing) {
        _topSpacing = topSpacing;
        if(_topSpacing < 0)
            _topSpacing = 0;
    };

    /* Gets top spacing currently set. */
    int GetTopSpacing() const {
        return _topSpacing;
    };

    /** Sets the current flags set, and returns previous flags state.
        wndflags wxTreeMultiWindowInfo flags@endlink
    */
    int SetFlags(int flags) {
        int tmp = _flags;
        _flags = flags;
        return tmp;
    };

    /** Gets the current flags set.
        wndflags wxTreeMultiWindowInfo flags@endlink
    */
    int GetFlags() const {
        return _flags;
    };

    /** Or's the given flag with the current flagset, and adds it. The
        old flag set is returned before the OR is applied.
        wndflags wxTreeMultiWindowInfo flags@endlink
    */
    int AddFlag(int flag_mask) {
        int tmp = _flags;
        _flags |= flag_mask;
        return tmp;
    };

    /** And's the given flag in a negated manner and removes this flag set from
        the current flags.  The old flag set is returned.
        wndflags wxTreeMultiWindowInfo flags@endlink
    */
    int RemoveFlag(int flag_mask) {
        int tmp = _flags;
        _flags &= ~(flag_mask);
        return tmp;
    };

#if(CHECKBOXVIEW)
    /** Sets the default checkstate if checkboxes if SetCheckboxView is chosen. For all the
        items to be added (even captions) this checkstate is chosen. The default checkstate
        is false. <b>This needs USE_CHECKBOXVIEW set to YES, or CHECKBOXVIEW=1 during compile</b> */
    void SetDefaultCheckState(bool checkState = true) {
        _checkState = checkState;
    };

    /** Gets the default checkstate of the item. <b>This needs USE_CHECKBOXVIEW set to YES, or CHECKBOXVIEW=1 during compile</b>
        SetDefaultCheckState */
    bool GetDefaultCheckState() const {
        return _checkState;
    };
#endif
};

// Default info string
#ifndef LINUX
  #define wxTreeMultiWindowInfoDefault wxTreeMultiWindowInfo(wxTMC_BG_ADJUST_CNT, 8, 0)
#else
  #define wxTreeMultiWindowInfoDefault wxTreeMultiWindowInfo(wxTMC_BG_ADJUST_ALL, 8, 0)
#endif

/** 
    The wxTreeMultiCtrl is the container class which can be filled in to get the tree shaped structure in
    which the controls are displayed. The user can add / delete new nodes at runtime, expand or collapse
    nodes, perform get or set actions on the controls inside, and iterate over the items.

    A typical usage of the wxTreeCtrl is;

    wxTreeMultiCtrl *tmc = new wxTreeMultiCtrl(this, -1);

    // add root
    wxTreeMultiItem item = tmc->AddRoot("This is the first root");

    // create a wxTreeMultiWindowInfo object for indenting etc
    wxTreeMultiWindowInfo wndinfo;

    // add subitem to root
    tmc->AppendWindow(item, new wxButton(tmc, -1, "Press this"), "", wndinfo);

    // indent a few
    wndinfo.Indent(4);

    // add a new one
    tmc->AddWindow(item, new wxCheckBox(tmc, ID_MYCHECK, "Check this!"), "", wndinfo);

    // now check the value of the checkbox
    tmc->SetBooleanValue(ID_MYCHECK, true);


    <b>IMPORTANT:</b> Every control added to the wxTreeMultiCtrl has to be child of the wxTreeMultiCtrl.

*/
class WX_ADDON_EXPORT wxTreeMultiCtrl: public wxScrolledWindow
{
    DECLARE_DYNAMIC_CLASS(wxTreeMultiCtrl)

    /**  When tab is pressed, the focus can get out of sight. This should be resolved by
       catching the TAB, see what window is focussed, and scroll the window so it's visible */
private:
    TreeMultiItemRoot _root;

    long _style;
    wxBitmap *_expandBmp, *_collBmp;

    /** Delta Y for bitmap, to center it on the caption */
    int _iconDeltaY, _checkDeltaY;

    /* Caption height. This is the height of the font of this window */
    int _captionHeight;

    /** The gutter spacing in front and back of the image. This determines the amount of spacing in front
        of each item */
    int _gutterWidth;

    /** The image width of the [+] / [-] icon. This is also calculated in the gutter */
    int _iconWidth;

    /** The image height of the [+] / [-] icon. This is calculated as minimal size and to allign */
    int _iconHeight;

    /** Max height. This is either the [+] bitmap or the checkbox */
    int _maxHeight;


    /** Extra Y spacing between the items. */
    int _spacingY;

    /** Two step create prevention. Something with GetWindowSize freaks out when not actually created */
    bool _create_called;

#if(CHECKBOXVIEW)
    wxBitmap *_checkBmp, *_uncheckBmp, *_tristateBmp;

    /** Toggles checkbox view. If true, every item created (caption or node) will be created with checkbox */
    bool _checkboxView;

    /** Height and weight for checkbox */
    int _checkHeight, _checkWidth;
#endif

    /** Does the actual collapsing / expanding. So that Expand and Collapse aren't using the same code twice */
    void DoFold(TreeMultiItemBase *item, bool expand, bool recursive);

    /** Redraws and recalculates the nodes from the current node. It will also clear all 'dirty' flags when
        they are recalculated */
    void RedrawFromNode(TreeMultiItemNode *n);

    /** Redraws from parent node of this node */
    void RedrawFromParentNode(TreeMultiItemBase *b);

    /** Draws checkbox belonging to this node, in the given state */
    void DrawCheckbox(TreeMultiItemBase *b, wxDC &dc, bool convertScrolled = false);

    /** Recalculate the space needed based on every node. It goes as follows.
        - A foldable node (TreeMultiItemNode) is as high as the 'Jg' y-size. This type only has a caption plus a
          picture which explains the state ([+] or [-]), the width will be the caption length
        - A window node will be as high as the y size of the window. The X size is the total length of the
          window.
        - For every child node an indent will be added to the x size.

        Only visible nodes will be recalculated. Non visible (collapsed) children will be skipped  */
    void RecalculateNodePositions();

    /** Recurses into the whole tree and provides every node with the proper x and y values. All non visible items
        are skipped (so when it encounters a collapsed node, it's children are not calculated). Returns the last Y */
    int CalculateNodeDimensions(TreeMultiItemBase *b, int currentY, int level);

    void DrawNode(TreeMultiItemBase *b, wxDC &dc);

    /** Sets background colour of all the windows and subwindows in this wxWindow. This is very handy
        for wxPanel derived classes which need to be made equal to the background colour of the wxTreeMultiCtrl */
    void SetWindowBackgroundColour(wxWindow *wnd, const wxColour &col, int flags);

    /** Shows or hides the assigned window (if there is any) */
    void ShowTreeMultiWindow(TreeMultiItemWindow *window, bool show = true);

    /** Goes down the tree, and if a window is not visible (thus all it's children aren't too) it will hide
        the window so it is not shown on the control */
    void UpdateAllWindowVisibility();

    /** Hides tree multi window. It this is a TreeMultiItemNode, hide all subwindows as well. */
    void UpdateTreeMultiWindowVisibility(TreeMultiItemBase *b, bool show);

    /** Recalculates totally needed virtual size of the wxTreeMultiCtrl. It will scan for
        the largest window, with the biggest size, and report that back */
    void RecalculateVirtualSize();

    /** Adjusts scrollbars in window, usually done after virtual size (x,y) is recalculated */
    virtual void AdjustScrollbars(int x, int y);

    /** Recalculates and accumulates largest x and y */
    void RecalculateVirtualSizeFromNode(const TreeMultiItemNode *node, int &x, int &y);

    /** Scans for TreeMultiItemBase node that contains x,y and in area returns a hittest constant to
        indicate what matched */
    TreeMultiItemBase *FindNodeByPoint(TreeMultiItemBase *b, wxPoint &pt, int &area);

    /** Scans for TreeMultiItemWindow that holds the wxWindow pointer. Does not scan in panels or does
        a deep search. Reason, this function is used to advance to next TreeMultiItemWindow for focus
        on this wxScrolledWindow. If a sub window is found, it will skip other windows on that same level */
    wxTreeMultiItem FindWindowNode(wxWindow *wnd, TreeMultiItemNode *n = 0);

    /** Finds next visible window item in chain. If not found use FindFirstVisibleItem to start from the
        beginning */
    TreeMultiItemWindow *FindNextVisibleWindowItem(TreeMultiItemBase *b, int index = -1);

    /** This captionFont is made equal to the font of the wxScrolledWindow. As extra the bold face
        is set on it when this is wanted by the user (see flags) */
    wxFont _captionFont;

    /** Adjust the centering of the bitmap icons (collapse / expand) when the caption font changes. They need to
        be centered in the middle of the font, so a bit of deltaY adjustment is needed */
    void AdjustIconsDeltaY();

	/** Calculate the spanning of the individual nodes */
	void CalculateNodeSpanning(TreeMultiItemBase *b);

#if(CHECKBOXVIEW)
    /** Recurse to all the children and set the checkbox state (even when there is no checkbox) */
    void SetRecursiveCheckState(TreeMultiItemNode *n, bool check);

    /** From current checked node, all children will be scanned and the parent node of this node
        will get tristate if the checked items are scattered (some are some aren't). If all nodes in this
        node are checked, the parent node gets checked all the way up to the last one that matches
        criteria. If all are cleared, parent node gets cleared */
    void ScanTristateCheckstates(TreeMultiItemBase *b);
#endif

private:
    void Init();

    // handlers
    //---------

    //virtual void OnDraw(wxDC& dc);
    void OnPaint(wxPaintEvent &event);
    void OnMouseClick( wxMouseEvent &event );
    void OnKey( wxKeyEvent &event);
	void OnSize(wxSizeEvent &event);

	/** Recalculates the spanning controls */
	void RecalculateSpanSizes();

public:
    /** Two step constructor. Call Create when this constructor is called to build up the
        wxTreeMultiCtrl
    */
    wxTreeMultiCtrl()
        : _create_called(false)
    {

        Init();
    }

    /** The default constructor. The style wxTAB_TRAVERSAL is enforced to make sure that the
        focus handling is being done correctly. The styles to be used are styles valid for
        the wxWindow and wxScrolledWindow
    */
    wxTreeMultiCtrl(wxWindow *parent, wxWindowID id = -1,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxTMC_DEFAULT_STYLE,
                        const wxValidator &validator = wxDefaultValidator,
                        const wxString& name = wxTreeMultiCtrlNameStr)
        : _style(style | wxTAB_TRAVERSAL)
        , _create_called(false)
    {
        Create(parent, id, pos, size, style, validator, name);
    }

    /** Destructor */
    virtual ~wxTreeMultiCtrl();

    /** Two step creation. Whenever the control is created without any parameters, use Create to actually
        create it. Don't access the control's public methods before this is called

        wxTreeMultiCtrl()
    */
    bool Create(wxWindow *parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTMC_DEFAULT_STYLE,
                const wxValidator &validator = wxDefaultValidator,
                const wxString& name = wxTreeMultiCtrlNameStr);

    // accessors
    //----------

    /** @name Add and delete methods
        To add and delete node items like a Root item, Window item or Node item. This allows the user to
        manipulate the tree, and build it up.
        @{
    */

    /** Adds a root node to the wxTreeMultiItem. There can be many root nodes. Use this wxTreeMultiNode pointer to add
        more subnodes to it. */
    wxTreeMultiItem AddRoot(const wxString &caption, const wxString &name = wxEmptyString);

    /** Adds a window to the tree control. Use this wxTreeMultiItem method to add a window subnode class to the
        current wxTreeMultiItem. The wxTreeMultiItem must point to a Node class. If this is not the case
        an empty wxTreeMultiItem is returned. The mask is used to override the mask settings of the
        wxTreeMultiWindowInfo class. This can be handy to set or clear extra flags only needed for certain
        situations */
    wxTreeMultiItem AppendWindow(const wxTreeMultiItem &item, wxWindow *window = 0, const wxString &name = wxEmptyString,
                              const wxTreeMultiWindowInfo &info = wxTreeMultiWindowInfoDefault, int mask = 0);

    /** Adds a node to the tree control. Use this wxTreeMultiItem method to add a recursive subnode class to the
        current wxTreeMultiItem. The wxTreeMultiItem must point to a Node class as parent. If this is not the case
        an empty wxTreeMultiItem is returned. A node can contain multiple nodes or window classes */
    wxTreeMultiItem AppendNode(const wxTreeMultiItem &item, const wxString &caption = wxEmptyString,
                            const wxString &name = wxEmptyString);

    /** Delete item from the tree control. Whenever it is present, delete it. If not, return false. After
        deletion the wxTreeMultiItem is 0, thus IsOk will return false */
    bool Delete(wxTreeMultiItem &item);

    /** Deletes all the items from the wxTreeMultiCtrl. */
    void DeleteAllItems() {
        _root.Clear();
        Refresh();
    };

    /** Deletes all children of the current node. The wxTreeMultiItem needs to be of type Node to
        do this. Call GetParentNode to get the parent wxTreeMultiItem which is always a node. */
    void DeleteChildren(const wxTreeMultiItem &item);

    /** @}
    */

    /** @name Expand and collapse methods
         These methods are all for items of type Node.
        @{
    */

    /** Expand all nodes and subnodes. Recursive = true means all subnodes are also expanded. */
    void ExpandNodes(bool recursive = false);

    /** Collapses all nodes and subnodes. Recursive = true means all subnodes are also expanded. */
    void CollapseNodes(bool recursive = false);

    /** Expands given node, and with recursive, also the subnodes */
    void Expand(const wxTreeMultiItem &item, bool recursive);

    /** Collapses given node, and with recursive, also the subnodes. If this item is not a node, but a window.
        the parent is resolved and that node is collapsed */
    void Collapse(const wxTreeMultiItem &item, bool recursive);

    /** Collapses this node and removes all children from it. This is only applicable on
        wxTreeMultiItems which are of type Node / Root. */
    void CollapseAndReset(const wxTreeMultiItem &item);

    /** Folds the given node. The same as Expand and Collapse, but the state can be
        given through a parameter */
    void Fold(const wxTreeMultiItem &item, bool expand = true) {
        if(expand)
            Expand(item, false);
        else
            Collapse(item, false);
    };

    /** @}
    */

    /** Visibility manipulation
        These methods allow you to manipulate a certain wxTreeMultiItem to temporarily exclude or to include
        the node from drawing. Whenever it is excluded, all operations can still be performed, however
        the node may not be visible.
        @{
    */

    /** Excludes this node from drawing. When excluded, the node will dissapear from the control but is still
        physically present in the tree itself. If this node is a composite node with children, they will all be
        hidden from the control. */
    void Exclude(const wxTreeMultiItem &item);

    /** Includes an excluded node. If the node was already present on the drawing, nothing happens. If the
        node is included after exclusion it will become visible. If the parent node is still excluded or
        collapsed (not visible) this will not show, until the parent node shows */
    void Include(const wxTreeMultiItem &item);

    /** Returns the parent that is responsible for excluding this node. If there is no excluded node as parent,
        the wxTreeMultiItem is zero. */
    wxTreeMultiItem GetExcludedParent(const wxTreeMultiItem &item);

    /** @}
    */

    /** Checks if the point is under one of the given areas. The returned areas can be
        wxTMC_HITTEST_GUTTER If the front part of the item is clicked (where the node is)
        wxTMC_HITTEST_WINDOW If located in the window area
        wxTMC_HITTEST_CAPTION If located on the caption of the MultiTreeItemNode

        Returned is the item which is located under the mouse, or none (IsOk = false) if
        no item under the mouse */
    wxTreeMultiItem HitTest(wxPoint &pt, int &flags);

    /** @name Find methods
         These methods are used for finding a node in the wxTreeMultiCtrl.
        @{
    */

    /** Searches for the given name from the given level and lower. It will return a wxTreeMultiItem which needs
        to be checked with IsOk() to see if it is a correct item. IsRootItem, IsNodeItem and IsWindowItem can be
        used to determine the type of item. If the search returned an item which doesn't satisfy the query, you can
        restart the search from here, with skip = true to skip the passed item. */
    wxTreeMultiItem FindItem(const wxTreeMultiItem &item, const wxString &name, bool ignoreCase = false,
                             bool skipFirst = false);

    /** Searches all nodes for the given name and returns the one found. This is a specialised method for FindItem */
    wxTreeMultiItem FindItem(const wxString &name, bool ignoreCase = false) {
        return FindItem(wxTreeMultiItem(&_root), name, ignoreCase, false);
    };

    /** @}
    */

    /** Returns the number of children in this node. If this node is not of type Node, count returns -1. */
    int GetChildrenCount(const wxTreeMultiItem &item) {
        if(item.IsNodeItem())
        {
            TreeMultiItemNode *n = (TreeMultiItemNode *)item.GetItem();
            return n->GetNodeCount();
        }

        return -1;
    };

    /** This method finds the current focused window, and returns the wxTreeMultiItem that has this window as pointer.
        If the wxTreeMultiItem is not ok, the focused window is on a sub-panel, or not on this control. If the focus is
        on a sub-panel, this panel will handle the focusing. If you are still interested in this window, try
        wxWindow::FindFocus which will always return with a pointer if a wxWindow has focus */
    wxTreeMultiItem GetFocus();

    /** @name Smart window Get/Set methods
         These methods are used for quickly getting or setting primitive values, like boolean, string, or selections into
         primitive wxWindow controls like a wxRadioButton, wxCheckBox, wxTextCtrl, etc.
         The methods allow you to quickly set e.g. a boolean value by using the wxWindow ID of the control. A type
         check is performed before the value is actually set with the appropiate method for it. If it fails, an assertion
         will follow in debug mode. If you want to get values back similar get methods are present to return values.
         This allows the user to quickly retrieve or set values by ID, without the fuss of remembering the pointer or
         node ID of the wxTreeMultiCtrl.
        @{
    */

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxCheckBox</b> and <b>wxRadioButton</b>. Whenever
        it finds a proper type it will return true or false. Whenever no proper type is found, it will return with false.
        In debug mode it will cause an assertion failure as well, to notify the developer something is wrong.

        SetBooleanValue
    */
    bool GetBooleanValue(int wndId);

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxTextCtrl</b>, <b>wxChoice</b> and <b>wxComboBox</b>.
        Whenever it finds a proper type it will return the (selected) text in the control. Whenever no proper type is found,
        it will return with an empty string. In debug mode it will cause an exception error as well if the cast fails, to notify
        the developer something is wrong.

        SetTextValue
    */
    wxString GetTextValue(int wndId);

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxCheckBox</b> and <b>wxRadioButton</b>. Whenever
        it finds a proper type it set the control's value to the given boolean parameter. Nothing happens when no proper type
        is found, however debug mode it will cause an assertion failure.

        GetBooleanValue
    */
    void SetBooleanValue(int wndId, bool value = true);

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxTextCtrl</b>. If the control is not found or
        of the wrong type, an asserion failure (in debug mode) follows. <br><i><b>NOTE: </b></i> this method
        does not typecast to wxChoice or wxComboBox and the likes. If a selection in one of those needs to be set, use the
        SetSelectionValue / GetSelectionValue combination.
        GetTextValue, GetSelectionValue, SetSelectionValue
    */
    void SetTextValue(int wndId, const wxString &value = wxEmptyString);

    /** Retrieves the wxWindow * associated with the wxTreeMultiItem. This only works for Window typed wxTreeMultiItem
        classes. If this type does not match or no window is associated, 0 is returned. */
    wxWindow *GetWindow(const wxTreeMultiItem &item) {
        if(item.IsWindowItem())
            return ((TreeMultiItemWindow *)item.GetItem())->GetWindow();
        return 0;
    };

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxListBox, wxChoice, wxComboBox</b>.
        If the control is not found or of the wrong type, an asserion failure (in debug mode) follows. If it is found
        the selection is set in the control.
    */
    void SetSelectionValue(int wndId, int sel);

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxListBox, wxChoice, wxComboBox</b>.
        If the control is not found or of the wrong type, an asserion failure (in debug mode) follows. In release mode -1 is
        returned. If it is found the selection index is returned.
    */
    int GetSelectionValue(int wndId);

    /** This function calls FindWindow on the wxTreeMultiCtrl to find the window with the given ID (which is added with
        AppendWindow). When it is found, it is type casted towards a <b>wxListBox</b>. If the control is not found or
        of the wrong type, an asserion failure (in debug mode) follows. In release mode an empty wxArrayInt is returned.
        If it is found the selections are stored in the wxArrayInt. This is only valid for wxListBox classes with multiple
        selection flag set.
    */
    void GetSelectionValues(int wndId, wxArrayInt &sels);

#if(CHECKBOXVIEW)
    /** Gets the checkbox state of the wxTreeMultiItem pointed out by "item". If the item does not have a checkbox
        associated (or the item is not ok), it will return -1. If the checkbox is checked it will return 1,
        unchecked is 0, and tri-state (usually only for caption nodes) it will return 2. <b>This needs USE_CHECKBOXVIEW
        set to YES, or CHECKBOXVIEW=1 during compile</b>*/
    int GetCheckboxState(const wxTreeMultiItem &item, bool recursive) {
        wxCHECK(item.IsOk(), -1);

        // return the checkbox state
        TreeMultiItemBase *b = item.GetItem();
        if(b->GetCheckbox())
            return b->GetCheckboxState();

        return -1;
    };

    /** Sets the checkbox state of the wxTreeMultiItem pointed out by "item". If the item does not have a checkbox
        associated (or the item is not ok), it will simply ignore this. <b>This needs USE_CHECKBOXVIEW
        set to YES, or CHECKBOXVIEW=1 during compile</b>. Contributed by Thomas Enickl.*/
	void SetCheckboxState( const wxTreeMultiItem &item, int state ) {
		wxCHECK2( item.IsOk(), return );
		TreeMultiItemBase *b = item.GetItem();
		if( b->GetCheckbox() )
			b->SetCheckboxState( state );
		RedrawFromNode( item.GetItem()->GetParent() );
	};
#endif

    /** @}
    */

    /** @name Extended visibility and manipulation methods
        These methods alter the wxTreeMultiCtrl appearance, or add more functionality like checkboxes before each node.
        @{
    */

    /** Sets the Y spacing of the wxTreeMultiCtrl to a new size. This can be used to give the controls some more
        spacing in between */

    void SetSpacingY(int spacingY) {
        if(spacingY >= 0)
        {
            _spacingY = spacingY;
            RedrawFromNode(0);
        }
    };

    /** Gets the Y spacing of the wxTreeMultiCtrl. */

    int GetSpacingY() const {
        return _spacingY;
    };

#if(CHECKBOXVIEW)
    /** This method toggles the checkbox view mode. Whenever set to true, the next window item, or caption node item
        will get a checkbox. <b>This needs USE_CHECKBOXVIEW set to YES, or CHECKBOXVIEW=1 during compile</b>
        GetCheckboxView, wxTreeMultiWindowInfo::SetDefaultCheckState
    */
    void SetCheckboxView(bool value) {
        _checkboxView = value;
    };

    /** This method returns the checkbox state currently used. <b>This needs USE_CHECKBOXVIEW set to YES, or
        CHECKBOXVIEW=1 during compile</b>
        SetCheckboxView, wxTreeMultiWindowInfo::SetDefaultCheckState, wxTreeMultiWindowInfo::GetDefaultCheckState
    */
    bool GetCheckboxView() const {
        return _checkboxView;
    };
#endif

    /** @}
    */

    /** @name Iteration methods
        Allows the user to iterate through a wxTreeMultiCtrl node, and get all the children
        @{
    */

    /** Returns the first child of this node. The type of wxTreeMultiItem needs to be of Node. Whenever not succesful,
        the item returned is not ok (IsOk = false). Upon success, a valid child is returned. The cookie variable doesn't
        need to be initialized */
    wxTreeMultiItem GetFirstChild(const wxTreeMultiItem &item, int &cookie) const;

    /** Returns the next child in the iteration on the level of 'item'. Make sure you called GetFirstChild first
        before calling this one */
    wxTreeMultiItem GetNextChild(const wxTreeMultiItem &item, int &cookie) const;

    /** Returns the last child of this node. The type of 'item' needs to be of Node. Whenever not succesful,
        the item returned is not ok (IsOk = false). Upon success, a valid last child is returned. */
    wxTreeMultiItem GetLastChild(const wxTreeMultiItem &item) const;

    /** @}
    */

    /** @name Get and set methods
        These methods allow you to set or get certain properties of the wxTreeMultiCtrl.
        @{
    */

    /** Get the currently used font for the caption headers in the wxTreeMultiCtrl. If you want to alter this
        font, simply obtain it and copy it to a local font. After that, set this font back using SetCaptionFont()
    */
    const wxFont &GetCaptionFont() const {
        return _captionFont;
    };

    /** Sets the font to be used for the text caption headers. This triggers a complete redraw because x,y sizes can
        differ, and ofcourse all nodes need to be updated. You can call this method at any time not only at the
        beginning. */
    void SetCaptionFont(const wxFont &font);

    /** @}
    */

private:
    DECLARE_EVENT_TABLE()
};

#endif
