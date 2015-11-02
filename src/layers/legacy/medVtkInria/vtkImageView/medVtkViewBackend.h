/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <medAbstractView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderWindow.h>

class MEDVTKINRIA_EXPORT medVtkViewBackend : public medViewBackend
{
public:
    medVtkViewBackend(vtkImageView2D * view2D_, vtkImageView3D * view3D_, vtkRenderWindow * renWin_);

    vtkImageView2D * view2D;
    vtkImageView3D * view3D;
    vtkRenderWindow * renWin;
};
