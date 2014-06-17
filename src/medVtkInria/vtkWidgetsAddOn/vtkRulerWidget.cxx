/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRulerWidget.h>

#include "vtkActor2D.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkCoordinate.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkProp.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

vtkStandardNewMacro(vtkRulerWidget);
vtkCxxRevisionMacro(vtkRulerWidget, "$Revision: 1.0 $");


class vtkRulerWidgetObserver : public vtkCommand
{
public:
  static vtkRulerWidgetObserver *New()
  { return new vtkRulerWidgetObserver; };
  
  vtkRulerWidgetObserver()
  {
    this->RulerWidget = 0;
  }
  
  virtual void Execute(vtkObject* wdg, unsigned long event, void *calldata)
  {
    if (this->RulerWidget)
    {
      this->RulerWidget->ExecuteCameraUpdateEvent(wdg, event, calldata);
    }
  }
  
  vtkRulerWidget *RulerWidget;
};


vtkRulerWidget::vtkRulerWidget()
{
  
  this->StartEventObserverId = 0;
  
  this->Observer = vtkRulerWidgetObserver::New();
  this->Observer->RulerWidget = this;
  
  this->RendererX = vtkRenderer::New();
  this->RendererX->SetViewport( 0.1, 0.1, 0.13, 0.95 );
  this->RendererX->SetLayer( 1 );
  this->RendererX->InteractiveOff();

  
  this->RendererY = vtkRenderer::New();
  this->RendererY->SetViewport( 0.1, 0.1, 0.95, 0.13 );
  this->RendererY->SetLayer( 1 );
  this->RendererY->InteractiveOff();


  this->OutlineX = vtkPolyData::New();
  this->OutlineX->Allocate();
  vtkPoints *xpoints = vtkPoints::New();
  vtkIdType ptIds[31];
  for( int i=0; i<31; i++)
    ptIds[i] = xpoints->InsertNextPoint( 0, 0, 0 );
  this->OutlineX->SetPoints( xpoints );
  this->OutlineX->InsertNextCell( VTK_POLY_LINE, 31, ptIds );
  
  
  this->OutlineY = vtkPolyData::New();
  this->OutlineY->Allocate();
  vtkPoints *ypoints = vtkPoints::New();
  for( int i=0; i<31; i++)
    ptIds[i] = ypoints->InsertNextPoint( 0, 0, 0 );
  this->OutlineY->SetPoints( ypoints );
  this->OutlineY->InsertNextCell( VTK_POLY_LINE, 31, ptIds );
  

  vtkCoordinate *tcoord = vtkCoordinate::New();
  tcoord->SetCoordinateSystemToDisplay();
  
  vtkPolyDataMapper2D *xmapper = vtkPolyDataMapper2D::New();
  xmapper->SetInput( this->OutlineX );
  xmapper->SetTransformCoordinate( tcoord );
  this->OutlineXActor = vtkActor2D::New();
  this->OutlineXActor->SetMapper( xmapper );
  this->OutlineXActor->SetPosition( 0, 0 );
  this->OutlineXActor->SetPosition2( 1, 1 );
  this->OutlineXActor->GetProperty()->SetColor(0.0,1.0,0.0);
  this->OutlineXActor->PickableOff();

  vtkPolyDataMapper2D *ymapper = vtkPolyDataMapper2D::New();
  ymapper->SetInput( this->OutlineY );
  ymapper->SetTransformCoordinate( tcoord );
  this->OutlineYActor = vtkActor2D::New();
  this->OutlineYActor->SetMapper( ymapper );
  this->OutlineYActor->SetPosition( 0, 0 );
  this->OutlineYActor->SetPosition2( 1, 1 );
  this->OutlineYActor->GetProperty()->SetColor(0.0,1.0,0.0);
  this->OutlineYActor->PickableOff();
  
  xpoints->Delete();
  ypoints->Delete();
  xmapper->Delete();
  ymapper->Delete();
  tcoord->Delete();
}


vtkRulerWidget::~vtkRulerWidget()
{
  this->Observer->Delete();
  this->RendererX->Delete();
  this->RendererY->Delete();
  this->OutlineXActor->Delete();
  this->OutlineX->Delete();
  this->OutlineYActor->Delete();
  this->OutlineY->Delete();
}


