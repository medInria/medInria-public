/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDataSelectorWidget.h 477 2007-11-20 17:46:10Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-20 18:46:10 +0100 (Tue, 20 Nov 2007) $
Version:   $Revision: 1 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWDataSelectorWidget_h
#define __vtkKWDataSelectorWidget_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <string>
#include <vector>

#include <vtkMetaDataSet.h>

class vtkKWMultiColumnList;
class vtkKWSimpleEntryDialog;
class vtkKWEntry;
class vtkKWCheckButton;
class vtkDataManager;



class KW_ADDON_EXPORT vtkKWDataSelectorWidget : public vtkKWFrame
{
 public:
  static vtkKWDataSelectorWidget* New();
  vtkTypeRevisionMacro(vtkKWDataSelectorWidget,vtkKWFrame);

  virtual void Initialize();
  void SelectionChangedCallback();
  void CellUpdateCallback(int row, int col, int state);
  void DoubleClickCallback();
  void SelectAllCallback(int state);

  vtkGetMacro (SingleSelection, int);
  vtkSetClampMacro (SingleSelection, int, 0, 1);
  vtkBooleanMacro (SingleSelection, int);
  
  //BTX
  std::vector<vtkMetaDataSet*> GetSelection (void);
  //ETX
  
  vtkMetaDataSet* GetHighlightedDataSet (void);
  
  void SetDataManager (vtkDataManager* manager)
  { this->DataManager = manager; }
  vtkDataManager* GetDataManager (void)
  { return this->DataManager; }

  void SetAllowDataType (unsigned int type, bool allow)
  {
    if (type >= vtkMetaDataSet::VTK_META_DATATYPE_NUMBER)
      return;
    this->AllowDataTypeList[type] = allow;
  }
  
  void AlllowOnlyOneDataType(unsigned int type)
  {
    for (unsigned int i=0; i<vtkMetaDataSet::VTK_META_DATATYPE_NUMBER; i++)
      this->SetAllowDataType (i, false);
    
    this->SetAllowDataType (vtkMetaDataSet::VTK_META_IMAGE_DATA, true);
  }

  
  

 protected:
  vtkKWDataSelectorWidget();
  ~vtkKWDataSelectorWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  
  
 private:
  
  vtkKWDataSelectorWidget(const vtkKWDataSelectorWidget&);   // Not implemented.
  void operator=(const vtkKWDataSelectorWidget&);                 // Not implemented.

  vtkKWMultiColumnList* MultiColumnList;
  vtkKWCheckButton*     SelectAllCheckbox;
  
  vtkDataManager*       DataManager;

  int SingleSelection;
  
  bool AllowDataTypeList[vtkMetaDataSet::VTK_META_DATATYPE_NUMBER];
  
  
};

#endif

