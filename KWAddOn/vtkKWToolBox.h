/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWToolBox.h 877 2008-06-04 16:10:40Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-06-04 18:10:40 +0200 (Wed, 04 Jun 2008) $
Version:   $Revision: 877 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWToolBox_h
#define __vtkKWToolBox_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <string>
#include <vector>


class vtkMetaDataSet;
class vtkKWMainWindowInteractor;
class vtkKWToolbar;
class vtkKWIcon;


class KW_ADDON_EXPORT vtkKWToolBox : public vtkKWFrame
{
public:
  static vtkKWToolBox* New();
  vtkTypeRevisionMacro(vtkKWToolBox,vtkKWFrame);

  vtkGetObjectMacro (MetaDataSet, vtkMetaDataSet);
  virtual void SetMetaDataSet (vtkMetaDataSet* metadataset);
  //BTX
  virtual void SetMetaDataSets (std::vector<vtkMetaDataSet*>);
  std::vector<vtkMetaDataSet*> GetMetaDataSets (void)
  { return this->MetaDataSets; }
  //ETX
  
  vtkGetObjectMacro (Icon, vtkKWIcon);
  virtual void SetIcon (vtkKWIcon* icon);

  //BTX
  enum ToolTypeId
  {
    TOOL_HORIZONTAL,
    TOOL_VERTICAL
  };
  //ETX

  vtkGetMacro (ToolType, unsigned int);
  vtkSetMacro (ToolType, unsigned int);
  vtkGetObjectMacro (Toolbar, vtkKWToolbar);
  
  virtual void Update();

  virtual void SetMetaDataSetChangedCommand(
    vtkObject *object,const char *method);
  virtual void SetMetaDataSetChangingCommand(
    vtkObject *object,const char *method);
  virtual void SetMetaDataSetModifiedCommand(
    vtkObject *object,const char *method);

  void SetParentObject(vtkKWMainWindowInteractor* parent)
  { this->ParentObject = parent; }
  vtkKWMainWindowInteractor* GetParentObject (void)
  { return this->ParentObject; }
 
  virtual void InvokeMetaDataSetChangedCommand();
  virtual void InvokeMetaDataSetChangingCommand();
  virtual void InvokeMetaDataSetModifiedCommand();

  void SetName (const char* name)
  { this->Name = name; }
  const char* GetName (void)
  { return this->Name.c_str(); }

  virtual void PrepareToPop (void)
  {}
  
  
protected:
  vtkKWToolBox();
  ~vtkKWToolBox();

  // Description:
  // Create the widget.
  virtual void Pack();
  virtual void CreateWidget();
  
  virtual void InvokeObjectMethodCommand(const char *command);
  char* MetaDataSetChangedCommand;
  char* MetaDataSetChangingCommand;
  char* MetaDataSetModifiedCommand;
 
  vtkMetaDataSet*            MetaDataSet;  
  vtkKWMainWindowInteractor* ParentObject;
  vtkKWToolbar*              Toolbar;
  vtkKWIcon*                 Icon;

  //BTX
  std::string Name;
  std::vector<vtkMetaDataSet*> MetaDataSets;
  //ETX
  unsigned int ToolType;
  
  
private:
  
  vtkKWToolBox(const vtkKWToolBox&);   // Not implemented.
  void operator=(const vtkKWToolBox&);                 // Not implemented.

};

#endif