void vtkRulerWidget::SetEnabled (int enabling)
{
  if (!this->Interactor)
  {
    vtkErrorMacro("The interactor must be set prior to enabling/disabling widget");
    return;
  }
  
  if (enabling)
  {
    if (this->Enabled)
    {
      return;
    }
    
    
    if (!this->CurrentRenderer)
    {
      this->SetCurrentRenderer( this->Interactor->FindPokedRenderer(
        this->Interactor->GetLastEventPosition()[0],
        this->Interactor->GetLastEventPosition()[1]));

      if (this->CurrentRenderer == NULL)
      {
        return;
      }
    }
    
    this->Enabled = 1;
    
    vtkRenderWindow* renwin = this->CurrentRenderer->GetRenderWindow();
    renwin->AddRenderer( this->RendererX );
    renwin->AddRenderer( this->RendererY );
    if (renwin->GetNumberOfLayers() < 2)
    {
      renwin->SetNumberOfLayers( 2 );
    }

    this->CurrentRenderer->AddViewProp( this->OutlineXActor );
    this->OutlineXActor->VisibilityOn();

    this->CurrentRenderer->AddViewProp( this->OutlineYActor );
    this->OutlineYActor->VisibilityOn();

    vtkCamera* pcam = this->CurrentRenderer->GetActiveCamera();
    vtkCamera* xcam = this->RendererX->GetActiveCamera();
    vtkCamera* ycam = this->RendererY->GetActiveCamera();
    if (pcam && xcam)
    {
      xcam->SetParallelProjection( pcam->GetParallelProjection() );
    }
    if (pcam && ycam)
    {
      ycam->SetParallelProjection( pcam->GetParallelProjection() );
    }
    
    // We need to copy the camera before the compositing observer is called.
    // Compositing temporarily changes the camera to display an image.
    this->StartEventObserverId = this->CurrentRenderer->AddObserver(
      vtkCommand::StartEvent, this->Observer, 1 );
    this->InvokeEvent( vtkCommand::EnableEvent, NULL );
  }
  else
  {
    if (!this->Enabled)
    {
      return;
    }
    
    this->Enabled = 0;

    this->OutlineXActor->VisibilityOff();
    this->CurrentRenderer->RemoveViewProp( this->OutlineXActor );
    this->OutlineYActor->VisibilityOff();
    this->CurrentRenderer->RemoveViewProp( this->OutlineYActor );

    // if the render window is still around, remove our renderer from it
    if (this->CurrentRenderer->GetRenderWindow())
    {
      this->CurrentRenderer->GetRenderWindow()->
        RemoveRenderer( this->RendererX );
      this->CurrentRenderer->GetRenderWindow()->
        RemoveRenderer( this->RendererY );
    }
    if ( this->StartEventObserverId != 0 )
      {
	this->CurrentRenderer->RemoveObserver( this->StartEventObserverId );
      }
    
    this->InvokeEvent( vtkCommand::DisableEvent, NULL );
    this->SetCurrentRenderer( NULL );
  }  
  
}



