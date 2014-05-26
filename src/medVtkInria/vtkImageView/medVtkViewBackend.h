#pragma once

#include <medVtkInriaExport.h>
#include <medAbstractView.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include <QVTKWidget2.h>
#include <QVTKGraphicsItem.h>

class MEDVTKINRIA_EXPORT medVtkViewBackend : public medViewBackend
{
public:
    medVtkViewBackend(vtkImageView2D * view2D_, vtkImageView3D * view3D_, vtkRenderWindow * renWin_);

    vtkImageView2D * view2D;
    vtkImageView3D * view3D;
    vtkRenderWindow * renWin;
};
