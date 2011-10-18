/*
 * vtkSeedPointActor2D.h
 *
 *  Created on: 18 oct. 2011 08:23:18
 *      Author: jstark
 */

#ifndef VTKSEEDPOINTACTOR2D_H_
#define VTKSEEDPOINTACTOR2D_H_

#include <vtkActor2D.h>
#include <vtkSmartPointer.h>

class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkActor2D;

class vtkSeedPointActor2D : public vtkActor2D {
public:
    void PrintSelf(ostream& os, vtkIndent indent);
    vtkTypeMacro(vtkSeedPointActor2D,vtkActor2D);

    // Description:
    // Creates an actor2D with the following defaults: 
    // position (0,0) (coordinate system is viewport);
    // at layer 0.
    static vtkSeedPointActor2D* New();

    virtual ~vtkSeedPointActor2D();

    // Description:
    // Render the actor
    int RenderOverlay(vtkViewport* viewport);
    int RenderOpaqueGeometry(vtkViewport* viewport);
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
    // Shallow copy of an axis actor. Overloads the virtual vtkProp method.
    void ShallowCopy(vtkProp *prop);

    //! Get set the radius of the displayed item
    vtkSetMacro(SizeInDisplayUnits,double);
    vtkGetMacro(SizeInDisplayUnits,double);

    // Description:
    // Return this objects MTime.
    virtual unsigned long GetMTime();

    void SetWorldPosition(double x, double y, double z);
    void SetWorldPosition(const double * xyz) {  SetWorldPosition(xyz[0], xyz[1], xyz[2] ); }
    const double * GetWorldPosition();
    void GetWorldPosition(double * xyz);

protected:
    vtkSeedPointActor2D();

    //! Build the polydata
    void BuildPolyData(vtkViewport *viewport);

    vtkSmartPointer<vtkPolyData>          PolyData;
    vtkSmartPointer<vtkPolyDataMapper2D>  Mapper2D;
    vtkSmartPointer<vtkActor2D>           AxisActor;

    vtkTimeStamp  BuildTime;

private:
    double SizeInDisplayUnits;

};

#endif /* VTKSEEDPOINTACTOR2D_H_ */

