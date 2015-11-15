#pragma once

#include <vtkImageView2D.h>
#include <vtkSmartPointer.h>
#include <qobject.h>
#include <vtkObject.h>
#include <vtkHandleWidget.h>
#include <manualRegistrationPluginExport.h>

class vtkSeedCallback;

class MANUALREGISTRATIONPLUGIN_EXPORT manualRegistrationLandmark : public vtkObject
{

public:
    static manualRegistrationLandmark* New();
    vtkTypeRevisionMacro(manualRegistrationLandmark, vtkObject);
    
    vtkGetObjectMacro(View, vtkImageView2D);
    vtkGetObjectMacro(HandleWidget, vtkHandleWidget);
    vtkGetMacro(ToDelete,bool);
    void SetToDelete(bool);
    vtkPointHandleRepresentation2D * GetHandleRepresentation();
    int * GetIndices();
    void SetIndices(int ind[3]);
//
//    void lockObserver(bool val);
//    
    void SetViewRepRwi(vtkImageView2D * view, vtkPointHandleRepresentation2D * rep,vtkRenderWindowInteractor * rwi);
    virtual void Off();
    virtual void On();
    void ShowOrHide();    

private:
    vtkImageView2D *View;
    vtkHandleWidget * HandleWidget;
    vtkSmartPointer<vtkSeedCallback> SeedCallback;
    int Indices[3]; // indices in image
    bool ToDelete;

protected:
    manualRegistrationLandmark();
    ~manualRegistrationLandmark();
};

