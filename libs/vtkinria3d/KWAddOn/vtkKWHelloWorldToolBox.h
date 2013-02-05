/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWHelloWorldToolBox.h 612 2008-01-14 10:59:37Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2008-01-14 10:59:37 +0000 (Mon, 14 Jan 2008) $
Version:   $Revision: 612 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkKWHelloWorldToolBox_h
#define __vtkKWHelloWorldToolBox_h

#include <vtkKWToolBox.h>

class vtkMetaDataSet;

class vtkKWPushButton;


class KW_ADDON_EXPORT vtkKWHelloWorldToolBox : public vtkKWToolBox
{
public:
  static vtkKWHelloWorldToolBox* New();
  vtkTypeRevisionMacro(vtkKWHelloWorldToolBox,vtkKWToolBox);

  virtual void ButtonCallBack();
  
  virtual void Update();

protected:
  vtkKWHelloWorldToolBox();
  ~vtkKWHelloWorldToolBox();

  // Description:
  // Create the widget.
  virtual void CreateButton();
  virtual void CreateWidget();
  virtual void Pack();

  vtkKWPushButton* Button;
  

private:
  
  vtkKWHelloWorldToolBox(const vtkKWHelloWorldToolBox&);   // Not implemented.
  void operator=(const vtkKWHelloWorldToolBox&);                 // Not implemented.

};

#endif


