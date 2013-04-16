/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


#include <vtkRenderingAddOn/vtkImageTracerWidgetCallback.h>

#include <vtkRenderingAddOn/vtkViewImage2DWithTracer.h>
#include "vtkImageTracerWidget.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "vtkRenderWindowInteractor.h"


vtkImageTracerWidgetCallback::vtkImageTracerWidgetCallback()
{
  this->View     = 0;
  this->PathPoly = vtkPolyData::New();
}


vtkImageTracerWidgetCallback::~vtkImageTracerWidgetCallback()
{
  this->PathPoly->Delete();
}


void vtkImageTracerWidgetCallback::SetView (vtkViewImage2DWithTracer* view)
{
  this->View = view;
}

void
vtkImageTracerWidgetCallback::Execute (vtkObject* caller, unsigned long event, void*)
{
  if( !this->GetView() )
  {
    return;
  }
  
  if(event == vtkCommand::EndInteractionEvent )
  {
    this->OnEndInteractionEvent (caller);
    return;
  }

  if( event == vtkCommand::KeyPressEvent )
  {
    this->OnKeyPressEvent (caller, this->GetView()->GetRenderWindowInteractor()->GetKeyCode() );
    return;
  }
    
}



void
vtkImageTracerWidgetCallback::OnEndInteractionEvent(vtkObject* caller)
{

  vtkImageTracerWidget *tracerWidget =
    reinterpret_cast<vtkImageTracerWidget*>(caller);
  if(!tracerWidget) { return; }

  int closed = tracerWidget->IsClosed();
  if( !closed ) return;


  vtkPolyData* auxPoly = vtkPolyData::New();
  tracerWidget->GetPath(auxPoly);

  
  this->PathPoly->Initialize();
  this->PathPoly->Allocate();
  this->PathPoly->DeepCopy (auxPoly);
  auxPoly->Delete();
  

  vtkPoints* points = this->PathPoly->GetPoints();
  if (!points)
  {
    return;
  }

  int numPoints = this->PathPoly->GetNumberOfPoints();

  
  for( int i=0; i<numPoints; i++)
  {
    double pt[4];
    points->GetPoint(i, pt);
    pt[3]=0.0;

    double ppt[2] = {pt[0], pt[1]};

    this->GetView()->ScreenCoordinatesToPosition (ppt, pt);
    points->SetPoint (i,pt);
  }

  /*
    this->GetView()->AddPolyData ( this->PathPoly);
    this->GetView()->SyncAddDataSet ( this->PathPoly);
    this->GetView()->SyncRender();
  */
}





void
vtkImageTracerWidgetCallback::OnKeyPressEvent (vtkObject* caller, unsigned char key)
{

 
  
  switch(key)
  {
    /*
      case '0':
        
        enabled = tracerWidget->GetEnabled();
        tracerWidget->SetEnabled ( (++enabled)%2 );
        
        return;
    */
      case 'j':

        // status will change
        if( this->GetView()->GetTracerWidget()->GetEnabled() )
        {
          this->GetView()->SetDownRightAnnotation("Tracer: Off");
        }
        else
        {
          this->GetView()->SetDownRightAnnotation("Tracer: On ");
        }
        break;
        
        
      case 'v':
      case 'V':
      case '1':

        vtkViewImage2DWithTracer* tracer =
          static_cast<vtkViewImage2DWithTracer*>( this->GetView() );
        if(tracer)
        {
          tracer->ValidateTracing();
          tracer->Render();
        }

        break;
        
  }
  
  
  return;
  
}
