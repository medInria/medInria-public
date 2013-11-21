#pragma once

#include "vtkImageViewExport.h"
#include <medAbstractView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>

class VTK_IMAGEVIEW_EXPORT medVtkViewBackend : public medViewBackend
{
public:
    medVtkViewBackend(vtkImageView2D * view2D_, vtkImageView3D * view3D_);

    vtkImageView2D * view2D;
    vtkImageView3D * view3D;
};
