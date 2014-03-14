/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <vtkOrientedPolygonalHandleRepresentation3D.h>
#include <vtkSmartPointer.h>

class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkActor2D;

class vtkSeedPointHandle : public vtkOrientedPolygonalHandleRepresentation3D {
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeMacro(vtkSeedPointHandle,vtkOrientedPolygonalHandleRepresentation3D);

    // Description:
    // Creates an actor2D with the following defaults: 
    // position (0,0) (coordinate system is viewport);
    // at layer 0.
    static vtkSeedPointHandle* New();

    virtual ~vtkSeedPointHandle();

    //! Get set the radius of the displayed item
    vtkSetMacro(SizeInDisplayUnits,double);
    vtkGetMacro(SizeInDisplayUnits,double);

    // Description:
    // Methods to make this class behave as a vtkProp.
    virtual void ShallowCopy(vtkProp *prop);
    virtual void DeepCopy(vtkProp *prop);

    //! Override vtkHandleRepresentation.
    virtual void SetWorldPosition(double pos[3]);

protected:
    vtkSeedPointHandle();

    //! Build the polydata
    void BuildPolyData();

    vtkSmartPointer<vtkPolyData>          PolyData;
    vtkSmartPointer<vtkPolyDataMapper2D>  Mapper2D;

private:
    double SizeInDisplayUnits;

};



