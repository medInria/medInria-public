/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWToolboxManager.h 885 2008-07-01 09:29:28Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-07-01 10:29:28 +0100 (Tue, 01 Jul 2008) $
Version:   $Revision: 885 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWToolboxManager_h
#define __vtkKWToolboxManager_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <vector>

class vtkKWMainWindowInteractor;
class vtkKWToolbar;
class vtkKWToolBox;
class vtkKWPushButtonSet;
class vtkKWLabel;


class KW_ADDON_EXPORT vtkKWToolboxManager : public vtkKWFrame
{
public:
  static vtkKWToolboxManager* New();
  vtkTypeRevisionMacro(vtkKWToolboxManager,vtkKWFrame);

  void SetParentObject(vtkKWMainWindowInteractor* parent)
  { this->ParentObject = parent; }
  vtkKWMainWindowInteractor* GetParentObject (void)
  { return this->ParentObject; }

 
  void InsertToolbox(vtkKWToolBox* box);
  
  vtkKWToolBox* GetToolbox(unsigned int id);
/*   void RemoveToolbox(vtkKWToolBox* box); */
/*   void RemoveAllToolboxes(void); */

  void SelectToolbox (vtkKWToolBox* box);
  
  virtual void Update();

  void SelectToolboxCallback (unsigned int pos);  

  virtual void PopupToolbox (unsigned int id);
  virtual void PopdownToolbox (unsigned int id);

  //BTX
  std::vector<vtkKWToolBox*> GetToolBoxList()
  { return this->ToolboxList; }
  //ETX
  
protected:
  vtkKWToolboxManager();
  ~vtkKWToolboxManager();

  // Description:
  // Create the widget.
  virtual void Pack();
  virtual void CreateWidget();
  
  vtkKWMainWindowInteractor* ParentObject;

  void ReplaceToolBox (unsigned int pos, vtkKWToolBox* box);
  
  vtkKWFrame* ButtonsFrame;
  vtkKWFrame* NameFrame;
  vtkKWFrame* ToolboxFrame;
  vtkKWLabel* ToolboxLabel;
  
  vtkKWPushButtonSet* ButtonSet;
  
  vtkKWToolBox* SelectedToolbox;



  //BTX
  std::vector<vtkKWToolBox*> ToolboxList;
  std::vector<vtkKWTopLevel*> ToolboxTopLevels;
  //ETX
  
private:
  
  vtkKWToolboxManager(const vtkKWToolboxManager&);   // Not implemented.
  void operator=(const vtkKWToolboxManager&);                 // Not implemented.

};

#endif




