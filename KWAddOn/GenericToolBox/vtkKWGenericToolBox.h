/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWGenericToolBox.h 488 2007-11-22 10:47:11Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-11-22 11:47:11 +0100 (Thu, 22 Nov 2007) $
Version:   $Revision: 488 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWGenericToolBox_h
#define __vtkKWGenericToolBox_h

#include "vtkINRIA3DConfigure.h"

#include <vtkKWToolBox.h>

class vtkMetaDataSet;

class vtkKWPushButton;


class vtkKWGenericToolBox : public vtkKWToolBox
{
public:
  static vtkKWGenericToolBox* New();
  vtkTypeRevisionMacro(vtkKWGenericToolBox,vtkKWToolBox);

  virtual void ButtonSaveCallBack();
  virtual void ButtonDuplicateCallBack();
  virtual void ButtonAttributesCallBack();
  virtual void ButtonLoadArrayCallBack();
  
  virtual void Update();

protected:
  vtkKWGenericToolBox();
  ~vtkKWGenericToolBox();

  // Description:
  // Create the widget.
  virtual void CreateButtons();
  virtual void CreateWidget();
  virtual void Pack();

  vtkKWPushButton* ButtonSave;
  vtkKWPushButton* ButtonDuplicate;
  vtkKWPushButton* ButtonAttributes;
  vtkKWPushButton* ButtonLoadArray;

private:
  
  vtkKWGenericToolBox(const vtkKWGenericToolBox&);   // Not implemented.
  void operator=(const vtkKWGenericToolBox&);                 // Not implemented.

};

#endif


