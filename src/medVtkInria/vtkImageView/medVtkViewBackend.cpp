#include <medVtkViewBackend.h>


medVtkViewBackend::medVtkViewBackend(vtkImageView2D *view2D_,vtkImageView3D *view3D_,vtkRenderWindow *renWin_, QVTKWidget2 *widget_)
    : view2D(view2D_),view3D(view3D_),renWin(renWin_), qvtkWidget(widget_)
{
}
