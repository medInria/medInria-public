#pragma once

#include <medAbstractView.h>
#include <vtkImageView2D.h>

class medVtkViewBackend : public medViewBackend
{
public:
    medVtkViewBackend(vtkImageView2D * view2D_);

    vtkImageView2D * view2D;
};
