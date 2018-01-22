#include "vtkFibersManagerCallback.h"
#include <vtkBoxWidget.h>

void vtkFibersManagerCallback::Execute(vtkObject *caller, unsigned long, void*)
{
    vtkBoxWidget *widget = vtkBoxWidget::SafeDownCast(caller);

    vtkPolyData* input = (vtkPolyData*)widget->GetInput();
    if (!input)
    {
        return;
    }

    // Get the poly data defined by the vtkBoxWidget
    vtkPolyData* myBox = vtkPolyData::New();
    widget->GetPolyData(myBox);

    // myBox contains 15 points and points 8 to 13
    // define the bounding box
    double xmin, xmax, ymin, ymax, zmin, zmax;
    double* pt = myBox->GetPoint(8);
    xmin = pt[0];
    pt = myBox->GetPoint(9);
    xmax = pt[0];
    pt = myBox->GetPoint(10);
    ymin = pt[1];
    pt = myBox->GetPoint(11);
    ymax = pt[1];
    pt = myBox->GetPoint(12);
    zmin = pt[2];
    pt = myBox->GetPoint(13);
    zmax = pt[2];

    this->FiberLimiter->SetVOI(xmin, xmax, ymin, ymax, zmin, zmax);
    this->ROIFiberLimiter->Update();
    this->FiberLimiter->Update();

    myBox->Delete();
}



