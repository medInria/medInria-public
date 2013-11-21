#include "medVtkViewBackend.h"


medVtkViewBackend::medVtkViewBackend(vtkImageView2D *view2D_,vtkImageView3D *view3D_)
    : view2D(view2D_),view3D(view3D_)
{
}
