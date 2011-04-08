/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWDataManagerWidget.h 874 2008-05-29 16:43:34Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-05-29 17:43:34 +0100 (Thu, 29 May 2008) $
Version:   $Revision: 874 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _vtkKWDataManagerWidget_h_
#define _vtkKWDataManagerWidget_h_

#include "vtkINRIA3DConfigure.h"

#include <vtkKWFrame.h>
#include <string>
#include <vector>

class vtkKWMultiColumnList;
class vtkKWMainWindowInteractor;
class vtkMetaDataSet;



class KW_ADDON_EXPORT vtkKWDataManagerWidget: public vtkKWFrame
{
 public:

  static vtkKWDataManagerWidget* New();
  vtkTypeRevisionMacro(vtkKWDataManagerWidget,vtkKWFrame);
  
  virtual void Update();
  void SelectionChangedCallback();
  void VisibilityChangedCallback(int row, int col, int state);
  void DoubleClickCallback();

  void SetParentObject(vtkKWMainWindowInteractor* object)
  { this->ParentObject = object; }
  
  vtkMetaDataSet* GetSelectedMetaDataSet (void);
  //BTX
  std::vector<vtkMetaDataSet*> GetSelectedMetaDataSets (void);
  //ETX
  void SetSelectedMetaDataSet (vtkMetaDataSet* metadataset);
  

  
 protected:
  vtkKWDataManagerWidget();
  ~vtkKWDataManagerWidget();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  virtual void Pack();

  bool Lock;
  

 private:
  
  vtkKWDataManagerWidget(const vtkKWDataManagerWidget&);      // Not implemented.
  void operator=(const vtkKWDataManagerWidget&);        // Not implemented.

  vtkKWMultiColumnList*      DataSetList;
  vtkKWMainWindowInteractor* ParentObject;
  
};

#endif
