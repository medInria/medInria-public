/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWInfoToolBox.h 866 2008-05-27 13:59:57Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-05-27 14:59:57 +0100 (Tue, 27 May 2008) $
Version:   $Revision: 866 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWInfoToolBox_h
#define __vtkKWInfoToolBox_h

#include <vtkKWToolBox.h>

class vtkKWPushButton;
class vtkKWEntryWithLabel;
class vtkKWLabel;
class vtkKWMenuButton;
class vtkKWComboBox;


class KW_ADDON_EXPORT vtkKWInfoToolBox : public vtkKWToolBox
{
public:
  static vtkKWInfoToolBox* New();
  vtkTypeRevisionMacro(vtkKWInfoToolBox,vtkKWToolBox);

  void ButtonRequestTagsCallback();
  void ButtonSaveCallback();
  void ButtonDeleteCallback();
  void EntryNameCallback(const char* name);
  void ButtonLoadCallback();
  void ButtonSaveAttributeCallback();
  void ListBoxTabCallback(const char* tab);
  void ListBoxAttributesCallback(const char* attributes);
  void ButtonLUTCallback(const char* lut);
  
  
  virtual void Update();

protected:
  vtkKWInfoToolBox();
  ~vtkKWInfoToolBox();

  // Description:
  // Create the widget.

  
  virtual void CreateWidget();
  virtual void Pack();

  vtkKWPushButton*     ButtonRequestTags;
  vtkKWPushButton*     ButtonSave;
  vtkKWPushButton*     ButtonDelete;
  vtkKWEntryWithLabel* EntryName;
  vtkKWLabel* LabelType;
  vtkKWLabel* LabelSize;
  vtkKWComboBox*       ListBoxTab;
  vtkKWMenuButton*     ButtonLUT;
  vtkKWPushButton*     ButtonLoad;
  vtkKWPushButton*     ButtonSaveAttribute;
  vtkKWComboBox*       ListBoxAttributes;
  

  
private:
  
  vtkKWInfoToolBox(const vtkKWInfoToolBox&);   // Not implemented.
  void operator=(const vtkKWInfoToolBox&);     // Not implemented.



};

#endif
