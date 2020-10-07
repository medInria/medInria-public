#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "manualRegistrationPluginExport.h"

#include <vtkHandleWidget.h>
#include <vtkImageView2D.h>

class vtkSeedCallback;

class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistrationLandmark : public vtkObject
{

public:
    static manualRegistrationLandmark* New();
    vtkTypeMacro(manualRegistrationLandmark, vtkObject)
    
    vtkGetObjectMacro(View, vtkImageView2D)
    vtkGetObjectMacro(HandleWidget, vtkHandleWidget)
    vtkGetMacro(ToDelete,bool)
    void SetToDelete(bool);
    vtkPointHandleRepresentation2D* GetHandleRepresentation();
    int* GetIndices();
    void SetIndices(int ind[3]);

    void SetViewRepRwi(vtkImageView2D* view, vtkPointHandleRepresentation2D* rep, vtkRenderWindowInteractor* rwi);
    virtual void Off();
    virtual void On();
    void ShowOrHide();    

private:
    vtkImageView2D* View;
    vtkHandleWidget* HandleWidget;
    vtkSmartPointer<vtkSeedCallback> SeedCallback;
    int Indices[3]; // indices in image
    bool ToDelete;

protected:
    manualRegistrationLandmark();
    ~manualRegistrationLandmark();
};

