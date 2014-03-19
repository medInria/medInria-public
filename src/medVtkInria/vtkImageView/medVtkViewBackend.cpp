#include <medVtkViewBackend.h>


medVtkViewBackend::medVtkViewBackend(vtkImageView2D *view2D_,vtkImageView3D *view3D_,
                                     vtkRenderer *renderer2D_, vtkRenderer *renderer3D_, vtkRenderWindow *renWin_)
    : view2D(view2D_),view3D(view3D_),renWin(renWin_), renderer2D(renderer2D_), renderer3D(renderer3D_)
{
}
