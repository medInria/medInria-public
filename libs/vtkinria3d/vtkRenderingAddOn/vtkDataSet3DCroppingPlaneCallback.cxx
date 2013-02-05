/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDataSet3DCroppingPlaneCallback.cxx 1290 2009-10-14 17:02:04Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-10-14 19:02:04 +0200 (Wed, 14 Oct 2009) $
Version:   $Revision: 1290 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkDataSet3DCroppingPlaneCallback.cxx 1290 2009-10-14 17:02:04Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-10-14 19:02:04 +0200 (Wed, 14 Oct 2009) $
Version:   $Revision: 1290 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn
#include <vtkRenderingAddOn/vtkDataSet3DCroppingPlaneCallback.h>

#include <vtkPlaneWidget.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCutter.h>

void vtkDataSet3DCroppingPlaneCallback::Execute ( vtkObject *caller, unsigned long, void* )
{
  
  if( !this->PlaneWidget )
  {
    return;
  }

  std::cout<<"here 1"<<std::endl;
  
  this->PlaneWidget->GetPlane (this->Plane);
  
}


unsigned int vtkDataSet3DCroppingPlaneCallback::AddInput (vtkDataSet* dataset)
{
  if (!dataset)
    return 9999;
  
  if( !this->PlaneWidget )
    return 9999;

  if (!this->Plane)
    this->Plane = vtkPlane::New();
  
  this->PlaneWidget->GetPlane (this->Plane);
  if (!this->Plane)
    return 9999;
  
  vtkCutter* clipper = vtkCutter::New();
  clipper->SetInput (dataset);
  clipper->SetCutFunction (this->Plane);
  clipper->Update();
  std::cout<<"cutting"<<std::endl;

  this->InputList.push_back (dataset);
  if (!clipper->GetOutput())
  {
    return 9999;
  }
  
  this->OutputList.push_back (clipper->GetOutput());
  clipper->GetOutput()->Register(this);
  
  clipper->Delete();
  
  return (this->InputList.size()-1);
  
}

unsigned int  vtkDataSet3DCroppingPlaneCallback::RemoveInput (vtkDataSet* dataset)
{
  return 9999;
}

vtkDataSet* vtkDataSet3DCroppingPlaneCallback::GetInput (unsigned int i)
{
  if (i >= this->InputList.size())
    return NULL;

  return this->InputList[i];
  
}
vtkDataSet* vtkDataSet3DCroppingPlaneCallback::GetOutput (unsigned int i)
{
  
  if (i >= this->OutputList.size())
    return NULL;

  return this->OutputList[i];
  
}
