/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWPageView.h 488 2007-11-22 10:47:11Z ntoussaint $
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
#ifndef _vtkKWViewImage2DDialog_h
#define _vtkKWViewImage2DDialog_h

#include "vtkINRIA3DConfigure.h"

#include "vtkKWTopLevel.h"

class vtkKWRenderWidget;
class vtkImageView2D;
class vtkImageView2DWithTracer;
class vtkImageView2DWithRegister;
class vtkRenderWindow;
class vtkImageData;




class KW_ADDON_EXPORT vtkKWViewImage2DDialog : public vtkKWTopLevel
{
public:
  static vtkKWViewImage2DDialog* New();
  vtkTypeRevisionMacro(vtkKWViewImage2DDialog,vtkKWTopLevel);

  vtkGetObjectMacro (View, vtkImageView2D);
  vtkGetObjectMacro (RenderWidget, vtkKWRenderWidget);
  
protected:
  vtkKWViewImage2DDialog();
  ~vtkKWViewImage2DDialog();

  // Description:
  // Create the widget.
  virtual void CreateWidget();
  

private:
  vtkKWViewImage2DDialog(const vtkKWViewImage2DDialog&);   // Not implemented.
  void operator=(const vtkKWViewImage2DDialog&);        // Not implemented.


  vtkImageView2D*    View;
  vtkKWRenderWidget* RenderWidget;
  
  
};


#endif

