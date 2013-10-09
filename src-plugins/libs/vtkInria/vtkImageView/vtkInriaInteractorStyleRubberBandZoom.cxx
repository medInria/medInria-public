/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkInriaInteractorStyleRubberBandZoom.h"
#include "vtkInteractorStyleRubberBandZoom.h"
#include "vtkCamera.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro(vtkInriaInteractorStyleRubberBandZoom);

vtkInriaInteractorStyleRubberBandZoom::vtkInriaInteractorStyleRubberBandZoom()
{
    leftButtonModeOn = true;
    rightButtonModeOn = false;
}

vtkInriaInteractorStyleRubberBandZoom::~vtkInriaInteractorStyleRubberBandZoom(){}

void vtkInriaInteractorStyleRubberBandZoom::OnLeftButtonDown()
{
    if (leftButtonModeOn)
        this->Superclass::OnLeftButtonDown();
}

void vtkInriaInteractorStyleRubberBandZoom::OnLeftButtonUp()
{
    if (leftButtonModeOn)
        this->Superclass::OnLeftButtonUp();
}

void vtkInriaInteractorStyleRubberBandZoom::OnRightButtonDown()
{
    if (rightButtonModeOn)
        this->Superclass::OnLeftButtonDown();
}

void vtkInriaInteractorStyleRubberBandZoom::OnRightButtonUp()
{
    if (rightButtonModeOn)
        this->Superclass::OnLeftButtonUp();
}

void vtkInriaInteractorStyleRubberBandZoom::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os, indent);
}

