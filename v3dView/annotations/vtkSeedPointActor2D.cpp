/*
 * vtkSeedPointActor2D.cpp
 *
 *  Created on: 18 oct. 2011 08:23:18
 *      Author: jstark
 */

#include "vtkSeedPointActor2D.h"

#include "vtkCellArray.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkViewport.h"
#include "vtkWindow.h"
#include "vtkMath.h"
#include "vtkRenderer.h"

vtkStandardNewMacro(vtkSeedPointActor2D);

vtkSeedPointActor2D::vtkSeedPointActor2D()
{
    this->PolyData = vtkSmartPointer<vtkPolyData>::New();
    this->Mapper2D = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    this->Mapper2D->SetInput(this->PolyData);
    this->AxisActor = vtkSmartPointer<vtkActor2D>::New();
    this->AxisActor->SetMapper(this->Mapper2D);
    this->SizeInDisplayUnits = 10;

    this->GetPositionCoordinate()->SetCoordinateSystemToWorld();
}

vtkSeedPointActor2D::~vtkSeedPointActor2D()
{
    // TODO Auto-generated destructor stub
}
//----------------------------------------------------------------------------
// Build the representation and render.

int vtkSeedPointActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  int renderedSomething=0;

  this->BuildPolyData(viewport);
  
  if ( this->GetVisibility() )
    {
    renderedSomething += this->AxisActor->RenderOpaqueGeometry(viewport);
    }

  return renderedSomething;
}

//----------------------------------------------------------------------------
// Render the axis, ticks, title, and labels.

int vtkSeedPointActor2D::RenderOverlay(vtkViewport *viewport)
{
  int renderedSomething=0;

  bool shouldRender = true;
  if ( vtkRenderer *ren = vtkRenderer::SafeDownCast(viewport) ) {
      vtkCamera * cam = ren->GetActiveCamera();
      double testPt[4];
      this->GetWorldPosition(testPt);
      testPt[3]=1;
      cam->GetViewTransformMatrix()->MultiplyPoint( testPt , testPt);
      const double * clipR = cam->GetClippingRange();
      const double zPos = -testPt[2];
      if ( zPos < clipR[0] || zPos > clipR[1] ) {
          shouldRender = false;
      }
  }

  // Everything is built, just have to render
  if (shouldRender && this->GetVisibility() )
    {
    renderedSomething += this->AxisActor->RenderOverlay(viewport);
    }

  return renderedSomething;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkSeedPointActor2D::HasTranslucentPolygonalGeometry()
{
  return 0;
}

//----------------------------------------------------------------------------
// Release any graphics resources that are being consumed by this actor.
// The parameter window could be used to determine which graphic
// resources to release.
void vtkSeedPointActor2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->AxisActor->ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------------
void vtkSeedPointActor2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkSeedPointActor2D::BuildPolyData(vtkViewport *viewport)
{
  // Check to see whether we have to rebuild everything
  if ( ( ! viewport->GetVTKWindow() ) ||
        ( viewport->GetMTime() < this->BuildTime ) &&
        ( viewport->GetVTKWindow()->GetMTime() < this->BuildTime ) &&
        ( this->GetMTime() < this->BuildTime ) )
    {
    return;
    }

  vtkDebugMacro(<<"Rebuilding vtkSeedPointActor2D");

  // Initialize and get important info
  this->PolyData->Initialize();
  this->AxisActor->SetProperty(this->GetProperty());

  vtkSmartPointer<vtkCoordinate> actorCoord = vtkSmartPointer<vtkCoordinate>::New();
  actorCoord->SetCoordinateSystemToDisplay();
  actorCoord->SetReferenceCoordinate(this->GetPositionCoordinate());
  this->Mapper2D->SetTransformCoordinate(actorCoord);

  //x = this->PositionCoordinate->GetComputedViewportValue(viewport);

  vtkSmartPointer<vtkPoints>  pts = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
  this->PolyData->SetPoints(pts);
  this->PolyData->SetLines(lines);

  // Generate circle
  // 
  double len = vtkMath::DoubleTwoPi()*this->SizeInDisplayUnits;
  int num = len / 8;
  num = std::min( num , 64 );
  num = std::max( num , 1 );
  num = num *8;

  pts->SetNumberOfPoints(num);
  lines->InsertNextCell(num + 1);

  for ( int i(0); i<num; ++i ) {
      double theta = i * (double)( vtkMath::DoubleTwoPi() / num );
      pts->SetPoint(i, this->SizeInDisplayUnits *  std::cos(theta), this->SizeInDisplayUnits * std::sin(theta), 0. );
      lines->InsertCellPoint(i);
  }
  lines->InsertCellPoint(0);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0);
  lines->InsertCellPoint(num/2);

  lines->InsertNextCell(2);
  lines->InsertCellPoint(0     + num/4);
  lines->InsertCellPoint(num/2 + num/4);

  this->BuildTime.Modified();
}



//----------------------------------------------------------------------------
void vtkSeedPointActor2D::ShallowCopy(vtkProp *prop)
{
  vtkSeedPointActor2D *a = vtkSeedPointActor2D::SafeDownCast(prop);
  if ( a != NULL )
    {
    this->SetSizeInDisplayUnits(a->GetSizeInDisplayUnits());
    }

  // Now do superclass
  this->vtkActor2D::ShallowCopy(prop);
}

unsigned long vtkSeedPointActor2D::GetMTime()
{
    unsigned long mTime = this->Superclass::GetMTime();
    return mTime;
}

void vtkSeedPointActor2D::SetWorldPosition( double x, double y, double z )
{
    this->PositionCoordinate->SetValue(x,y,z);
}

const double * vtkSeedPointActor2D::GetWorldPosition()
{
    return this->PositionCoordinate->GetValue();
}

void vtkSeedPointActor2D::GetWorldPosition( double * xyz )
{
    this->PositionCoordinate->GetValue(xyz);
}
