/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkCellArray.h>
#include <vtkCircleActor2D.h>
#include <vtkInteractorObserver.h>
#include <vtkLineSource.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

vtkStandardNewMacro(vtkCircleActor2D)

//----------------------------------------------------------------------------
// Instantiate this object.
vtkCircleActor2D::vtkCircleActor2D()
{
    this->Circle = vtkPolyData::New();
    this->CircleMapper = vtkPolyDataMapper2D::New();
    this->CircleMapper->SetInputData(this->Circle);
    this->SetMapper(this->CircleMapper);
    this->Radius = 100;
    this->Center[0] = 0; 
    this->Center[1] = 0;
}

//----------------------------------------------------------------------------
vtkCircleActor2D::~vtkCircleActor2D()
{
    this->Circle->Delete();
    this->CircleMapper->Delete();
}

//----------------------------------------------------------------------------
// Build the Circle

int vtkCircleActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
    int renderedSomething = 0;

    this->BuildCircle(viewport);

    if ( this->Visibility )
    {
        renderedSomething = Superclass::RenderOpaqueGeometry(viewport);
    }
    
    return renderedSomething;
}

//----------------------------------------------------------------------------
// Render the Circle

int vtkCircleActor2D::RenderOverlay(vtkViewport *viewport)
{
    int renderedSomething = 0;

    // Everything is built, just have to render

    if ( this->Visibility )
    {
        renderedSomething = Superclass::RenderOverlay(viewport);
    }

    return renderedSomething;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkCircleActor2D::HasTranslucentPolygonalGeometry()
{
    return 0;
}

//----------------------------------------------------------------------------
// Release any graphics resources that are being consumed by this actor.
// The parameter window could be used to determine which graphic
// resources to release.
void vtkCircleActor2D::ReleaseGraphicsResources(vtkWindow *win)
{
    Superclass::ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------------
void vtkCircleActor2D::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "Circle Visibility: "
    << (this->Visibility ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
void vtkCircleActor2D::BuildCircle(vtkViewport *viewport)
{
    vtkIdType ptIds[2];

    vtkDebugMacro(<<"Rebuilding Circle");
    
    // Initialize and get important info
    this->Circle->Initialize();

    vtkPoints *pts = vtkPoints::New();
    vtkCellArray *lines = vtkCellArray::New();
    pts->Initialize();
    this->Circle->SetPoints(pts);
    this->Circle->SetLines(lines);

    // Compute Circle
    int nbPoints = 360;
    double deltaAngle = 2*vtkMath::Pi()/(double)(nbPoints);
    double pt[3];
    for(int i=0; i<nbPoints; i++)
    {
        pt[0] = Center[0] + Radius * cosf(deltaAngle*i);
        pt[1] = Center[1] + Radius * sinf(deltaAngle*i);
        pt[2] = 0.0;

        pts->InsertNextPoint(pt[0], pt[1], 0);
        if (i>0)
        {
            ptIds[0] = i-1;
            ptIds[1] = i;
            lines->InsertNextCell(2, ptIds);
            if (i == nbPoints-1)
            {
                ptIds[0] = i;
                ptIds[1] = 0;
                lines->InsertNextCell(2, ptIds);
            }
        }
    }
    pts->Delete();
    lines->Delete();
}

//----------------------------------------------------------------------------
void vtkCircleActor2D::ShallowCopy(vtkProp *prop)
{
    // Now do superclass
    this->vtkActor2D::ShallowCopy(prop);
}
