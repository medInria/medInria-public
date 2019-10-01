/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#ifndef _vtk_LandmarkWidget_h_
#define _vtk_LandmarkWidget_h_

#include <vtkCommand.h>
#include <vtkHandleWidget.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <vtkSphereWidget.h>

class vtkLandmarkWidget;

//BTX
class VTK_EXPORT vtkLandmarkWidgetCommand : public vtkCommand
{    
public:
    static  vtkLandmarkWidgetCommand* New() { return new vtkLandmarkWidgetCommand; }
    void Execute(vtkObject *   caller,
                 unsigned long event,
                 void *        callData);
    void SetLandmark (vtkLandmarkWidget *l);

    void SetWidget2D (vtkHandleWidget *widget);

    void lock(){isLocked = true;}
    void unlock(){isLocked = false;}

protected:
    vtkLandmarkWidgetCommand()
    {
        this->Landmark = nullptr;
    }
    ~vtkLandmarkWidgetCommand(){}

private:
    vtkLandmarkWidget *Landmark;
    vtkHandleWidget *Widget2D;
    bool isLocked;
};


class VTK_EXPORT vtkLandmarkWidget : public vtkSphereWidget
{
public:
    static vtkLandmarkWidget* New();
    vtkTypeMacro(vtkLandmarkWidget, vtkSphereWidget)
    vtkGetObjectMacro (Command, vtkLandmarkWidgetCommand)
    vtkGetMacro (Value, double)
    vtkSetMacro (Value, double)

    vtkGetObjectMacro(SphereActor, vtkActor)
    vtkGetObjectMacro(HandleActor, vtkActor)

    virtual void SetEnabled(int);

    int *GetIndices()
    {
        return indices;
    }
    void SetIndices(int ind[3])
    {
        indices[0]=ind[0];
        indices[1]=ind[1];
        indices[2]=ind[2];
    }

    vtkGetObjectMacro(View2D,vtkImageView2D)
    void SetView2D(vtkImageView2D* View);
    vtkGetObjectMacro(View3D,vtkImageView3D)
    vtkSetObjectMacro(View3D,vtkImageView3D)

    vtkGetObjectMacro(Widget2D,vtkHandleWidget)

    vtkGetMacro(ToDelete,bool)
    vtkSetMacro(ToDelete,bool)

    vtkSetObjectMacro(BigBrother,vtkLandmarkWidget)
    vtkGetObjectMacro(BigBrother,vtkLandmarkWidget)

    QList<vtkLandmarkWidget*> * GetLittleBrothers();

    void AddBrothers(vtkLandmarkWidget *littleBrother);
    void showOrHide2DWidget();
    void updateLandmarksPosFromWidget2D();
    void PropagateEventToLittleBrothers(unsigned long event, vtkLandmarkWidget *l);
    void updatePosition(double *worldPos);
    void cleanUpLittleBrothersReferences();

protected:
    vtkLandmarkWidget();
    ~vtkLandmarkWidget();

private:
    vtkLandmarkWidgetCommand *Command;
    vtkHandleWidget *Widget2D;
    vtkImageView2D *View2D;
    vtkImageView3D *View3D;
    double Value;
    int indices[3]; // indices in image
    bool ToDelete;
    vtkLandmarkWidget *BigBrother;
    QList<vtkLandmarkWidget*> *LittleBrothers;
};

//ETX

#endif //_vtk_LandmarkWidget_h_
