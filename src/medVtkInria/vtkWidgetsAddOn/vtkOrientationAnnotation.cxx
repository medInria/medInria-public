/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <vtkOrientationAnnotation.h>
#include "vtkObjectFactory.h"
#include "vtkTextProperty.h"
#include "vtkTextMapper.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOrientationAnnotation);
vtkCxxRevisionMacro(vtkOrientationAnnotation, "$Revision: 1284 $");

//----------------------------------------------------------------------------
vtkOrientationAnnotation::vtkOrientationAnnotation()
{
}

//----------------------------------------------------------------------------
vtkOrientationAnnotation::~vtkOrientationAnnotation()
{
}


//----------------------------------------------------------------------------
void vtkOrientationAnnotation::SetTextActorsPosition(int vsize[2])
{
  this->TextActor[2]->SetPosition(5, vsize[1]/2);
  this->TextActor[3]->SetPosition(vsize[0]/2, 5);
  this->TextActor[0]->SetPosition(vsize[0]-5, vsize[1]/2);
  this->TextActor[1]->SetPosition(vsize[0]/2, vsize[1]-5);
}
      
//----------------------------------------------------------------------------
void vtkOrientationAnnotation::SetTextActorsJustification()
{
  vtkTextProperty *tprop = this->TextMapper[2]->GetTextProperty();
  tprop->SetJustificationToLeft();
  tprop->SetVerticalJustificationToCentered();

  tprop = this->TextMapper[3]->GetTextProperty();
  tprop->SetJustificationToCentered();
  tprop->SetVerticalJustificationToBottom();
        
  tprop = this->TextMapper[0]->GetTextProperty();
  tprop->SetJustificationToRight();
  tprop->SetVerticalJustificationToCentered();
        
  tprop = this->TextMapper[1]->GetTextProperty();
  tprop->SetJustificationToCentered();
  tprop->SetVerticalJustificationToTop();
}
