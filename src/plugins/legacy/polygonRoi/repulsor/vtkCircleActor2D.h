#pragma once
/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2020. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/


#include <polygonRoiPluginExport.h>
#include <vtkActor2D.h>

class vtkPolyDataMapper2D;
class vtkPolyData;

class POLYGONROIPLUGIN_EXPORT vtkCircleActor2D : public vtkActor2D
{
public:
    vtkTypeMacro(vtkCircleActor2D,vtkActor2D)
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Instantiate object.
    static vtkCircleActor2D *New();

    // Description:
    // Draw the Circle
    int RenderOverlay(vtkViewport *viewport);
    int RenderOpaqueGeometry(vtkViewport *viewport);
    virtual int RenderTranslucentPolygonalGeometry(vtkViewport *) {return 0;}

    // Description:
    // Does this prop have some translucent polygonal geometry?
    virtual int HasTranslucentPolygonalGeometry();

    // Description:
    // Release any graphics resources that are being consumed by this actor.
    // The parameter window could be used to determine which graphic
    // resources to release.
    void ReleaseGraphicsResources(vtkWindow *);

    // Description:
    // Shallow copy of an Circle actor. Overloads the virtual vtkProp method.
    void ShallowCopy(vtkProp *prop);

    vtkSetVector2Macro(Center,double)
    vtkGetVectorMacro(Center,double,3)
    vtkSetMacro(Radius,int)
    vtkGetMacro(Radius,int)

protected:
    vtkCircleActor2D();
    ~vtkCircleActor2D();

    double Center[3];
    int Radius;
    
    virtual void BuildCircle(vtkViewport *viewport);

    vtkPolyData         *Circle;
    vtkPolyDataMapper2D *CircleMapper;

private:
    vtkCircleActor2D(const vtkCircleActor2D&);  // Not implemented.
    void operator=(const vtkCircleActor2D&);  // Not implemented.
};
