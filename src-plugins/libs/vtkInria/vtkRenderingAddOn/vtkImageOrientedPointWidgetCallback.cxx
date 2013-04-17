/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkRenderingAddOn/vtkImageOrientedPointWidgetCallback.h>
#include <vtkRenderingAddOn/vtkViewImage2DWithOrientedPoint.h>

#include <vtkImageOrientedPointWidget.h>
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"
#include "vtkImageReslice.h"

#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "vtkRenderWindowInteractor.h"

vtkImageOrientedPointWidgetCallback::vtkImageOrientedPointWidgetCallback()
{
  this->View     = 0;
  this->PathPoly = vtkPolyData::New();
  // this->PositionText  = 0;
  // this->DirectionText = 0;
  this->Points = 0;

  this->Position = new double[3];
  this->Position[0]   = 0;
  this->Position[1]   = 0;
  this->Position[2]   = 0;
  this->Direction = new double[3];
  this->Direction[0]  = 0;
  this->Direction[1]  = 0;
  this->Direction[2]  = 0;
  
}


vtkImageOrientedPointWidgetCallback::~vtkImageOrientedPointWidgetCallback()
{
  this->PathPoly->Delete();
  delete [] this->Position;
  delete [] this->Direction;
  
}


void vtkImageOrientedPointWidgetCallback::SetView (vtkViewImage2DWithOrientedPoint* view)
{
  this->View = view;
}


void
vtkImageOrientedPointWidgetCallback::Execute (vtkObject* caller, unsigned long event, void*)
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

  if( event == vtkCommand::MouseMoveEvent )
  {
    this->OnMouseMoveEvent (caller);
    return;
  }

}

void
vtkImageOrientedPointWidgetCallback::UpdatePoints(vtkObject* caller)
{
//   vtkImageOrientedPointWidget *OrientedPointWidget = reinterpret_cast<vtkImageOrientedPointWidget*>(caller);
  vtkImageOrientedPointWidget *OrientedPointWidget = vtkImageOrientedPointWidget::SafeDownCast(caller);
  if(!OrientedPointWidget)
  { return;
  }

  vtkPolyData* auxPoly = vtkPolyData::New();
  OrientedPointWidget->GetPath(auxPoly);
  
  this->PathPoly->Initialize();
  this->PathPoly->Allocate();
  this->PathPoly->DeepCopy (auxPoly);
  auxPoly->Delete();

  this->Points = this->PathPoly->GetPoints();
  if (!this->Points)
  {
    return;
  }

  int numPoints = this->PathPoly->GetNumberOfPoints();
  /*
  vtkMatrix4x4* transform = this->GetView()->GetImageReslice()->GetResliceAxes();
  double origin[3];
  this->GetView()->GetImageReslice()->GetResliceAxesOrigin (origin);
  */
  for( int i=0; i<numPoints; i++)
  {
    double pt[4];
    this->Points->GetPoint(i, pt);
    pt[3]=0.0;

    double ppt[2] = {pt[0], pt[1]};

    this->GetView()->ScreenCoordinatesToPosition (ppt, pt);
    this->Points->SetPoint (i,pt);
    
    /*
      transform->MultiplyPoint (pt, pt);
      
      pt[0]+=origin[0];
      pt[1]+=origin[1];
      pt[2]+=origin[2];
    */
    
    this->Points->SetPoint (i,pt);
  }

  int Number_of_Points = this->Points->GetNumberOfPoints();

  if (Number_of_Points<1)
  {
    return;
  }
  
  double pt1[4], pt2[4];
  this->Points->GetPoint(0,pt1);
  this->Points->GetPoint(Number_of_Points-1, pt2);

  pt2[0]=pt2[0]-pt1[0];
  pt2[1]=pt2[1]-pt1[1];
  pt2[2]=pt2[2]-pt1[2];
  
  double norm=sqrt (pt2[0]*pt2[0] + pt2[1]*pt2[1] + pt2[2]*pt2[2]);
  if (norm>0)
  {
    pt2[0]=pt2[0]/norm;
    pt2[1]=pt2[1]/norm;
    pt2[2]=pt2[2]/norm;
  }

  this->Position[0]=pt1[0];
  this->Position[1]=pt1[1];
  this->Position[2]=pt1[2];
  this->Direction[0]=pt2[0];
  this->Direction[1]=pt2[1];
  this->Direction[2]=pt2[2];
  
//   wxVtkViewImage2DWithOrientedPoint* Register =
//     reinterpret_cast<wxVtkViewImage2DWithOrientedPoint*>( this->GetView() );
//   if(Register)
//   {
//     Register->ValidateRegistering();
//     Register->Render();
//   }
}

void
vtkImageOrientedPointWidgetCallback::OnEndInteractionEvent(vtkObject* caller)
{
}

void
vtkImageOrientedPointWidgetCallback::OnMouseMoveEvent(vtkObject* caller)
{
  this->UpdatePoints(caller);
  this->RefreshText();
}

void
vtkImageOrientedPointWidgetCallback::OnKeyPressEvent (vtkObject* caller, unsigned char key)
{

//   vtkImageOrientedPointWidget *OrientedPointWidget =
//     reinterpret_cast<vtkImageOrientedPointWidget*>(caller);
//   if(!OrientedPointWidget) return;

//   //int enabled;
  
//   switch(key)
//   {
//     /*
//       case '0':
        
//         enabled = OrientedPointWidget->GetEnabled();
//         OrientedPointWidget->SetEnabled ( (++enabled)%2 );
        
//         return;
//     */
        
//       case 'v':
//       case 'V':
//       case '1':

//         wxVtkViewImage2DWithOrientedPoint* Register =
//           reinterpret_cast<wxVtkViewImage2DWithOrientedPoint*>( this->GetView() );
//         if(Register)
//         {
//           Register->ValidateRegistering();
// 	  Register->Render();
//         }        
        
//         return;
//   }
  
  
//   return;
  
}

void
vtkImageOrientedPointWidgetCallback::RefreshText ()
{

//   if (!this->points)
//     return;
  
  
//   char position[128], direction[128];
//   sprintf (position, "Position %3.1f, %3.1f, %3.1f", (float)this->position[0],
//                                                      (float)this->position[1],
//                                                      (float)this->position[2]);
//   sprintf (direction, "Direction %3.2f, %3.2f, %3.2f", (float)this->direction[0],
//                                                        (float)this->direction[1],
//                                                        (float)this->direction[2]);
//   if (this->PositionText)
//     this->PositionText->SetLabel (position);
  
//   if (this->DirectionText)
//     this->DirectionText->SetLabel (direction);  
}
