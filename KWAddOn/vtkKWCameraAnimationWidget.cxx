/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkKWCameraAnimationWidget.cxx 572 2007-12-21 15:52:58Z ntoussaint $
Language:  C++
Author:    $Author: ntoussaint $
Date:      $Date: 2007-12-21 16:52:58 +0100 (Fri, 21 Dec 2007) $
Version:   $Revision: 572 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkKWCameraAnimationWidget.h"
#include "vtkObjectFactory.h"

#include <vtkKWScaleWithEntrySet.h>
#include <vtkKWScaleWithEntry.h>
#include <vtkKWScale.h>

//----------------------------------------------------------------------------

#define VTK_VV_ANIMATION_BUTTON_PREVIEW_ID 0
#define VTK_VV_ANIMATION_BUTTON_CREATE_ID  1
#define VTK_VV_ANIMATION_BUTTON_CANCEL_ID  2

#define VTK_VV_ANIMATION_SCALE_NB_OF_FRAMES_ID 0
#define VTK_VV_ANIMATION_SCALE_SLICE_START_ID  1
#define VTK_VV_ANIMATION_SCALE_SLICE_END_ID    2
#define VTK_VV_ANIMATION_SCALE_AZIMUTH_ID      3
#define VTK_VV_ANIMATION_SCALE_ELEVATION_ID    4
#define VTK_VV_ANIMATION_SCALE_ROLL_ID         5
#define VTK_VV_ANIMATION_SCALE_ZOOM_ID         6

#define VTK_VV_ANIMATION_SCALE_NB_FRAMES       500


//----------------------------------------------------------------------------
vtkStandardNewMacro( vtkKWCameraAnimationWidget );
vtkCxxRevisionMacro( vtkKWCameraAnimationWidget, "$Revision: 1 $");



//----------------------------------------------------------------------------
vtkKWCameraAnimationWidget::vtkKWCameraAnimationWidget()
{
  this->SliceRangeValues[0] = 0;
  this->SliceRangeValues[1] = 20;
  this->RotationValues[0] = 0;
  this->RotationValues[1] = 0;
  this->RotationValues[2] = 0;
  this->ZoomFactorValue = 1;
}


//----------------------------------------------------------------------------
void vtkKWCameraAnimationWidget::Update()
{
  this->Superclass::Update();

  int is_slice = 
    this->AnimationType == vtkKWSimpleAnimationWidget::AnimationTypeSlice;

   if (is_slice)
   {
     vtkKWScale *scale_start = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_SLICE_START_ID)->GetWidget();
     vtkKWScale *scale_end = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_SLICE_END_ID)->GetWidget();
     scale_start->SetValue (this->SliceRangeValues[0]);
     scale_end->SetValue (this->SliceRangeValues[1]);
   }
   else
   {
     vtkKWScale *scale_x = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_AZIMUTH_ID)->GetWidget();
     vtkKWScale *scale_y = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_ELEVATION_ID)->GetWidget();
     vtkKWScale *scale_z = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_ROLL_ID)->GetWidget();
     vtkKWScale *scale_zoom = this->Parameters->GetWidget(VTK_VV_ANIMATION_SCALE_ZOOM_ID)->GetWidget();

     scale_x->SetValue (this->RotationValues[0]);
     scale_y->SetValue (this->RotationValues[1]);
     scale_z->SetValue (this->RotationValues[2]);
     scale_zoom->SetValue (this->ZoomFactorValue);
   }
}

