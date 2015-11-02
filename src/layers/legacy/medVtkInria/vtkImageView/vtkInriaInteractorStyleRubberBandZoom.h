/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medVtkInriaExport.h>
#include <vtkInteractorStyleRubberBandZoom.h>

class vtkUnsignedCharArray;

class MEDVTKINRIA_EXPORT vtkInriaInteractorStyleRubberBandZoom : public vtkInteractorStyleRubberBandZoom
{
public:
    static vtkInriaInteractorStyleRubberBandZoom *New();
    vtkTypeMacro(vtkInriaInteractorStyleRubberBandZoom, vtkInteractorStyleRubberBandZoom);

    // Description:
    // Event bindings
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    virtual void OnRightButtonDown();
    virtual void OnRightButtonUp();

    void LeftButtonModeOn(){leftButtonModeOn = true;};
    void RightButtonModeOn(){rightButtonModeOn = true;};
    void LeftButtonModeOff(){leftButtonModeOn = false;};
    void RightButtonModeOff(){rightButtonModeOn = false;};
    bool RightButtonModeIsOn(){return rightButtonModeOn;};
    bool LeftButtonModeIsOn(){return leftButtonModeOn;};

protected:
    vtkInriaInteractorStyleRubberBandZoom();
    ~vtkInriaInteractorStyleRubberBandZoom();
    virtual void Zoom();

private:
    vtkInriaInteractorStyleRubberBandZoom(const vtkInriaInteractorStyleRubberBandZoom&);  // Not implemented
    void operator=(const vtkInriaInteractorStyleRubberBandZoom&);  // Not implemented

    bool leftButtonModeOn;
    bool rightButtonModeOn;
};