void vtkRulerWidget::ExecuteCameraUpdateEvent(vtkObject *vtkNotUsed(o),
							  unsigned long vtkNotUsed(event),
							  void *vtkNotUsed(calldata))
{
  if (!this->CurrentRenderer)
  {
    return;
  }
  
  vtkCamera *cam = this->CurrentRenderer->GetActiveCamera();
  double pos[3], fp[3], viewup[3];
  cam->GetPosition( pos );
  cam->GetFocalPoint( fp );
  cam->GetViewUp( viewup );

  double vp[4];
  this->RendererX->GetViewport( vp );
  this->RendererX->NormalizedDisplayToDisplay( vp[0], vp[1] );
  this->RendererX->NormalizedDisplayToDisplay( vp[2], vp[3] );

  vtkPoints *xpoints = this->OutlineX->GetPoints();

  double mid[2]={0.5, 0.5};
  this->RendererX->NormalizedDisplayToDisplay( mid[0], mid[1] );

  double ww1[4] = {0.0, 0.0, 0.0, 1.0};
  double ww2[4] = {10.0, 10.0, 10.0, 1.0};
  double norma = 300; // = ( 3*10*10 )

  this->CurrentRenderer->SetWorldPoint(ww1);
  this->CurrentRenderer->WorldToDisplay();
  this->CurrentRenderer->GetDisplayPoint( ww1 );

  this->CurrentRenderer->SetWorldPoint( ww2 );
  this->CurrentRenderer->WorldToDisplay();
  this->CurrentRenderer->GetDisplayPoint( ww2 );

  vp[0] = 20;
  vp[2] = 30;
  
  double dist = 0.0;
  for (int i=0; i<3; i++) {
	dist += (ww1[i]-ww2[i])*(ww1[i]-ww2[i]);
  }	
	
  double yfactor = sqrt(dist/norma)*10; //fabs(ww2[1]-ww1[1]);
  double ymin = mid[1] - 5.0 * yfactor;
  double ymax = mid[1] + 5.0 * yfactor;
  double xstep = ( vp[2] - vp[0] )/ 3.0;
  double ystep = yfactor;


/*
  double xpoints[31]={vp[2], vp[0], vp[0], vp[0]+xstep, vp[0], vp[0], vp[0]+xstep, vp[0],
		    vp[0], vp[0]+xstep, vp[0], vp[0], vp[0]+xstep, vp[0], vp[0], vp[2], 
		    vp[0], vp[0], vp[0]+xstep, vp[0], vp[0], vp[0]+xstep, vp[0], vp[0], 
		    vp[0]+xstep, vp[0], vp[0], vp[0]+xstep, vp[0], vp[0], vp[2]};
*/

  xpoints->SetPoint( 0, vp[2], ymin, 0 );
  xpoints->SetPoint( 1, vp[0], ymin, 0 );
  
  xpoints->SetPoint( 2, vp[0], mid[1] - 4.0*ystep, 0 );
  xpoints->SetPoint( 3, vp[0]+xstep, mid[1] - 4.0*ystep, 0 );
  xpoints->SetPoint( 4, vp[0], mid[1] - 4.0*ystep, 0 );

  xpoints->SetPoint( 5, vp[0], mid[1] - 3.0*ystep, 0 );
  xpoints->SetPoint( 6, vp[0]+xstep, mid[1] - 3.0*ystep, 0 );
  xpoints->SetPoint( 7, vp[0], mid[1] - 3.0*ystep, 0 );

  xpoints->SetPoint( 8, vp[0], mid[1] - 2.0*ystep, 0 );
  xpoints->SetPoint( 9, vp[0]+xstep, mid[1] - 2.0*ystep, 0 );
  xpoints->SetPoint( 10, vp[0], mid[1] - 2.0*ystep, 0 );

  xpoints->SetPoint( 11, vp[0], mid[1] - 1.0*ystep, 0 );
  xpoints->SetPoint( 12, vp[0]+xstep, mid[1] - 1.0*ystep, 0 );
  xpoints->SetPoint( 13, vp[0], mid[1] - 1.0*ystep, 0 );

  xpoints->SetPoint( 14, vp[0], mid[1], 0 );
  xpoints->SetPoint( 15, vp[2], mid[1], 0 );
  xpoints->SetPoint( 16, vp[0], mid[1], 0 );

  xpoints->SetPoint( 17, vp[0], mid[1] + 1.0*ystep, 0 );
  xpoints->SetPoint( 18, vp[0]+xstep, mid[1] + 1.0*ystep, 0 );
  xpoints->SetPoint( 19, vp[0], mid[1] + 1.0*ystep, 0 );

  xpoints->SetPoint( 20, vp[0], mid[1] + 2.0*ystep, 0 );
  xpoints->SetPoint( 21, vp[0]+xstep, mid[1] + 2.0*ystep, 0 );
  xpoints->SetPoint( 22, vp[0], mid[1] + 2.0*ystep, 0 );

  xpoints->SetPoint( 23, vp[0], mid[1] + 3.0*ystep, 0 );
  xpoints->SetPoint( 24, vp[0]+xstep, mid[1] + 3.0*ystep, 0 );
  xpoints->SetPoint( 25, vp[0], mid[1] + 3.0*ystep, 0 );

  xpoints->SetPoint( 26, vp[0], mid[1] + 4.0*ystep, 0 );
  xpoints->SetPoint( 27, vp[0]+xstep, mid[1] + 4.0*ystep, 0 );
  xpoints->SetPoint( 28, vp[0], mid[1] + 4.0*ystep, 0 );

  xpoints->SetPoint( 29, vp[0], ymax, 0 );
  xpoints->SetPoint( 30, vp[2], ymax, 0 );




  this->RendererY->GetViewport( vp );
  this->RendererY->NormalizedDisplayToDisplay( vp[0], vp[1] );
  this->RendererY->NormalizedDisplayToDisplay( vp[2], vp[3] );

  vp[1] = 20;
  vp[3] = 30;

  vtkPoints *ypoints = this->OutlineY->GetPoints();

	
	double xfactor = yfactor; //sqrt(dist); //fabs(ww2[0]-ww1[0]);
  //double xfactor = fabs(ww2[2]-ww1[2]);
  double xmin = mid[0] - 5.0 * xfactor;
  double xmax = mid[0] + 5.0 * xfactor;

  ystep = ( vp[3] - vp[1] )/ 3.0;
  xstep = xfactor;

  ypoints->SetPoint( 0, xmin, vp[3], 0 );
  ypoints->SetPoint( 1, xmin, vp[1], 0 );
  
  ypoints->SetPoint( 2, mid[0] - 4.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 3, mid[0] - 4.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 4, mid[0] - 4.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 5, mid[0] - 3.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 6, mid[0] - 3.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 7, mid[0] - 3.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 8, mid[0] - 2.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 9, mid[0] - 2.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 10, mid[0] - 2.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 11, mid[0] - 1.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 12, mid[0] - 1.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 13, mid[0] - 1.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 14, mid[0], vp[1], 0 );
  ypoints->SetPoint( 15, mid[0], vp[3], 0 );
  ypoints->SetPoint( 16, mid[0], vp[1], 0 );

  ypoints->SetPoint( 17, mid[0] + 1.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 18, mid[0] + 1.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 19, mid[0] + 1.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 20, mid[0] + 2.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 21,  mid[0] + 2.0*xstep, vp[1]+ystep,0 );
  ypoints->SetPoint( 22, mid[0] + 2.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 23, mid[0] + 3.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 24, mid[0] + 3.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 25, mid[0] + 3.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 26, mid[0] + 4.0*xstep, vp[1], 0 );
  ypoints->SetPoint( 27, mid[0] + 4.0*xstep, vp[1]+ystep, 0 );
  ypoints->SetPoint( 28, mid[0] + 4.0*xstep, vp[1], 0 );

  ypoints->SetPoint( 29,  xmax, vp[1],0 );
  ypoints->SetPoint( 30, xmax, vp[3], 0 );

}
