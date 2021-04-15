/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medMouseCrossPosition.h"

#include <medVtkViewBackend.h>
#include <vtkCursor2D.h>
#include <vtkProperty2D.h>


medMouseCrossPosition::medMouseCrossPosition(medAbstractImageView *iView)
{
    vtkImageView2D *view2d = static_cast<medVtkViewBackend *>(iView->backend())->view2D;

    handle2d = vtkSmartPointer<vtkHandleWidget>::New();
    handle2d->Off();
    handle2d->SetProcessEvents(0);
    handle2d->SetManagesCursor(0);
    handle2d->SetInteractor(view2d->GetInteractor());

    handle2dRep = vtkSmartPointer<vtkPointHandleRepresentation2D>::New();
    handle2dRep->GetProperty()->SetLineWidth(1.0);
    handle2dRep->GetProperty()->SetColor(0.0, 1.0, 0.0);
    vtkSmartPointer<vtkCursor2D> cursor2D = vtkSmartPointer<vtkCursor2D>::New();
    cursor2D->AllOff();
    cursor2D->AxesOn();
    cursor2D->PointOn();
    double bounds[6] = {-40.0, 40.0,
                        -40.0, 40.0,
                        0.0, 0.0};
    cursor2D->SetModelBounds(bounds);
    cursor2D->Update();
    vtkSmartPointer<vtkPolyData> cursor = vtkSmartPointer<vtkPolyData>::New();
    cursor->DeepCopy(cursor2D->GetOutput());
    handle2dRep->SetCursorShape(cursor);

    handle2d->SetRepresentation(handle2dRep);

}

void medMouseCrossPosition::draw(double *position)
{

    handle2dRep->SetDisplayPosition(position);
    handle2d->On();
}

void medMouseCrossPosition::erase()
{
    if (handle2d->GetEnabled())
    {
        handle2d->Off();
    }
}
