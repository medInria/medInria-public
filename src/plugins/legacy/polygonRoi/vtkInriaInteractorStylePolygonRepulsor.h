/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include <medAbstractView.h>

#include <polygonRoi.h>
#include <polygonRoiPluginExport.h>

#include <vtkCircleActor2D.h>
#include <vtkInteractorStyleImageView2D.h>
#include <vtkPoints.h>
#include <vtkProperty2D.h>
#include <vtkSmartPointer.h>

class vtkUnsignedCharArray;

class POLYGONROIPLUGIN_EXPORT vtkInriaInteractorStylePolygonRepulsor : public vtkInteractorStyleImageView2D
{
public:
    static vtkInriaInteractorStylePolygonRepulsor *New();
    vtkTypeMacro(vtkInriaInteractorStylePolygonRepulsor, vtkInteractorStyleImageView2D)
    void PrintSelf(ostream& os, vtkIndent indent);

    virtual void OnMouseMove();
    virtual void OnLeftButtonDown();
    virtual void OnLeftButtonUp();
    void SetCurrentView(medAbstractView *view);
    bool IsInRepulsorDisk(double *pt);
    vtkGetObjectMacro(RepulsorActor,vtkCircleActor2D)
    vtkGetObjectMacro(RepulsorProperty,vtkProperty2D)

protected:
    vtkInriaInteractorStylePolygonRepulsor();
    ~vtkInriaInteractorStylePolygonRepulsor();

    virtual void RedefinePolygons();
    void ReallyDeletePoint(vtkSmartPointer<vtkPoints> points, vtkIdList *idList);
    void  DisplayPointFromPolygon(double *displayPoint, QList<double*> list, int ind);

    int Position[2];
    int On;
    int Radius;
    medAbstractView *CurrentView;
    vtkCircleActor2D *RepulsorActor;
    vtkProperty2D *RepulsorProperty;
    QList<polygonRoi*> ListPolygonsToSave;

private:
    vtkInriaInteractorStylePolygonRepulsor(const vtkInriaInteractorStylePolygonRepulsor&);  // Not implemented
    void operator=(const vtkInriaInteractorStylePolygonRepulsor&);  // Not implemented
};

