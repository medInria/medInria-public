/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medVtkViewBackend.h>


medVtkViewBackend::medVtkViewBackend(vtkImageView2D *view2D_, vtkImageView3D *view3D_, vtkRenderWindow * renWin_)
    : view2D(view2D_),view3D(view3D_),renWin(renWin_)
{
}
