/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "vtkRenderingAddOnExport.h"

#include <vtkCommand.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>


class vtkViewImage2D;


class VTK_RENDERINGADDON_EXPORT vtkImage3DImagePlaneCallback: public vtkCommand
{

 public:
  
  static vtkImage3DImagePlaneCallback* New()
  { return new vtkImage3DImagePlaneCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  /*
  void SetViewImage2D (vtkViewImage2D* view)
  {
    this->ViewImage2D = view;
    this->FirstRender = true;
    }*/

  vtkImageData* GetOutput() const
  {
    return this->Reslice->GetOutput();
  }
  

  virtual void Reset()
  {
    this->Reslice->SetInput (NULL);
    //this->FirstRender = true;
  }

  vtkImageReslice* GetReslice()
  {
    return this->Reslice;
  }
  vtkMatrix4x4* GetMatrix()
  {
    return this->ResliceAxes;
  }
  
  

  
 protected:
 vtkImage3DImagePlaneCallback()
 {
   /*
   this->ViewImage2D = 0;
   this->FirstRender = true;
   */
   this->Reslice     = vtkImageReslice::New();
   this->ResliceAxes = vtkMatrix4x4::New();
 }
 ~vtkImage3DImagePlaneCallback()
 {
   this->Reslice->Delete();
   this->ResliceAxes->Delete();
 }

 private:
 /*
 vtkViewImage2D*   ViewImage2D;
 bool              FirstRender;*/
 
 vtkImageReslice*  Reslice;
 vtkMatrix4x4*     ResliceAxes;
 
};



