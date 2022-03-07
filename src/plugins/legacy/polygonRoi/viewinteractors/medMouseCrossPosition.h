/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractImageView.h>
#include <vtkHandleWidget.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkSmartPointer.h>

class medMouseCrossPosition
{
public:
    explicit medMouseCrossPosition(medAbstractImageView *iView);

    void draw(double *position);
    void erase();
    double *getPosition();
    int isEnabled();
    void getWorldPosition(double *dPos, double *wPos);

private:
    vtkSmartPointer<vtkHandleWidget> handle2d;
    vtkSmartPointer<vtkPointHandleRepresentation2D> handle2dRep;

};

