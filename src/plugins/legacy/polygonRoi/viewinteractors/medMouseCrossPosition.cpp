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
    handle2d->SetCurrentRenderer(view2d->GetRenderer());

    handle2dRep = vtkSmartPointer<vtkPointHandleRepresentation2D>::New();
    handle2dRep->GetProperty()->SetLineWidth(1.0);
    handle2dRep->GetProperty()->SetColor(0.0, 1.0, 0.0);

    vtkSmartPointer<vtkCursor2D> cursor2D = vtkSmartPointer<vtkCursor2D>::New();
    cursor2D->AllOff();
    cursor2D->AxesOn();
    cursor2D->PointOn();
    double bounds[6] = {-15.0, 15.0,
                        -15.0, 15.0,
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
    double dPos[3];
    handle2d->ComputeWorldToDisplay(handle2d->GetCurrentRenderer(), position[0], position[1], position[2], dPos);
    handle2dRep->SetDisplayPosition(dPos);
    handle2d->On();
}

void medMouseCrossPosition::erase()
{
    if (handle2d->GetEnabled())
    {
        handle2d->Off();
    }
}

double *medMouseCrossPosition::getPosition()
{
    return handle2dRep->GetDisplayPosition();
}

int medMouseCrossPosition::isEnabled()
{
    return handle2d->GetEnabled();
}

void medMouseCrossPosition::getWorldPosition(double *dPos, double *wPos)
{
    handle2d->ComputeDisplayToWorld( handle2d->GetCurrentRenderer(), dPos[0], dPos[1], 0.0,  wPos);
}