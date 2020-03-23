/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "vtkDataSet2DWidget.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkCursor2D.h"
#include "vtkProperty2D.h"
#include <vtkCommand.h>
#include <vtkImageView2D.h>
#include <vtkImageActor.h>
#include <vtkPointData.h>
#include <vtkPropCollection.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPlane.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkRenderer.h>
#include <vtkInteractorStyleImageView2D.h>
#include <vtkImageView2DCommand.h>
#include <vtkCutter.h>


vtkStandardNewMacro(vtkDataSet2DWidget);



//----------------------------------------------------------------------------------
vtkDataSet2DWidget::vtkDataSet2DWidget()
{
  this->ImageView = 0;

  this->Source        = 0;
  this->ImplicitPlane = vtkPlane::New();
  this->Cutter        = vtkCutter::New();
  this->Mapper        = vtkPolyDataMapper::New();
  this->Actor         = vtkActor::New();
  this->Renderer      = vtkRenderer::New();  
  this->Command       = vtkDataSet2DWidgetCommand::New();

  this->Cutter->SetCutFunction (this->ImplicitPlane);
  this->Mapper->SetInputConnection (this->Cutter->GetOutputPort());
  this->Actor->SetMapper (this->Mapper);
  this->Renderer->AddViewProp (this->Actor);
  this->Command->SetWidget (this);

  this->Renderer->SetLayer (1);
  this->Renderer->InteractiveOff();
  this->Renderer->SetViewport (0.0, 0.0, 1.0, 1.0);
  this->ImplicitPlane->SetOrigin (0,0,0);
  this->ImplicitPlane->SetNormal (1,0,0);
  this->Enabled = 0;
}

//----------------------------------------------------------------------------------
vtkDataSet2DWidget::~vtkDataSet2DWidget()
{
  this->ImplicitPlane->Delete();
  this->Cutter->Delete();
  this->Mapper->Delete();
  this->Actor->Delete();
  this->Renderer->Delete();
  this->Command->Delete();
}

//----------------------------------------------------------------------
void vtkDataSet2DWidget::SetImageView(vtkImageView2D* arg)
{
  if (this->ImageView && arg != this->ImageView)
  {
    this->ImageView->RemoveObserver (this->Command);
    if (this->ImageView->GetRenderWindow())
      this->ImageView->GetRenderWindow()->RemoveRenderer (this->Renderer);
  }
  
  this->ImageView = arg;
}

//----------------------------------------------------------------------
void vtkDataSet2DWidget::SetSource(vtkPointSet* arg)
{
  this->Source = arg;

  if (this->Source)
    this->Cutter->SetInputData (this->Source);
}

//----------------------------------------------------------------------
void vtkDataSet2DWidget::SetEnabled(int enabling)
{

  this->Enabled = enabling;

  if (enabling)
  {
    
    if( !this->ImageView ||
	!this->ImageView->GetRenderer() ||
	!this->ImageView->GetRenderWindow())
    {
      vtkErrorMacro ( << "Please set Source, View and Renderer before enabling the widget");
      return;
    }

    this->Renderer->SetActiveCamera (this->ImageView->GetRenderer()->GetActiveCamera());
    
    vtkCamera *cam = this->Renderer->GetActiveCamera();    
    double* position = cam->GetPosition();
    double* focalpoint = cam->GetFocalPoint();
    double focaltoposition[3];
    for (unsigned int i=0; i<3; i++)
      focaltoposition[i] = position[i] - focalpoint[i];
    this->ImplicitPlane->SetNormal (focaltoposition);
    this->ImplicitPlane->SetOrigin (focalpoint);

    if (!this->ImageView->HasObserver (vtkImageView2D::SliceChangedEvent, this->Command))
    {
      this->ImageView->AddObserver (vtkImageView2D::CurrentPointChangedEvent, this->Command);
      this->ImageView->AddObserver (vtkImageView2D::SliceChangedEvent, this->Command);
      this->ImageView->AddObserver (vtkImageView2D::OrientationChangedEvent, this->Command);
    }
    
    if (this->ImageView->GetRenderWindow()->GetNumberOfLayers() < 2)
      this->ImageView->GetRenderWindow()->SetNumberOfLayers( 2 );
    this->ImageView->GetRenderWindow()->AddRenderer (this->Renderer);
  }
  else
  {
    if( !this->ImageView )
      return;
    
    this->ImageView->RemoveObserver (this->Command);
    if (this->ImageView->GetInteractorStyle())
      this->ImageView->GetInteractorStyle()->RemoveObserver (this->Command);
    if( this->ImageView->GetRenderWindow() )
      this->ImageView->GetRenderWindow()->RemoveRenderer (this->Renderer);
  }    
}

//----------------------------------------------------------------------------------
void vtkDataSet2DWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------------
void vtkDataSet2DWidgetCommand::Execute(vtkObject *caller,
				     unsigned long event,
				     void *callData)
{

  if (event == vtkImageView2D::SliceChangedEvent ||
      event == vtkImageView2D::CurrentPointChangedEvent )
  {
    if (!this->Widget || !this->Widget->GetImageView())
      return;
    
    this->Widget->GetImplicitPlane()->SetOrigin (this->Widget->GetImageView()->GetCurrentPoint());
  }
  
  if (event == vtkImageView2D::OrientationChangedEvent)
  {
    if (!this->Widget)
      return;
    vtkImageView2D *view = vtkImageView2D::SafeDownCast(caller);
    if (!view)
      return;    
    vtkRenderer* renderer = view->GetRenderer();
    vtkCamera *cam = renderer ? renderer->GetActiveCamera() : NULL;
    if (!cam)
      return;

    this->Widget->GetImplicitPlane()->SetNormal (cam->GetViewPlaneNormal());
  }
  
}
